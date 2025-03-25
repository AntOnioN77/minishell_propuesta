/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redirs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 21:17:14 by jperez-r          #+#    #+#             */
/*   Updated: 2025/03/25 15:00:15 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>
#include "executor.h"

// file_redirector: Configura la redirección de un archivo a un descriptor de archivo.
// Argumentos:
// - newfd: Descriptor de archivo al que se redirigirá el archivo.
// - file: Nombre del archivo a redirigir.
// - opflag: Flags de apertura del archivo (O_RDONLY, O_WRONLY, etc.).
// - wordfail: Puntero para almacenar el nombre del archivo en caso de error.
// Retorno:
// - Devuelve ALL_OK si la operación es exitosa.
// - Devuelve un código de error si ocurre un fallo (e.g., NO_EXIST, NO_PERMISSION, IS_A_DIR, errno).
t_errors	file_redirector(int newfd, char *file, int opflag, char **wordfail)
{
	struct stat	tipe;
	int			fd;

	*wordfail = file;
	if ((opflag & 0b11) == O_RDONLY) // Verifica si es una redirección de lectura.
	{
		if (access(file, F_OK) == -1)
			return (NO_EXIST); // El archivo no existe.
		if (access(file, W_OK == -1))
			return (NO_PERMISSION); // No hay permisos de escritura.
	}
	else if (stat(file, &tipe) == 0) // Verifica si el archivo es un directorio.
	{
		if ((tipe.st_mode & 0170000) == (0040000))
			return (IS_A_DIR); // Es un directorio.
		if (access(file, F_OK) == 0 && access(file, R_OK == -1))
			return (NO_PERMISSION); // No hay permisos de lectura.
	}
	if ((opflag & O_CREAT) == O_CREAT) // Si se debe crear el archivo.
		fd = open(file, opflag, 0664);
	else
		fd = open(file, opflag);
	if (fd == -1 || dup2(fd, newfd) == -1 || close(fd))
		return (errno); // Error al abrir, duplicar o cerrar el archivo.
	return (ALL_OK);
}

// apply_redirs: Aplica las redirecciones de entrada y salida especificadas en un nodo de redirección.
// Argumentos:
// - redir: Nodo que contiene la información de las redirecciones.
// - word_fail: Puntero para almacenar el nombre del archivo que falló en caso de error.
// Retorno:
// - Devuelve 0 (ALL_OK) si todas las redirecciones se aplican correctamente.
// - Devuelve un código de error si ocurre un fallo.
t_errors	apply_redirs(t_redir *redir, char **word_fail)
{
	t_errors	error;

	error = 0;
	if (redir->insymbol == infile) // Redirección de entrada desde un archivo.
		error = file_redirector(0, redir->infoo, O_RDONLY, word_fail);
	else if (redir->insymbol == heredoc) // Redirección de entrada desde un heredoc.
		error = file_redirector(STDIN_FILENO, redir->tmp_file,
				O_RDONLY, word_fail);
	if (error != 0)
		return (error); // Retorna el error si ocurre en la redirección de entrada.
	if (redir->outsymbol == outfile) // Redirección de salida a un archivo.
		error = file_redirector(1, redir->outfile,
				O_WRONLY | O_CREAT | O_TRUNC, word_fail);
	else if (redir->outsymbol == append) // Redirección de salida en modo append.
		error = file_redirector(1, redir->outfile,
				O_WRONLY | O_APPEND | O_CREAT, word_fail);
	return (error); // Retorna el error si ocurre en la redirección de salida.
}
