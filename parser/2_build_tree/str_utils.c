/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 12:03:40 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:47:22 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Verifica si un carácter es un delimitador.
// Retorna 1 si el carácter es un delimitador, 0 en caso contrario.
// No modifica los argumentos externos.
int	isdelimiter(char c)
{
	if (ft_strchr(DELIMITERS, c))
		return (1);
	return (0);
}

// Reemplaza los delimitadores en una cadena con caracteres nulos.
// No retorna valor.
// Modifica la cadena `str` directamente, reemplazando los delimitadores con '\0'.
void	nullify_delimiters(char *str)
{
	if (!str)
		return ;
	while (*str)
	{
		skip_quotes(&str, str + ft_strlen(str));
		if (*str == '\0')
			break ;
		if (isdelimiter(*str))
			*str = '\0';
		str++;
	}
}

// Omite los espacios en blanco en un segmento de texto.
// No retorna valor.
// Modifica el puntero `*segment` para apuntar al primer carácter no blanco.
void	skipwhitesp(char **segment, char *end)
{
	while (*segment != end && ft_strchr(WHITESPACES, **segment))
		(*segment)++;
	return ;
}

// Omite las comillas en una cadena.
// Retorna 1 si se omitieron comillas, 0 en caso contrario.
// Modifica el puntero `*strpnt` para avanzar más allá de las comillas.
int	skip_quotes(char **strpnt, char *end)
{
	char	*tmp;

	tmp = *strpnt;
	while (**strpnt == '"' || **strpnt == 39)
	{
		if (**strpnt == '"' && ft_strnchr((*strpnt) + 1, '"', (end - *strpnt)))
			*strpnt = 1 + ft_strnchr(*strpnt +1, '"', (end - *strpnt));
		else if (**strpnt == 39 && ft_strnchr(*strpnt +1, 39, (end - *strpnt)))
			*strpnt = 1 + ft_strnchr(*strpnt +1, 39, (end - *strpnt));
		else
			break ;
	}
	if (*strpnt != tmp)
		return (1);
	return (0);
}

// Busca un carácter fuera de comillas en un segmento de texto.
// Retorna 1 si se encuentra el carácter, 0 en caso contrario.
// Modifica el puntero `*str` para apuntar a la posición del carácter encontrado.
int	strnchr_outquot(char **str, char *end, char c)
{
	char	*strpnt;

	strpnt = *str;
	while (strpnt <= end && *strpnt != '\0')
	{
		if (*strpnt == c)
		{
			*str = strpnt;
			return (1);
		}
		if (!skip_quotes(&strpnt, end))
			strpnt++;
	}
	return (0);
}
