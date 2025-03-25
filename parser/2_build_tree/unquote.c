/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unquote.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 23:45:03 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:47:09 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// Procesa una cadena para eliminar comillas de un segmento.
// Retorna un puntero actualizado a la posición siguiente en la cadena original.
// Modifica `*pntnew` para incluir el contenido sin comillas.
char	*unquote_one(char **pntnew, char *pntstr)
{
	size_t	quotlen;

	if ((*pntstr == 39 || *pntstr == '"') && ft_strchr(pntstr +1, *pntstr))
	{
		quotlen = ft_strchr(pntstr +1, *pntstr) - (pntstr +1);
		ft_strlcpy(*pntnew, pntstr +1, quotlen +1);
		*pntnew = *pntnew + quotlen;
		pntstr = ft_strchr(pntstr +1, *pntstr);
	}
	else
	{
		ft_strlcpy(*pntnew, pntstr, 2);
		(*pntnew)++;
	}
	pntstr++;
	return (pntstr);
}

// Elimina las comillas de una cadena completa.
// No retorna valor.
// Modifica la cadena `str` directamente, reemplazándola con su versión sin comillas.
void	unquote(char *str)
{
	char	*newstr;
	char	*pntnew;
	char	*pntstr;

	pntstr = str;
	if (!str || !*str)
		return ;
	newstr = ft_calloc(ft_strlen(pntstr) + 1, sizeof(char));
	pntnew = newstr;
	while (*pntstr)
	{
		pntstr = unquote_one(&pntnew, pntstr);
	}
	ft_strlcpy(str, newstr, 1 + ft_strlen(str));
	free(newstr);
}

// Elimina las comillas de los elementos de una tarea.
// No retorna valor.
// Modifica los campos `cmd`, `redir.infoo`, `redir.outfile` y los elementos de `argv` en `node`.
void	unquote_task(t_task *node)
{
	int	i;

	unquote(node->cmd);
	unquote(node->redir.infoo);
	unquote(node->redir.outfile);
	i = 0;
	while ((node->argv)[i])
	{
		unquote((node->argv)[i]);
		i++;
	}
	return ;
}
