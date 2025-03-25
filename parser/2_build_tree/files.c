/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 00:01:59 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:47:18 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <errno.h>

// Busca caracteres específicos en una cadena dentro de un rango.
// Retorna un puntero al primer carácter encontrado o al final del rango si no se encuentra.
// No modifica los argumentos externos.
char	*findchars(char *str, char *end, char *wanted)
{
	char	*ptr;
	char	*w;

	if (!str || !end || !wanted)
		return (end);
	ptr = str;
	while (ptr < end)
	{
		w = wanted;
		while (*w != '\0')
		{
			if (*ptr == *w)
				return (ptr);
			w++;
		}
		ptr++;
	}
	return (end);
}

// Verifica si un archivo existe y tiene permisos de lectura.
// No retorna valor.
// Modifica `redir->error` si el archivo no existe o no tiene permisos.
void	check_file(char *segment, char *end, t_redir *redir)
{
	char	*file;

	if (!redir)
		return ;
	file = ft_substr(segment, 0, findchars(segment, end, DELIMITERS) - segment);
	unquote(file);
	if (access(file, F_OK) || access(file, R_OK))
		redir->error = NO_PERMISSION;
	free(file);
}

// Crea un archivo si no existe y verifica permisos de escritura.
// No retorna valor.
// Modifica `redir->error` si el archivo no tiene permisos de escritura.
void	create_file(char *segment, char *end, int flag, t_redir *redir)
{
	int		fd;
	char	*file;

	if (!redir)
		return ;
	file = ft_substr(segment, 0, findchars(segment, end, DELIMITERS) - segment);
	unquote(file);
	if (!access(file, F_OK) && access(file, W_OK) == -1)
		redir->error = NO_PERMISSION;
	else
	{
		fd = open(file, O_WRONLY | O_CREAT | flag, 0664);
		close(fd);
	}
	free(file);
}
