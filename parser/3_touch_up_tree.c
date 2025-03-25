/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_touch_up_tree.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 23:41:02 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:42:35 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// Maneja nodos tipo PIPE, expandiendo y procesando sus tareas.
// Retorna ALL_OK si se completa correctamente o un código de error en caso contrario.
// Modifica los nodos del árbol al expandir y procesar las tareas.
static t_errors	handle_pipe_node(t_pipe *pipe, char *envp[])
{
	t_errors	error;

	error = expand_task(pipe->left, envp);
	if (error)
		return (error);
	unquote_task((t_task *)pipe->left);
	error = create_heredoc(&(pipe->left->redir));
	if (error)
		return (error);
	error = touch_up_tree((t_tree *)pipe->rigth, envp);
	return (error);
}

// Maneja nodos tipo TASK, expandiendo y procesando sus redirecciones.
// Retorna ALL_OK si se completa correctamente o un código de error en caso contrario.
// Modifica los nodos del árbol al expandir y procesar las redirecciones.
static t_errors	handle_task_node(t_task *task, char *envp[])
{
	t_errors	error;

	error = expand_task(task, envp);
	if (error)
		return (error);
	unquote_task(task);
	error = create_heredoc(&(task->redir));
	return (error);
}

// Realiza ajustes finales en el árbol de comandos.
// Retorna ALL_OK si se completa correctamente o un código de error en caso contrario.
// Modifica los nodos del árbol al realizar los ajustes finales.
t_errors	touch_up_tree(t_tree *node, char *envp[])
{
	t_errors	error;

	if (node->type == PIPE)
	{
		error = handle_pipe_node((t_pipe *)node, envp);
		return (error);
	}
	else if (node->type == TASK)
	{
		error = handle_task_node((t_task *)node, envp);
		return (error);
	}
	return (INVALID_TYPE);
}
