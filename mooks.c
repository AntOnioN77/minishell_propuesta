/*                        ███╗   ███╗ ██████╗  ██████╗ ██╗  ██╗███████╗                                  
                        ████╗ ████║██╔═══██╗██╔═══██╗██║ ██╔╝██╔════╝                                  
                        ██╔████╔██║██║   ██║██║   ██║█████╔╝ ███████╗                                  
                        ██║╚██╔╝██║██║   ██║██║   ██║██╔═██╗ ╚════██║                                  
                        ██║ ╚═╝ ██║╚██████╔╝╚██████╔╝██║  ██╗███████║                                  
                        ╚═╝     ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝                                  
                                                                                                       
███████╗██╗   ██╗███╗   ██╗ ██████╗██╗ ██████╗ ███╗   ██╗███████╗███████╗     ██████╗ ██╗   ██╗███████╗
██╔════╝██║   ██║████╗  ██║██╔════╝██║██╔═══██╗████╗  ██║██╔════╝██╔════╝    ██╔═══██╗██║   ██║██╔════╝
█████╗  ██║   ██║██╔██╗ ██║██║     ██║██║   ██║██╔██╗ ██║█████╗  ███████╗    ██║   ██║██║   ██║█████╗  
██╔══╝  ██║   ██║██║╚██╗██║██║     ██║██║   ██║██║╚██╗██║██╔══╝  ╚════██║    ██║▄▄ ██║██║   ██║██╔══╝  
██║     ╚██████╔╝██║ ╚████║╚██████╗██║╚██████╔╝██║ ╚████║███████╗███████║    ╚██████╔╝╚██████╔╝███████╗
╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚══════╝     ╚══▀▀═╝  ╚═════╝ ╚══════╝
                                                                                                       
        ██████╗ ███████╗██████╗ ███████╗███╗   ██╗    ███████╗███████╗██████╗                          
        ██╔══██╗██╔════╝██╔══██╗██╔════╝████╗  ██║    ██╔════╝██╔════╝██╔══██╗                         
        ██║  ██║█████╗  ██████╔╝█████╗  ██╔██╗ ██║    ███████╗█████╗  ██████╔╝                         
        ██║  ██║██╔══╝  ██╔══██╗██╔══╝  ██║╚██╗██║    ╚════██║██╔══╝  ██╔══██╗                         
        ██████╔╝███████╗██████╔╝███████╗██║ ╚████║    ███████║███████╗██║  ██║                         
        ╚═════╝ ╚══════╝╚═════╝ ╚══════╝╚═╝  ╚═══╝    ╚══════╝╚══════╝╚═╝  ╚═╝                         
                                                                                                       
███████╗██╗   ██╗███████╗████████╗██╗████████╗██╗   ██╗██╗██████╗  █████╗ ███████╗                     
██╔════╝██║   ██║██╔════╝╚══██╔══╝██║╚══██╔══╝██║   ██║██║██╔══██╗██╔══██╗██╔════╝                     
███████╗██║   ██║███████╗   ██║   ██║   ██║   ██║   ██║██║██║  ██║███████║███████╗                     
╚════██║██║   ██║╚════██║   ██║   ██║   ██║   ██║   ██║██║██║  ██║██╔══██║╚════██║                     
███████║╚██████╔╝███████║   ██║   ██║   ██║   ╚██████╔╝██║██████╔╝██║  ██║███████║                     
╚══════╝ ╚═════╝ ╚══════╝   ╚═╝   ╚═╝   ╚═╝    ╚═════╝ ╚═╝╚═════╝ ╚═╝  ╚═╝╚══════╝                    */



#include "minishell.h"
#include <sys/wait.h>

//LO DE JOSE

#include "minishell.h"
#include "executor.h"
#include <limits.h>

int     ft_free_double(char **s)
{
        int     i;

        i = 0;
        if (!s)
                return (0);
        while (s[i])
        {
                free (s[i]);
                i++;
        }
        free(s);
        return (1);
}


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

//Empezando en fd_since, cierra tosos los fd abiertos,
//hasta encontrar uno cerrado o inexistente. Antes de cerrar consulta
//si existe y esta abierto con fstat.
void close_fds(int fd_since)
{
	struct stat statbuf;

	while(fstat(fd_since, &statbuf)!= -1)
	{
		printf("cerrando fd %d\n", fd_since);
		close(fd_since);
		fd_since++;
	}
}

int create_child(t_task *task, char **envp)
{
	int pid;
	char *pathcmd;
	int		err;

	pid = fork();
	if (pid == -1)
		return (2);
	if (pid == 0)
	{
		close_fds(3);
		pathcmd = com_path(task->cmd, envp);
		if (pathcmd == NULL)
			return (5);//error en reserva de memoria?
		execve(pathcmd, task->argv, envp);
		err = errno;
		close_fds(0);
		free(pathcmd);
		exit(err);
	}
	task->pid = pid;
	return(0);
}

int exec_pipe(t_pipe *pipe_node, char **envp)
{
	int previous_stdin;
    int original_stdout;
	int pipefd[2];
	int	err;

	previous_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	pipe(pipefd);
	if (pipe_node->left)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		err = executor((t_tree *)pipe_node->left, envp);
		if(err != 0)
			return (err);
		dup2(original_stdout, STDOUT_FILENO);
	}
	if (pipe_node->rigth) 
	{
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			err = executor(pipe_node->rigth, envp);
			if(err != 0)
				return (err);
			dup2(previous_stdin, STDIN_FILENO);
	}
	close(previous_stdin);
	close(original_stdout);
	return (0);
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
		err = exec_pipe(pipe_node, envp);
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

//fin lo de

int	search_var(char **envp, const char* var)
{
	int pos;

	pos = 0;
	while (envp[pos])
	{
		if (ft_strncmp(envp[pos], var, ft_strlen(var)) == 0)
		{
			return (pos);
		}
		pos++;
	}
	return (-1);
}

char *ft_getenv(const char *name, char *envp[])
{
	int pos;
	char *word_start;

	pos = search_var(envp, name);
	if (pos == -1)
		return(NULL);
	word_start = ft_strchr(envp[pos], '=') + 1;
	return(word_start);
}

//SOLO PARA PRUEBAS no requiere implementacion
//-Imprime el arbol
void print_tree(t_tree *node, int depth)
{
    if (!node)
        return;
    for (int i = 0; i < depth; i++)
        printf("  ");
    if(node->type == PIPE)
	{
    	printf("PIPE\n");
        print_tree(((t_tree *)((t_pipe *)node)->left), depth + 1);
        print_tree(((t_pipe *)node)->rigth, depth + 1);
	}
    else if(node->type == TASK)
    {
            printf("TASK->cmd: %s\n", ((t_task *)node)->cmd);
			for (int j = 0; ((t_task *)node)->argv[j]; j++)
				printf("\t->argv[%d]: %s\n", j, ((t_task *)node)->argv[j]);
			printf("\t->redir->insymbol: %d\n", ((t_task *)node)->redir.insymbol);
			printf("\t->redir->infoo: %s\n", ((t_task *)node)->redir.infoo);
			printf("\t->redir->outsymbol: %d\n", ((t_task *)node)->redir.outsymbol);
			printf("\t->redir->outfile: %s\n", ((t_task *)node)->redir.outfile);
    }
	else
		return; //llegar aqui indicaria un error
}

void wait_all(t_tree *node)
{
    int status;
    
    if (!node)
        return;
        
    if (node->type == PIPE)
    {
        t_pipe *pipe_node = (t_pipe *)node;
        wait_all((t_tree *)pipe_node->left);
        wait_all(pipe_node->rigth);
    }
    else if (node->type == TASK)
    {
        t_task *task = (t_task *)node;
        waitpid(task->pid, &status, 0);
    }
}
