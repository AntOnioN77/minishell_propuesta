/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_check_tree.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 00:10:31 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:42:36 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Verifica si una redirección es inválida.
// Retorna 1 si es inválida, 0 en caso contrario.
// No modifica los argumentos externos.
int	bad_redir(t_redir *redir)
{
	if (redir->insymbol && !redir->infoo)
		return (1);
	if (redir->outsymbol && !redir->outfile)
		return (1);
	return (0);
}

// Determina si una tarea es vacía (sin comandos ni redirecciones).
// Retorna 1 si la tarea es vacía, 0 en caso contrario.
// No modifica los argumentos externos.
int	is_void_task(t_task *task)
{
	char	*cmd;
	int		i;

	if (task->cmd == NULL && task->redir.insymbol == 0
		&& task->redir.outsymbol == 0)
		return (1);
	cmd = task->cmd;
	if (!cmd)
		return (1);
	i = 0;
	while (cmd[i] && ft_strchr(WHITESPACES, cmd[i]))
		i++;
	if (!cmd[i])
		return (1);
	return (0);
}

// Verifica la validez de un nodo tipo PIPE en el árbol.
// Retorna ALL_OK si es válido o un código de error en caso contrario.
// No modifica los argumentos externos.
static t_errors	check_pipe(t_tree *tree, char *envp[])
{
	t_pipe		*pipenode;
	t_errors	error;

	pipenode = (t_pipe *)tree;
	if (!pipenode->left)
		return (ERROR_MALLOC);
	error = check_tree((t_tree *)(pipenode->left), envp);
	if (error == TASK_IS_VOID)
		return (SYNTAX_ERROR);
	else if (error)
		return (error);
	if (!pipenode->rigth)
		return (ERROR_MALLOC);
	error = check_tree(pipenode->rigth, envp);
	if (error == TASK_IS_VOID)
	{
		free_tree(pipenode->rigth);
		error = continue_cmd_tree(&(pipenode->rigth), envp);
	}
	return (error);
}

// Verifica la validez de un árbol de comandos.
// Retorna ALL_OK si es válido o un código de error en caso contrario.
// No modifica los argumentos externos.
int	check_tree(t_tree *tree, char **envp)
{
	t_task		*tasknode;
	t_errors	error;

	if (tree == NULL)
		return (ERROR_MALLOC);
	if (tree->type == PIPE)
	{
		error = check_pipe(tree, envp);
		if (error)
			return (error);
	}
	else if (tree->type == TASK)
	{
		tasknode = (t_task *)tree;
		if (bad_redir(&(tasknode->redir)))
			return (SYNTAX_ERROR);
		if (is_void_task(tasknode))
			return (TASK_IS_VOID);
		if (tasknode->redir.error == SYNTAX_ERROR)
			return (SYNTAX_ERROR);
	}
	else
		return (INVALID_TYPE);
	return (0);
}
