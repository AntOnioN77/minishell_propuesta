/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 00:22:02 by antofern          #+#    #+#             */
/*   Updated: 2025/03/25 14:42:32 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../executor.h"
#include <errno.h>

// Genera un nombre de archivo temporal único basado en un índice.
// Retorna un puntero a una cadena con el nombre del archivo o NULL en caso de error.
// Modifica el valor de `*error` si ocurre un error, asignándole el código de error correspondiente.
char	*compose_filename(int i, t_errors *error)
{
	char	*num;
	char	*str;

	num = NULL;
	num = ft_itoa(i);
	if (num == NULL)
	{
		*error = errno;
		return (NULL);
	}
	str = ft_strjoin("/tmp/.minishell.", num);
	ft_free_null((void **)&num);
	if (str == NULL)
	{
		*error = errno;
		return (NULL);
	}
	return (str);
}

// Obtiene un nombre de archivo temporal que no exista en el sistema.
// Retorna un puntero a una cadena con el nombre del archivo o NULL en caso de error.
// Modifica el valor de `*error` si ocurre un error, asignándole el código de error correspondiente.
char	*get_tmp_name(t_errors *error)
{
	char	*str;
	int		i;

	i = 0;
	str = NULL;
	while (i < 500)
	{
		str = compose_filename(i, error);
		if (access(str, F_OK) != 0 && errno == ENOENT)
			return (str);
		i++;
		ft_free_null((void **)&str);
	}
	ft_putstr_fd("mini$hell: Could not name temporary file required by \
		heredoc", 2);
	*error = TMP_FILE_ERROR;
	return (NULL);
}

// Escribe una línea en el heredoc, verificando si coincide con el separador.
// Retorna CONTINUE si debe continuar escribiendo, o ALL_OK si se encuentra el separador.
// No modifica los argumentos externos.
static t_errors	write_heredoc_line(int fd, char *separator, size_t seplen)
{
	char	*line;

	signal(SIGINT, SIG_DFL);
	signal(SIGINT, handle_sigint_heredoc);
	line = readline("> ");
	if (!line)
	{
		ft_putstr_fd("minishell: warning: here-document delimited by EOF\n", 1);
		exit (ALL_OK);
	}
	if (ft_strlen(line) == seplen && !ft_strncmp(line, separator, seplen))
	{
		free(line);
		exit (ALL_OK);
	}
	ft_putstr_fd(line, fd);
	ft_putchar_fd('\n', fd);
	free(line);
	exit (CONTINUE);
}

// Maneja el proceso de escritura del heredoc en un proceso hijo.
// Retorna el estado del proceso hijo o un código de error en caso de fallo.
// No modifica los argumentos externos.
static t_errors	write_heredoc_fork(int fd, char *separator, size_t seplen)
{
	pid_t	pid;
	int		status;

	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid < 0)
		return (errno);
	if (pid == 0)
		status = write_heredoc_line(fd, separator, seplen);
	else
	{
		if (waitpid(pid, &status, 0) == -1)
			return (errno);
		signal(SIGINT, handle_sigint);
		status = ((status) & 0xff00) >> 8;
	}
	return (status);
}

// Escribe el contenido del heredoc en un archivo temporal.
// Retorna ALL_OK si se completa correctamente o un código de error en caso contrario.
// No modifica los argumentos externos.
t_errors	heredoc_writer(char *separator, t_redir *redir)
{
	int			fd;
	size_t		seplen;
	t_errors	status;

	fd = open(redir->tmp_file, O_WRONLY | O_TRUNC);
	if (fd < 0)
		return (errno);
	seplen = ft_strlen(separator);
	status = CONTINUE;
	while (status == CONTINUE)
		status = write_heredoc_fork(fd, separator, seplen);
	if (close(fd) < 0)
		return (errno);
	if (status == E_SIGINT && unlink(redir->tmp_file) < 0)
		return (errno);
	return (status);
}

// Crea un heredoc y maneja los errores asociados.
// Retorna ALL_OK si se completa correctamente o un código de error en caso contrario.
// Modifica `redir->tmp_file` con el nombre del archivo temporal creado.
t_errors	create_heredoc(t_redir *redir)
{
	t_errors	error;
	char		*tmp_file;
	int			fd;

	if (redir->insymbol != heredoc)
		return (0);
	if (redir->infoo == NULL || (redir->infoo[0]) == '\0')
		return (SYNTAX_ERROR);
	error = 0;
	tmp_file = get_tmp_name(&error);
	if (error || !tmp_file)
		return (error);
	fd = open(tmp_file, O_CREAT | O_WRONLY | O_APPEND, 00600);
	if (fd == -1)
		return (errno);
	else
	{
		redir->tmp_file = tmp_file;
		close(fd);
	}
	error = heredoc_writer(redir->infoo, redir);
	return (error);
}
