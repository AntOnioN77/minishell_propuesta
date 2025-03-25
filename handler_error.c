/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_error.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 12:58:36 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 15:10:04 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

/**
 * print_error - Imprime un mensaje de error en la salida estándar de error.
 * @cmd: Comando que causó el error.
 * @error_msg: Mensaje de error asociado.
 *
 * Esta función combina el comando y el mensaje de error en una sola cadena
 * y lo imprime en la salida estándar de error. No retorna ningún valor.
 */
void	print_error(char *cmd, char *error_msg)
{
	char	*msg_error;

	msg_error = ft_strjoin(cmd, error_msg);
	ft_putstr_fd(msg_error, 2);
	free(msg_error);
}

/**
 * ft_perror - Imprime un mensaje de error basado en un código de error.
 * @error: Código de error.
 *
 * Esta función genera un mensaje de error apropiado según el código recibido
 * y lo imprime en la salida estándar de error. No retorna ningún valor.
 */
void	ft_perror(int error)
{
	char	*msg;
	char	*strerr;
	char	*num;

	if (error == SYNTAX_ERROR)
		msg = ft_strdup("minishell: syntax error\n");
	else if (error == LINE_TOO_LONG)
		msg = ft_strdup("minishell: line too long\n");
	else
	{
		num = ft_itoa(error);
		strerr = ft_strjoin(num, "\n");
		free(num);
		msg = ft_strjoin("minishell: ", strerr);
		free(strerr);
	}
	ft_putstr_fd(msg, 2);
	free (msg);
}

/**
 * error_var - Actualiza la variable de error en el entorno.
 * @error: Código de error.
 * @environ: Estructura del entorno del shell.
 *
 * Esta función actualiza la variable `?` en el entorno según el código de error.
 * Modifica el entorno si es necesario. Retorna el código de error recibido.
 */
t_errors	error_var(t_errors error, t_environ *environ)
{
	if (error == CONTINUE || error == TASK_IS_VOID || error == SYNTAX_ERROR
		|| error == LINE_TOO_LONG || error == E_SIGINT)
	{
		if (error == SYNTAX_ERROR)
			change_var("?", "2", environ);
		else if (error == E_SIGINT)
			change_var("?", "130", environ);
		return (error);
	}
	return (0);
}

/**
 * handlerr - Maneja errores y libera recursos.
 * @error: Código de error.
 * @tree: Árbol de comandos a liberar.
 * @environ: Estructura del entorno del shell.
 *
 * Esta función maneja errores, imprime mensajes de error si es necesario,
 * libera el árbol de comandos y el entorno, y finaliza el programa si el error
 * es crítico. Modifica el árbol y el entorno según sea necesario. Retorna el
 * código de error procesado.
 */
t_errors	handlerr(t_errors error, t_tree **tree, t_environ *environ)
{
	if (error == ALL_OK)
		return (0);
	if (error == FINISH)
		error = 0;
	else if (error != TASK_IS_VOID && error != CONTINUE
		&& error != READLINE_FAIL && error != E_SIGINT)
		ft_perror(error);
	if (tree && *tree)
	{
		free_tree(*tree);
		*tree = NULL;
	}
	if (error_var(error, environ))
		return (error);
	if (error == READLINE_FAIL)
		error = (unsigned char) ft_atoi(ft_getenv("?", environ->envp));
	if (environ)
	{
		free_arr(environ->envp);
		ft_bzero(environ, sizeof(t_environ));
	}
	rl_clear_history();
	close_fds(0);
	exit(error);
}
