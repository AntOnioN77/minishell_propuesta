/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 21:12:48 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 15:00:22 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../executor.h"
#include <sys/stat.h>

// close_fds: Cierra todos los descriptores de archivo abiertos a partir de un número dado.
// Argumentos:
// - fd_since: Número del descriptor de archivo desde el cual comenzar a cerrar.
// Retorno:
// - No tiene retorno.
// Nota: Esta función no modifica los argumentos.
void	close_fds(int fd_since)
{
	struct stat	statbuf;

	while (fstat(fd_since, &statbuf) != -1)
	{
		close(fd_since);
		fd_since++;
	}
}

// wait_all: Espera a que todos los procesos hijos terminen su ejecución.
// Argumentos:
// - node: Nodo del árbol de ejecución que representa un comando o un pipe.
// Retorno:
// - Devuelve el estado de salida del último proceso hijo esperado.
// Nota: Modifica la variable global `g_ctrlc` si se detecta una interrupción (SIGINT).
int	wait_all(t_tree *node)
{
	int		status;
	t_task	*task;
	t_pipe	*pipe_node;

	status = 0;
	if (!node)
		return (0);
	if (node->type == PIPE)
	{
		pipe_node = (t_pipe *) node;
		status = wait_all(pipe_node->rigth);
		wait_all((t_tree *) pipe_node->left);
	}
	else if (node->type == TASK)
	{
		task = (t_task *) node;
		waitpid(task->pid, &status, 0);
		if ((((signed char)(((status) & 0x7f) + 1) >> 1) > 0)
			&& ((status) & 0x7f) == SIGINT)
			g_ctrlc = SIGINT;
		close_fds(3);
	}
	return (status);
}
