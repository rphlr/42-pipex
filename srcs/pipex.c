/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 15:33:01 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/16 20:27:33 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*find_path_env(char **env)
{
	while (ft_strncmp("PATH", *env, 4))
		env++;
	return (*env + 5);
}

void	free_pipex(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (pipex->command_arg_list[i])
	{
		free(pipex->command_arg_list[i]);
		i++;
	}
	free(pipex->command_arg_list);
	free(pipex->command);
}

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

void	execute_first_command(t_pipex pipex, char *argv[], char *env[])
{
	dup2(pipex.pipe_fds[1], 1);
	close(pipex.pipe_fds[0]);
	dup2(pipex.infile, 0);
	pipex.command_arg_list = ft_split(argv[2], ' ');
	pipex.command = get_command_path(pipex.command_path_list, pipex.command_arg_list[0]);
	if (!pipex.command)
	{
		free_pipex(&pipex);
		write(2, COMMAND1_ERROR, ft_strlen(COMMAND1_ERROR));
		exit(1);
	}
	execve(pipex.command, pipex.command_arg_list, env);
}

void	execute_second_command(t_pipex pipex, char *argv[], char *env[])
{
	dup2(pipex.pipe_fds[0], 0);
	close(pipex.pipe_fds[1]);
	dup2(pipex.outfile, 1);
	pipex.command_arg_list = ft_split(argv[3], ' ');
	pipex.command = get_command_path(pipex.command_path_list, pipex.command_arg_list[0]);
	if (!pipex.command)
	{
		free_pipex(&pipex);
		write(2, COMMAND2_ERROR, ft_strlen(COMMAND2_ERROR));
		exit(1);
	}
	execve(pipex.command, pipex.command_arg_list, env);
}

void	close_communication(t_pipex *pipex)
{
	close(pipex->pipe_fds[0]);
	close(pipex->pipe_fds[1]);
}

int	main(int argc, char **argv, char **env)
{
	t_pipex	pipex;
	int		i;

	i = 0;
	if (argc != 5)
	{
		ft_printf("%s", INPUT_ERROR);
		exit (1);
	}
	pipex.infile = open(argv[1], O_RDONLY);
	if (pipex.infile < 0)
	{
		ft_printf("%s", INFILE_ERROR);
		exit (1);
	}
	pipex.outfile = open(argv[argc - 1], O_TRUNC | O_CREAT | O_RDWR, 0000644);
	if (pipex.outfile < 0)
	{
		ft_printf("%s", OUTFILE_ERROR);
		exit (1);
	}
	if (pipe(pipex.pipe_fds) < 0)
	{
		ft_printf("%s", PIPE_ERROR);
		exit (1);
	}
	pipex.path_var = find_path_env(env);
	pipex.command_path_list = ft_split(pipex.path_var, ':');
	pipex.command1_pid = fork();
	if (pipex.command1_pid == 0)
		execute_first_command(pipex, argv, env);
	pipex.command2_pid = fork();
	if (pipex.command2_pid == 0)
		execute_second_command(pipex, argv, env);
	close_communication(&pipex);
	waitpid(pipex.command1_pid, NULL, 0);
	waitpid(pipex.command2_pid, NULL, 0);
	close(pipex.infile);
	close(pipex.outfile);
	while (pipex.command_path_list[i])
	{
		free(pipex.command_path_list[i]);
		i++;
	}
	free(pipex.command_path_list);
}
