/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 21:14:20 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 15:00:17 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include <sys/stat.h>
#include <stdio.h>

// child_error_handler: Maneja errores específicos de un proceso hijo.
// Argumentos:
// - err: Código de error.
// - cmd: Comando que causó el error.
// Retorno:
// - Devuelve el código de error modificado si aplica.
// - Devuelve 0 si no hay error.
static t_errors	child_error_handler(t_errors err, char *cmd)
{
	if (err != 0)
	{
		if (cmd == NULL)
			return (child_error_handler_fail);
		if (err == COM_NOT_FOUND)
			print_error(cmd, ": Command not found\n");
		else if (err == IS_A_DIR)
			print_error(cmd, ": Is a directory\n");
		else if (err == NO_PERMISSION)
		{
			err = 126;
			print_error(cmd, ": Permission denied\n");
		}
		else if (err == NO_EXIST)
		{
			err = 127;
			print_error(cmd, ": No such file or directory\n");
		}
		else
			perror("minishell");
		close_fds(0);
		return (err);
	}
	return (0);
}

// repipe_child: Configura redirecciones de entrada/salida para un proceso hijo.
// Argumentos:
// - task: Nodo de tarea que contiene las redirecciones.
// - in: Descriptor de archivo de entrada.
// - out: Descriptor de archivo de salida.
// - word_fail: Puntero para almacenar el nombre del archivo que falló.
// Retorno:
// - Devuelve 0 si la operación es exitosa.
// - Devuelve un código de error si ocurre un fallo.
t_errors	repipe_child(t_task *task, int in, int out, char **word_fail)
{
	t_errors	err;

	if (out != STDOUT_FILENO)
	{
		dup2(out, STDOUT_FILENO);
		close(out);
	}
	if (in != STDIN_FILENO)
	{
		dup2(in, STDIN_FILENO);
		close(in);
	}
	close(3);
	close(4);
	err = apply_redirs(&(task->redir), word_fail);
	if (err != 0)
		return (err);
	return (0);
}

// child: Ejecuta un proceso hijo para un comando o builtin.
// Argumentos:
// - task: Nodo de tarea que contiene el comando y sus argumentos.
// - environ: Estructura que contiene el entorno actual.
// - in: Descriptor de archivo de entrada.
// - out: Descriptor de archivo de salida.
// Retorno:
// - Devuelve 0 si la operación es exitosa.
// - Devuelve un código de error si ocurre un fallo.
t_errors	child(t_task *task, t_environ *environ, int in, int out)
{
	t_errors	err;
	char		*pathcmd;
	char		*word_fail;
	char		**envp;

	envp = environ->envp;
	word_fail = NULL;
	signal(SIGINT, SIG_DFL);
	err = repipe_child(task, in, out, &word_fail);
	if (child_error_handler(err, word_fail))
		return (1);
	if (is_builtin(task->cmd))
		return (builtins_exe(task, environ));
	pathcmd = com_path(task->cmd, envp, &err);
	if (err)
		return (child_error_handler(err, task->cmd));
	rl_clear_history();
	execve(pathcmd, task->argv, envp);
	err = errno;
	free(pathcmd);
	return (err);
}

// create_child: Crea un proceso hijo para ejecutar un comando o builtin.
// Argumentos:
// - task: Nodo de tarea que contiene el comando y sus argumentos.
// - environ: Estructura que contiene el entorno actual.
// - in: Descriptor de archivo de entrada.
// - out: Descriptor de archivo de salida.
// Retorno:
// - Devuelve 0 si la operación es exitosa.
// - Devuelve un código de error si ocurre un fallo.
t_errors	create_child(t_task *task, t_environ *environ, int in, int out)
{
	int			pid;
	t_errors	err;

	err = 0;
	pid = fork();
	if (pid == -1)
		return (errno);
	task->pid = pid;
	if (pid == 0)
	{
		err = child(task, environ, in, out);
		if (err)
			return (err);
	}
	else
		signal(SIGINT, SIG_IGN);
	if (out != STDOUT_FILENO)
		close(out);
	if (in != STDIN_FILENO)
		close(in);
	return (err);
}
