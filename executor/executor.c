/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 21:14:05 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 15:00:19 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>

// exec_pipe: Ejecuta un nodo de tipo PIPE en el árbol de ejecución.
// Argumentos:
// - pipe_node: Nodo que representa un pipe.
// - environ: Estructura que contiene el entorno actual.
// - in: Descriptor de archivo de entrada.
// Retorno:
// - Devuelve 0 si la operación es exitosa.
// - Devuelve un código de error si ocurre un fallo.
t_errors	exec_pipe(t_pipe *pipe_node, t_environ *environ, int in)
{
	int			pipefd[2];
	t_errors	err;

	pipe(pipefd);
	if (pipe_node->left)
	{
		err = executor((t_tree *)pipe_node->left, environ, in, pipefd[1]);
		if (err != 0)
			return (err);
	}
	if (pipe_node->rigth)
	{
		err = executor(pipe_node->rigth, environ, pipefd[0], STDOUT_FILENO);
		if (err != 0)
			return (err);
	}
	return (0);
}

// executor: Ejecuta un nodo del árbol de ejecución (comando o pipe).
// Argumentos:
// - node: Nodo del árbol de ejecución.
// - environ: Estructura que contiene el entorno actual.
// - in: Descriptor de archivo de entrada.
// - out: Descriptor de archivo de salida.
// Retorno:
// - Devuelve 0 si la operación es exitosa.
// - Devuelve un código de error si ocurre un fallo.
t_errors	executor(t_tree *node, t_environ *environ, int in, int out)
{
	t_pipe		*pipe_node;
	t_task		*task;
	t_errors	err;

	if (!node)
		return (0);
	if (node->type == PIPE)
	{
		pipe_node = (t_pipe *)node;
		err = exec_pipe(pipe_node, environ, in);
		if (err != 0)
			return (err);
	}
	else if (node->type == TASK)
	{
		task = (t_task *)node;
		err = create_child(task, environ, in, out);
		if (err != 0)
			return (err);
	}
	return (0);
}
