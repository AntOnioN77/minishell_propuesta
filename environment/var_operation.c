/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_operation.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 21:02:47 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 14:55:24 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <errno.h>

// custom_realloc: Realiza una reasignación de memoria para un puntero dado.
// Argumentos:
// - pnt: Doble puntero al bloque de memoria que se desea reasignar.
// - oldsize: Tamaño actual del bloque de memoria.
// - newsize: Nuevo tamaño deseado para el bloque de memoria.
// Retorno:
// - Devuelve el nuevo puntero al bloque de memoria reasignado.
// - Si ocurre un error, devuelve NULL.
// Nota: Modifica el valor del puntero original (*pnt) para que apunte al nuevo bloque.
void	*custom_realloc(void **pnt, size_t oldsize, size_t newsize)
{
	void	*newpnt;

	if (*pnt == NULL)
		return (NULL);
	if (oldsize >= newsize)
		return (*pnt);
	newpnt = ft_calloc(1, newsize);
	if (newpnt == NULL)
		return (NULL);
	ft_memcpy(newpnt, *pnt, oldsize);
	free(*pnt);
	*pnt = newpnt;
	return (newpnt);
}

// var_buid: Construye una nueva variable de entorno en formato "clave=valor".
// Argumentos:
// - key: Clave de la variable de entorno.
// - newvalue: Valor asociado a la clave.
// Retorno:
// - Devuelve un puntero a la nueva cadena "clave=valor".
// - Si ocurre un error, devuelve NULL.
char	*var_buid(char *key, char *newvalue)
{
	size_t	len;
	char	*newvar;

	len = ft_strlen(key) + ft_strlen(newvalue) + 2;
	newvar = ft_calloc(len, sizeof(char));
	if (newvar == NULL)
		return (NULL);
	ft_strlcpy(newvar, key, len);
	ft_strlcat(newvar, "=", len);
	ft_strlcat(newvar, newvalue, len);
	return (newvar);
}

// change_var: Cambia el valor de una variable de entorno existente.
// Argumentos:
// - key: Clave de la variable de entorno a modificar.
// - newvalue: Nuevo valor para la variable de entorno.
// - environ: Estructura que contiene el entorno actual (modifica su contenido).
// Retorno:
// - Devuelve 0 si la operación es exitosa.
// - Devuelve 1 si no se encuentra la clave o si hay argumentos inválidos.
// - Devuelve un código de error (errno) si ocurre un fallo al asignar memoria.
t_errors	change_var(char *key, char *newvalue, t_environ *environ)
{
	size_t	keylen;
	char	*newvar;
	int		i;
	char	**envp;

	if (!key || !newvalue || !environ)
		return (1);
	envp = environ->envp;
	keylen = ft_strlen(key);
	newvar = var_buid(key, newvalue);
	if (!newvar)
		return (errno);
	i = 0;
	while (i < environ->next)
	{
		if (ft_strncmp(envp[i], key, keylen) == 0)
		{
			free(envp[i]);
			envp[i] = newvar;
			return (0);
		}
		i++;
	}
	free(newvar);
	return (1);
}

// add_var: Añade una nueva variable de entorno al entorno actual.
// Argumentos:
// - key: Clave de la nueva variable de entorno.
// - value: Valor asociado a la clave.
// - environ: Estructura que contiene el entorno actual (modifica su contenido).
// Retorno:
// - Devuelve 0 si la operación es exitosa.
// - Devuelve 1 si los argumentos son inválidos.
// - Devuelve un código de error (errno) si ocurre un fallo al asignar memoria.
t_errors	add_var(char *key, char *value, t_environ *environ)
{
	size_t	len;
	char	*newvar;

	if (key == NULL || value == NULL || key[0] == '\0')
		return (1);
	if (environ->next >= (environ->alloced - 1))
	{
		if (!custom_realloc((void **) &(environ->envp)
				, environ->alloced * sizeof(char *)
				, environ->alloced * 2 * sizeof(char *)))
			return (errno);
		environ->alloced = environ->alloced * 2;
	}
	len = ft_strlen(key) + ft_strlen(value) + 2;
	newvar = ft_calloc(len, sizeof(char));
	if (newvar == NULL)
		return (errno);
	ft_strlcpy(newvar, key, len);
	ft_strlcat(newvar, "=", len);
	ft_strlcat(newvar, value, len);
	environ->envp[environ->next] = newvar;
	environ->next = environ->next + 1;
	environ->envp[environ->next] = NULL;
	return (0);
}

// search_var: Busca una variable de entorno en el arreglo envp.
// Argumentos:
// - envp: Arreglo de cadenas que representa el entorno actual.
// - var: Nombre de la variable de entorno a buscar.
// Retorno:
// - Devuelve la posición (índice) de la variable en el arreglo si se encuentra.
// - Devuelve -1 si no se encuentra o si envp es NULL.
int	search_var(char **envp, const char *var)
{
	int		pos;
	size_t	len;

	if (!envp)
		return (-1);
	pos = 0;
	while (envp[pos])
	{
		len = ft_strchr(envp[pos], '=') - envp[pos];
		if (ft_strncmp(envp[pos], var, len) == 0 && len == ft_strlen(var))
		{
			return (pos);
		}
		pos++;
	}
	return (-1);
}
