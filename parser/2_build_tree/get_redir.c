/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 12:11:59 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:47:20 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <errno.h>

// Maneja redirecciones de tipo ">>" (append).
// No retorna valor.
// Modifica `redir` para establecer el tipo de redirección y el archivo asociado.
static void	handle_append(char **segment, char *end, t_redir *redir)
{
	(*segment) += 2;
	if (redir)
	{
		create_file(*segment, end, O_APPEND, redir);
		redir->outsymbol = append;
		getpntword(segment, end, &(redir->outfile));
	}
	else
		getpntword(segment, end, NULL);
	return ;
}

// Maneja redirecciones de tipo "<<" (heredoc).
// No retorna valor.
// Modifica `redir` para establecer el tipo de redirección y el separador asociado.
static void	handle_heredoc(char **segment, char *end, t_redir *redir)
{
	(*segment) += 2;
	if (redir)
	{
		redir->insymbol = heredoc;
		getpntword(segment, end, &(redir->infoo));
		if (!redir->infoo || *(redir->infoo) == '\0')
			redir->error = SYNTAX_ERROR;
	}
	else
		getpntword(segment, end, NULL);
	return ;
}

// Maneja redirecciones de entrada "<".
// No retorna valor.
// Modifica `redir` para establecer el tipo de redirección y el archivo asociado.
static void	handle_input(char **segment, char *end, t_redir *redir)
{
	(*segment)++;
	if (redir)
	{
		check_file(*segment, end, redir);
		redir->insymbol = infile;
		getpntword(segment, end, &(redir->infoo));
	}
	else
		getpntword(segment, end, NULL);
}

// Maneja redirecciones de salida ">".
// No retorna valor.
// Modifica `redir` para establecer el tipo de redirección y el archivo asociado.
static void	handle_output(char **segment, char *end, t_redir *redir)
{
	{
		(*segment)++;
		if (redir)
		{
			create_file(*segment, end, O_TRUNC, redir);
			redir->outsymbol = outfile;
			getpntword(segment, end, &(redir->outfile));
		}
		else
			getpntword(segment, end, NULL);
		return ;
	}
}

// Procesa las redirecciones en un segmento de texto.
// No retorna valor.
// Modifica `redir` para incluir las redirecciones encontradas en el segmento.
void	get_redir(char **segment, char *end, t_redir *redir)
{
	while (*segment < end)
	{
		if (redir && !(redir->error == ALL_OK))
		{
			redir = NULL;
		}
		skipwhitesp(segment, end);
		if (*segment == ft_strnstr(*segment, "<<", end - *segment))
			handle_heredoc(segment, end, redir);
		else if (*segment == ft_strnstr(*segment, ">>", end - *segment))
			handle_append(segment, end, redir);
		else if (**segment == '<')
			handle_input(segment, end, redir);
		else if (**segment == '>')
			handle_output(segment, end, redir);
		else
			return ;
	}
	return ;
}
