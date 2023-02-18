/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/18 11:46:52 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/18 11:58:30 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	*get_command_path(char **path_var, char *command)
{
	char	*temp_path;
	char	*command_path;

	while (*path_var)
	{
		temp_path = ft_strjoin(*path_var, "/");
		command_path = ft_strjoin(temp_path, command);
		free(temp_path);
		if (access(command_path, 0) == 0)
			return (command_path);
		free(command_path);
		path_var++;
	}
	return (NULL);
}

void	close_communication(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < (pipex->pipes_count))
	{
		close(pipex->pipe_fds[i]);
		i++;
	}
}

void	create_pipes(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < pipex->command_count - 1)
	{
		if (pipe(pipex->pipe_fds + 2 * i) < 0)
			free_ressources(pipex);
		i++;
	}
}

static void	setup_pipe_communication(int input_fd, int output_fd)
{
	dup2(input_fd, 0);
	dup2(output_fd, 1);
}

void	run_commands(t_pipex pipex, char **argv, char **env)
{
	pipex.pid = fork();
	if (!pipex.pid)
	{
		if (pipex.i == 0)
			setup_pipe_communication(pipex.input, pipex.pipe_fds[1]);
		else if (pipex.i == pipex.command_count - 1)
			setup_pipe_communication(pipex.pipe_fds[2 * pipex.i - 2],
				pipex.output);
		else
			setup_pipe_communication(pipex.pipe_fds[2 * pipex.i - 2],
				pipex.pipe_fds[2 * pipex.i + 1]);
		close_communication(&pipex);
		pipex.command_arg_list = ft_split(argv[2 + pipex.has_here_doc
				+ pipex.i], ' ');
		pipex.command = get_command_path(pipex.command_path_list,
				pipex.command_arg_list[0]);
		if (!pipex.command)
		{
			ft_printf("%s\n", pipex.command_arg_list[0]);
			free_pipex(&pipex);
			exit(1);
		}
		execve(pipex.command, pipex.command_arg_list, env);
	}
}
