/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/18 11:46:52 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/18 15:10:40 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @brief	Gets the full path of the command executable.
 * @param	path_var	A pointer to the PATH environment variable split into
 * 						an array of paths.
 * @param	command		The name of the command executable.
 * @return	A pointer to the full path of the command executable, or NULL if
 * 			not found.
 */
char	*get_command_path(char **path_var, char *command)
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

/**
 * @brief	Closes the unused ends of the communication pipes.
 * @param	pipex	A pointer to the t_pipex structure containing file and pipe
 * 					information.
 * @return	void
 */
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

/**
 * @brief	Creates the required pipes for communication between commands.
 * @param	pipex	A pointer to the t_pipex structure containing file and
 * 					pipe information.
 * @return	void
 */
void	create_pipes(t_pipex *pipex)
{
	int	i;

	i = 1;
	while (i < pipex->command_count)
	{
		if (pipe(pipex->pipe_fds + 2 * i) < 0)
			free_ressources(pipex);
		i++;
	}
}

/**
 * @brief	Sets up pipe communication between the specified input and output
 * 			file descriptors.
 * @param	input_fd	The input file descriptor to be used for the pipe
 * 						communication.
 * @param	output_fd	The output file descriptor to be used for the pipe
 * 						communication.
 * @return	void
 */
void	setup_pipe_communication(int input_fd, int output_fd)
{
	dup2(input_fd, 0);
	dup2(output_fd, 1);
}

/**
 * @brief	Runs the specified command with the appropriate input and output.
 * @param	pipex	A t_pipex structure containing file and pipe information.
 * @param	argv	A pointer to the array of command-line arguments.
 * @param	env		A pointer to the array of environment variables.
 * @return	void
 */
void	run_commands(t_pipex pipex, char **argv, char **env)
{
	char *error;

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
		pipex.command_arg_list = ft_split(argv[2 + pipex.has_here_doc +
			pipex.i], ' ');
		pipex.command = get_command_path(pipex.command_path_list,
			pipex.command_arg_list[0]);
		if (!pipex.command)
		{
			error = ft_strjoin(pipex.command_arg_list[0],
				" command does not exists.\n\e[K");
			error = ft_strjoin("\e[31m", error);
			write(2, error, ft_strlen(error));
			free_pipex(&pipex);
			exit(1);
		}
		execve(pipex.command, pipex.command_arg_list, env);
	}
}
