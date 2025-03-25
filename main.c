/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 20:33:54 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 15:10:07 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"

/**
 * status_control - Actualiza la variable de estado del shell.
 * @status: Código de estado retornado por un proceso hijo.
 * @environ: Estructura del entorno del shell.
 *
 * Esta función calcula el código de estado apropiado, lo convierte a una cadena
 * y actualiza la variable de entorno `?`. Modifica el entorno al cambiar el valor
 * de la variable `?`. No retorna ningún valor.
 */
void	status_control(int status, t_environ *environ)
{
	char	*str_status;

	str_status = NULL;
	if (((((status) & 0x7f) + 1) >> 1) > 0)
		str_status = ft_itoa(((status) & 0x7f) + IS_SIGNAL);
	else
		str_status = ft_itoa(((status) & 0xff00) >> 8);
	change_var("?", str_status, environ);
	free(str_status);
}

/**
 * shell_cycle - Ejecuta un ciclo del shell.
 * @tree: Árbol de comandos a ejecutar.
 * @environ: Estructura del entorno del shell.
 *
 * Esta función configura las señales, construye el árbol de comandos, ejecuta
 * los comandos y maneja el estado del shell. Modifica el árbol de comandos y
 * el entorno según sea necesario. No retorna ningún valor.
 */
void	shell_cycle(t_tree *tree, t_environ *environ)
{
	int		status;

	signal_conf();
	if (handlerr(get_cmd_tree(&tree, environ), &tree, environ))
		return ;
	if (handlerr(non_pipable_builtin(tree, environ), &tree, environ))
		return ;
	if (0 == handlerr(executor(tree, environ, 0, 1), &tree, environ))
	{
		status = wait_all(tree);
		if (g_ctrlc == SIGINT)
		{
			write(1, "\n", 1);
			change_var("?", "130", environ);
		}
		else
			status_control(status, environ);
		close_fds(3);
		free_tree(tree);
	}
}

/**
 * main - Punto de entrada principal del programa.
 * @argc: Número de argumentos pasados al programa.
 * @argv: Lista de argumentos pasados al programa.
 * @envp: Lista de variables de entorno heredadas.
 *
 * Esta función inicializa el entorno del shell, verifica argumentos no válidos
 * y ejecuta el ciclo principal del shell. Retorna 0 si el programa finaliza correctamente,
 * o 1 si hay un error con los argumentos.
 */
int	main(int argc, char **argv, char **envp)
{
	t_tree		*tree;
	t_environ	environ;

	tree = NULL;
	if (argc > 1)
	{
		ft_putstr_fd("Minishell doesn't admit arguments like: ", 2);
		ft_putstr_fd(argv[1], 2);
		return (1);
	}
	handlerr(create_envp(envp, &environ), &tree, &environ);
	while (1)
		shell_cycle(tree, &environ);
	return (0);
}
