/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 21:13:24 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 15:00:20 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "executor.h"
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>

// form_path: Construye una ruta completa combinando un directorio y un comando.
// Argumentos:
// - cmd: Nombre del comando.
// - enpath: Lista de rutas de directorios del PATH.
// - err: Puntero para almacenar errores.
// - i: Índice del directorio actual en `enpath`.
// Retorno:
// - Devuelve la ruta completa del comando si tiene éxito.
// - Devuelve NULL si ocurre un error (modifica `err` en consecuencia).
static char	*form_path(char *cmd, char **enpath, t_errors *err, int i)
{
	char	*pathcmd;
	char	*slash;

	slash = ft_strjoin(enpath[i], "/");
	if (slash == NULL)
	{
		*err = ERROR_MALLOC;
		free_null_arr(&enpath);
		return (NULL);
	}
	pathcmd = ft_strjoin(slash, cmd);
	free(slash);
	if (pathcmd == NULL)
	{
		*err = ERROR_MALLOC;
		free_null_arr(&enpath);
		return (NULL);
	}
	return (pathcmd);
}

// match_pathcmd: Busca el comando en las rutas del PATH y verifica permisos.
// Argumentos:
// - cmd: Nombre del comando.
// - enpath: Lista de rutas de directorios del PATH.
// - err: Puntero para almacenar errores.
// Retorno:
// - Devuelve la ruta completa del comando si se encuentra y tiene permisos de ejecución.
// - Devuelve NULL si no se encuentra o si ocurre un error (modifica `err` en consecuencia).
static char	*match_pathcmd(char *cmd, char **enpath, t_errors *err)
{
	char	*pathcmd;
	int		i;

	i = 0;
	while (enpath[++i])
	{
		pathcmd = form_path(cmd, enpath, err, i);
		if (pathcmd == NULL)
			return (NULL);
		if (access(pathcmd, F_OK) == 0)
		{
			free_null_arr(&enpath);
			if (access(pathcmd, X_OK) == 0)
				return (pathcmd);
			*err = NO_PERMISSION;
			free(pathcmd);
			return (NULL);
		}
		free(pathcmd);
	}
	free_null_arr(&enpath);
	*err = COM_NOT_FOUND;
	return (NULL);
}

// get_index_path: Encuentra el índice de la variable PATH en el entorno.
// Argumentos:
// - envp: Arreglo de cadenas que representan el entorno actual.
// Retorno:
// - Devuelve el índice de la variable PATH si se encuentra.
// - Devuelve -1 si no se encuentra o si ocurre un error.
int	get_index_path(char **envp)
{
	int		pos;
	char	*subpath;

	pos = 0;
	while (envp[pos])
	{
		subpath = ft_substr(envp[pos], 0, 5);
		if (!subpath)
			return (-1);
		if (ft_strncmp(subpath, "PATH=", ft_strlen(subpath)) == 0)
		{
			free(subpath);
			return (pos);
		}
		free(subpath);
		pos++;
	}
	return (-1);
}

// is_absolutepath: Verifica si un comando es una ruta absoluta y valida permisos.
// Argumentos:
// - cmd: Ruta del comando.
// - err: Puntero para almacenar errores.
// Retorno:
// - Devuelve 1 si el comando es una ruta válida y ejecutable.
// - Devuelve 0 si no es válido o si ocurre un error (modifica `err` en consecuencia).
static int	is_absolutepath(char *cmd, t_errors *err)
{
	struct stat	tipe;

	if (access(cmd, F_OK) == 0)
	{
		stat(cmd, &tipe);
		if ((tipe.st_mode & 0170000) == (0040000))
		{
			*err = IS_A_DIR;
			return (0);
		}
		if (access(cmd, X_OK))
		{
			*err = NO_PERMISSION;
			return (0);
		}
		return (1);
	}
	else
	{
		*err = NO_EXIST;
		return (0);
	}
}

// com_path: Encuentra la ruta ejecutable de un comando.
// Argumentos:
// - cmd: Nombre del comando.
// - envp: Arreglo de cadenas que representan el entorno actual.
// - err: Puntero para almacenar errores.
// Retorno:
// - Devuelve la ruta completa del comando si se encuentra y tiene permisos de ejecución.
// - Devuelve NULL si no se encuentra o si ocurre un error (modifica `err` en consecuencia).
char	*com_path(char *cmd, char **envp, t_errors *err)
{
	char	**enpath;
	int		pos;

	*err = 0;
	if (cmd && ft_strchr(cmd, '/'))
	{
		if (is_absolutepath(cmd, err))
			return (cmd);
		return (NULL);
	}
	pos = get_index_path(envp);
	if (pos == -1)
	{
		*err = NO_EXIST;
		return (NULL);
	}
	enpath = ft_split(envp[pos], ':');
	if (enpath == NULL)
	{
		*err = ERROR_MALLOC;
		return (NULL);
	}
	return (match_pathcmd(cmd, enpath, err));
}
