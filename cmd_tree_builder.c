/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_tree_builder.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 20:44:27 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 15:10:05 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

/**
 * continue_cmd_tree - Continúa construyendo el árbol de comandos.
 * @right: Puntero al nodo derecho del árbol.
 * @envp: Variables de entorno.
 *
 * Esta función solicita una nueva línea de entrada, la procesa y construye
 * un árbol de comandos. Modifica el nodo derecho del árbol y puede retornar
 * un código de error si ocurre algún problema.
 */
t_errors	continue_cmd_tree(t_tree **right, char **envp)
{
	char		*line;
	t_errors	error;

	line = readline("> ");
	if (!line)
		return (READLINE_FAIL);
	if (line[0] == '\0')
	{
		free(line);
		return (continue_cmd_tree(right, envp));
	}
	add_history(line);
	*right = build_tree(line);
	if (*right == NULL)
	{
		free(line);
		perror("build_tree:");
		rl_clear_history();
		return (ERROR_MALLOC);
	}
	(*right)->line_extra = line;
	error = touch_up_tree(*right, envp);
	if (error)
		return (error);
	return (check_tree(*right, envp));
}

/**
 * line_control - Valida y procesa la línea de entrada.
 * @line: Línea de entrada del usuario.
 * @environ: Estructura del entorno del shell.
 *
 * Esta función valida la longitud de la línea, actualiza la variable `?` si
 * se interrumpió con SIGINT, y agrega la línea al historial si es válida.
 * Modifica el entorno si es necesario. Retorna un código de error o 0 si no hay errores.
 */
t_errors	line_control(char *line, t_environ *environ)
{
	if (g_ctrlc == 2)
		change_var("?", "130", environ);
	if (!line)
		return (READLINE_FAIL);
	if (ft_strlen(line) >= S_LINE_MAX)
	{
		free(line);
		return (LINE_TOO_LONG);
	}
	if (*line)
		add_history(line);
	return (0);
}

/**
 * get_cmd_tree - Construye el árbol de comandos a partir de la entrada del usuario.
 * @tree: Puntero al árbol de comandos.
 * @environ: Estructura del entorno del shell.
 *
 * Esta función solicita una línea de entrada, la valida y construye un árbol
 * de comandos. Modifica el árbol y el entorno según sea necesario. Retorna
 * un código de error si ocurre algún problema.
 */
t_errors	get_cmd_tree(t_tree **tree, t_environ *environ)
{
	char		*line;
	char		**envp;
	t_errors	error;

	envp = environ->envp;
	g_ctrlc = 0;
	line = readline("mini$hell> ");
	error = line_control(line, environ);
	if (error)
		return (error);
	*tree = build_tree(line);
	if (*tree == NULL)
	{
		free(line);
		perror("build_tree:");
		rl_clear_history();
		return (ERROR_MALLOC);
	}
	(*tree)->line = line;
	error = touch_up_tree(*tree, envp);
	if (error)
		return (error);
	return (check_tree(*tree, envp));
}
