/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antofern <antofern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 15:45:35 by antofern          #+#    #+#             */
/*   Updated: 2025/01/16 13:23:53 by antofern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
int	add_pathname(char **cmd,  t_garbage *garbage, char *envp[])
{
	//usa access para verificar cmd como ruta absoluta
	if ((*cmd)[0] == '/' || strncmp((*cmd), "../", 3) == 0 || strncmp((*cmd), "./", 2) == 0)
		return (0);

	//busca en PATH una ruta valida para cmd
		
		//	si la encuentra:
		//	-libera garbaje (aun no se como va a saber si cmd fue o no fue expandida )
		//	-costruye path/name

}
*/

//puede que innecesaria


static int	prepare_expansion(char **origin, char **new_str, t_garbage *garbage, char *envp[])
{
	int	len;

	len = calculate_expansion_length(*origin, envp);
	if (len < 0)
		return (1);
	*new_str = ft_calloc(len + 1, sizeof(char));
	if (*new_str == NULL)
	 	return (1);
	if (garbage->size <= garbage->current)
	{
		ft_putstr_fd("Bad count on expandstr", 2);//DEBUGEO
	 	return(1);
	}
	garbage->pointers[garbage->current] = *new_str;
	garbage->current++;
	*origin = *new_str;
	(*new_str)[len] = '\0';
	return (0);
}

int	expandstr(char **origin, t_garbage *garbage, char *envp[]) //envp debe recibir el array de strings que hemos creado y sobre el que se reflejan las modificaciones que pueda hacer minishell durante la ejecucion
{
	char	*marker;
	char	*new_str;
	char	*str;

	if(!is_expansible(*origin))
		return (0);
	str = *origin;
	if (prepare_expansion(origin, &new_str, garbage, envp))
		return (1);
	marker = str;
	while (*marker)
	{
		if (*marker == '$')
		{
			if(handle_dollar(&new_str, &str, &marker, envp))
				return (1);
			continue ;
		}
		if (*marker == 39 && ft_strchr(marker + 1, 39))
			marker = ft_strchr(marker + 1, 39);
		marker++;
	}
	if (*str)
        ft_strlcpy(new_str, str, marker - str + 1);
    return 0;
}

static int	expand_task(t_task *node, char *envp[])
{
	int	i;

	node->garb.size = count_expansions(((t_task *)node));
	node->garb.pointers = ft_calloc(node->garb.size + 1, sizeof(void *));
	if (node->garb.pointers == NULL)
		return (1);
	(node->garb.pointers)[node->garb.size] = NULL;
	
	if (expandstr(&(node->cmd), &(node->garb), envp))
		return (1);
//	if (add_pathname(&(node->cmd), &(node->garb), envp))
//		return (1);
	if (expandstr(&(node->redir.infoo), &(node->garb), envp))
		return (1);
	if (expandstr(&(node->redir.outfile), &(node->garb), envp))
		return (1);
	i = 0;
	while ((node->argv)[i])
	{
		if (expandstr((&(node->argv)[i]), &(node->garb), envp))
			return (1);
		i++;
	}
	return(0);
}

//Si retorna 1 imprimir perror(errno)
int	expand_tree(t_tree *node, char *envp[])
{
	if (node->type == PIPE)
	{
		if(expand_task(((t_pipe *)node)->left, envp))
			return (1);
		if(expand_tree((t_tree *)((t_pipe *)node)->rigth, envp))
			return (1);
	}
	else if (node->type == TASK)
	{
		if(expand_task(((t_task *)node), envp))
			return (1);
	}
	return (0);
}
