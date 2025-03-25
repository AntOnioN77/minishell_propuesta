/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 13:02:17 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 15:09:19 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

/**
 * handle_sigint - Maneja la señal SIGINT (Ctrl+C).
 * @signal: Número de la señal recibida.
 *
 * Esta función actualiza la variable global `g_ctrlc` con el valor de la señal,
 * escribe un salto de línea en la salida estándar, limpia la línea actual de entrada
 * y redistribuye el prompt. No retorna ningún valor y no modifica argumentos.
 */
void	handle_sigint(int signal)
{
	g_ctrlc = signal;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

/**
 * handle_sigint_heredoc - Maneja SIGINT durante la entrada heredoc.
 * @signal: Número de la señal recibida.
 *
 * Esta función ignora el argumento `signal`, escribe un salto de línea en la salida estándar,
 * limpia la línea actual de entrada, cierra los descriptores de archivo abiertos y termina
 * el proceso con el código de salida `E_SIGINT`.
 */
void	handle_sigint_heredoc(int signal)
{
	(void)signal;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	close_fds(3);
	exit(E_SIGINT);
}

/**
 * signal_conf - Configura el manejo de señales para el shell.
 *
 * Esta función establece un manejador personalizado para SIGINT y configura
 * SIGQUIT para ser ignorado. No retorna ningún valor ni modifica argumentos.
 */
void	signal_conf(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}
