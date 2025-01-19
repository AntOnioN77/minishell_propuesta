#include "minishell.h"
#include "executor.h"
#include <limits.h>

int	error_exe(int er, char *cmd)
{
	char	*msg;

	msg = NULL;
	//De momento no hay error 0, 1, 2, 3, 4, 5 y más
	if (er == 0)
		msg = ft_strjoin("Error: Need 4 arguments:\n", cmd);
	else if (er == 1)
		ft_putstr_fd("Error: Pipe not open\n", 2);
	else if (er == 2)
		ft_putstr_fd("Error: PID incorrect\n", 2);
	else if (er == 3)
		msg = ft_strjoin("Error: No such file or directory: ", cmd);
	else if (er == 4)
		msg = ft_strjoin("Error: Permission denied: ", cmd);
	else if (er == 5)
		msg = ft_strjoin("Error. Command not found: ", cmd);
	else if (er > 5)
		perror("Error");
	if (msg)
	{
		ft_putstr_fd(msg, 2);
		free (msg);
	}
	return (1);
}

/**
 * Busca el array que comienza con "PATH" (donde se encuentran las rutas de los
 * ejecutables) y devuelve su posición.
 */
int	search_path(char **envp)
{
	int pos;
	char *subpath;

	pos = 0;
	while (envp[pos])
	{
		subpath = ft_substr(envp[pos], 0, 4);
		if (ft_strcmp(subpath, "PATH") == 0)
		{
			free(subpath);
			return (pos);
		}
		free(subpath);
		pos++;
	}
	return (0);
}
/**
 * Se encarga de comprobar todas las posibles rutas, indicadas en envp, en las
 * que se puede encontrar el comando ejecutable al que se está haciendo
 * referencia.
 * El modo de comprobar si existe e utilizando la función access con los modos
 * F_OK y X_OK que evalúan si el archivo existe y si es ejecutable, 
 * respectivamente.
 */
char	*com_path(char *cmd, char **envp)
{
	char	*path;
	char	*slash;
	char	**enpath;
	int		pos;
	int 	i;

	i = 1;
	pos = search_path(envp);
	enpath = ft_split(envp[pos], ':');
	while (enpath[i])
	{
		slash = ft_strjoin(enpath[i], "/");
		path = ft_strjoin(slash, cmd);
		free(slash);
		if (access (path, F_OK | X_OK) == 0)
		{
			ft_free_double(enpath);
			return (path);
		}
		i++;
		free(path);
	}
	ft_free_double(enpath);
	return (NULL);
}

int create_child(t_task *task, char **envp)
{
	int pid;
	char *pathcmd;
	pathcmd = com_path(task->cmd, envp);
	if (pathcmd == NULL)
		return (5);//error en reserva de memoria?
	pid = fork();
	if (pid == -1)
		return (2);
	if (pid == 0)
	{
		execve(pathcmd, task->argv, envp);
		exit(EXIT_FAILURE);
	}
	task->pid = pid;
	return(0);
}

int execpipe(t_pipe *pipe_node, char **envp)
{
	int previous_stdin;
    int original_stdout;
	int pipefd[2];

	previous_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	pipe(pipefd);
	if (pipe_node->left)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		executor((t_tree *)pipe_node->left, envp);
		dup2(original_stdout, STDOUT_FILENO);
	}
	if (pipe_node->rigth) 
	{
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			executor(pipe_node->rigth, envp);
			dup2(previous_stdin, STDIN_FILENO);
	}
	close(previous_stdin);
	close(original_stdout);
}

int executor(t_tree *node, char **envp)
{
	t_pipe *pipe_node;
	t_task *task;
	int	err;

    if (!node)
        return (0);
    
    if (node->type == PIPE)
    {
        pipe_node = (t_pipe *)node;
		err = exec_pipe(node, envp);
		if (err != 0)
			return (err);
	}
    else if (node->type == TASK)
    {
        task = (t_task *)node;
		err = create_child(task, envp);
		if (err != 0)
			return (err);
    }
    return (0);
}

