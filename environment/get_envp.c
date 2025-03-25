/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_envp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 21:02:57 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 14:51:39 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <errno.h>

// Obtiene el valor de una variable de entorno por su clave.
// Retorna un puntero al valor de la variable o NULL si no se encuentra.
// No modifica los argumentos externos.
char	*ft_getenv(const char *name, char *envp[])
{
	int		pos;
	char	*word_start;

	pos = search_var(envp, name);
	if (pos == -1)
		return (NULL);
	word_start = ft_strchr(envp[pos], '=') + 1;
	return (word_start);
}
