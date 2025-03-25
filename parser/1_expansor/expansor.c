/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 22:19:40 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:47:13 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// Maneja la expansión de una variable en una cadena.
// Retorna ALL_OK si se completa correctamente o ERROR_MALLOC en caso de error.
// Modifica `*marker` para avanzar más allá de la variable expandida y `*newline` para incluir el valor expandido.
t_errors	handle_expansion(char **marker, char **newline, char *buffer,
char **envp)
{
	int	expanded;

	expanded = expand_one(newline, buffer, *marker, envp);
	if (expanded >= 0)
		*marker += expanded;
	else
	{
		return (ERROR_MALLOC);
	}
	if (!*newline)
		return (ERROR_MALLOC);
	ft_bzero(buffer, BUFFER_SIZE);
	return (ALL_OK);
}

// Agrega caracteres al buffer y lo une a la nueva línea si es necesario.
// Retorna ALL_OK si se completa correctamente o ERROR_MALLOC en caso de error.
// Modifica `*newline` para incluir el contenido del buffer y `*marker` para avanzar en la cadena.
static t_errors	append_to_buffer(char **newline, char *buffer,
	char **marker, int *i)
{
	if (*i == BUFFER_SIZE - 1)
	{
		if (join_line_buffer(newline, buffer))
			return (ERROR_MALLOC);
		*i = 0;
	}
	buffer[*i] = **marker;
	(*i)++;
	(*marker)++;
	return (ALL_OK);
}

// Motor principal para expandir variables en una cadena.
// Retorna ALL_OK si se completa correctamente o ERROR_MALLOC en caso de error.
// Modifica `*origin` para incluir la cadena expandida.
t_errors	expandstr_motor(char **origin, char **newline, char *buffer,
	char *envp[])
{
	int		quoted;
	char	*marker;
	int		i;

	quoted = 0;
	marker = *origin;
	i = 0;
	while (*marker)
	{
		quoted = is_quoted(marker, quoted);
		if (quoted != 1 && *marker == '$')
		{
			if (handle_expansion(&marker, newline, buffer, envp))
				return (ERROR_MALLOC);
			i = 0;
		}
		else
		{
			if (append_to_buffer(newline, buffer, &marker, &i))
				return (ERROR_MALLOC);
		}
	}
	return (0);
}

// Expande variables en una cadena y las almacena en la estructura de basura.
// Retorna ALL_OK si se completa correctamente o ERROR_MALLOC en caso de error.
// Modifica `*origin` para incluir la cadena expandida y `garbage` para almacenar punteros a memoria dinámica.
t_errors	expandstr(char **origin, t_garbage *garbage, char *envp[])
{
	char	buffer[BUFFER_SIZE];
	char	*newline;

	if (!is_expansible(*origin))
		return (ALL_OK);
	ft_bzero(buffer, BUFFER_SIZE);
	newline = ft_strdup("");
	if (!newline)
		return (ERROR_MALLOC);
	if (expandstr_motor(origin, &newline, buffer, envp))
	{
		free(newline);
		return (ERROR_MALLOC);
	}
	if (join_line_buffer(&newline, buffer))
	{
		free(newline);
		return (ERROR_MALLOC);
	}
	garbage->pointers[garbage->current] = newline;
	garbage->current++;
	*origin = newline;
	return (ALL_OK);
}

// Expande todas las variables en una tarea.
// Retorna ALL_OK si se completa correctamente o ERROR_MALLOC en caso de error.
// Modifica los campos `cmd`, `redir.infoo`, `redir.outfile` y los elementos de `argv` en `node`.
t_errors	expand_task(t_task *node, char *envp[])
{
	int	i;

	node->garb.size = count_expansions(((t_task *)node)) + 1;
	node->garb.pointers = ft_calloc(node->garb.size, sizeof(void *));
	if (node->garb.pointers == NULL)
		return (1);
	(node->garb.pointers)[node->garb.size -1] = NULL;
	if (expandstr(&(node->cmd), &(node->garb), envp))
		return (1);
	if (node->redir.insymbol != heredoc)
	{
		if (expandstr(&(node->redir.infoo), &(node->garb), envp))
			return (1);
	}
	if (expandstr(&(node->redir.outfile), &(node->garb), envp))
		return (1);
	i = 0;
	while ((node->argv)[i])
	{
		if (expandstr((&(node->argv)[i]), &(node->garb), envp))
			return (1);
		i++;
	}
	return (0);
}
