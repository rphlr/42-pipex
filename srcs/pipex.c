/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 15:33:01 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/18 10:57:10 by rrouille         ###   ########.fr       */
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

void	pipe_free(t_pipex *pipex)
{
	close(pipex->infile);
	close(pipex->outfile);
	if (pipex->here_doc)
		unlink(".heredoc_content");
	free(pipex->pipe_fds);
	ft_printf("%s\n", ENV_ERROR);
	exit(1);
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

void	get_heredoc_file(char *limiter, t_pipex *pipex)
{
	int		file;
	char	*buf;

	file = open(".heredoc_content", O_CREAT | O_WRONLY | O_TRUNC, 0000644);
	if (file < 1)
	{
		ft_printf("%s", HERE_DOC_ERROR);
		exit (1);
	}
	while (true)
	{
		write(1, "pipe heredoc> ", 14);
		if ((buf = get_next_line(0)) < 0)
			exit(1);
		if (!ft_strncmp(limiter, buf, ft_strlen(limiter + 1)))
			break ;
		write(file, buf, ft_strlen(buf));
		write(file, "\n", 1);
		free(buf);
	}
	free(buf);
	close(file);
	pipex->infile = open(".heredoc_content", O_RDONLY);
	if (pipex->infile < 0)
	{
		unlink(".heredoc_content");
		ft_printf("%s", HERE_DOC_ERROR);
		exit (1);
	}
}

void	open_input_file(char **argv, t_pipex *pipex)
{
	if (!strncmp("here_doc", argv[1], 8))
		get_heredoc_file(argv[2], pipex);
	else
	{
		pipex->infile = open(argv[1], O_RDONLY);
		if (pipex->infile < 0)
		{
			ft_printf("%s", INFILE_ERROR);
			exit (1);
		}
	}
}

void	open_output_file(char *argv, t_pipex *pipex)
{
	if (pipex->here_doc)
		pipex->outfile = open(argv, O_WRONLY | O_CREAT | O_APPEND, 0000644);
	else
		pipex->outfile = open(argv, O_CREAT | O_RDWR | O_TRUNC, 0000644);
	if (pipex->outfile < 0)
	{
		ft_printf("%s", OUTFILE_ERROR);
		exit (1);
	}
}

int	check_command_arguments(char *output_fd_argument, t_pipex *pipex)
{
	if (!ft_strncmp("here_doc", output_fd_argument, 9))
	{
		pipex->here_doc = 1;
		return (6);
	}
	else
	{
		pipex->here_doc = 0;
		return (5);
	}
}

void	free_ressources(t_pipex *pipex)
{
	int	i;

	i = 0;
	close(pipex->infile);
	close(pipex->outfile);
	if (pipex->here_doc)
		unlink(".heredoc_content");
	while (pipex->command_path_list[i])
	{
		free(pipex->command_path_list[i]);
		i++;
	}
	free(pipex->command_path_list);
	free(pipex->pipe_fds);
}

static void	create_pipes(t_pipex *pipex)
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
			setup_pipe_communication(pipex.infile, pipex.pipe_fds[1]);
		else if (pipex.i == pipex.command_count - 1)
			setup_pipe_communication(pipex.pipe_fds[2 * pipex.i - 2], pipex.outfile);
		else
			setup_pipe_communication(pipex.pipe_fds[2 * pipex.i - 2], pipex.pipe_fds[2 * pipex.i + 1]);
		close_communication(&pipex);
		pipex.command_arg_list = ft_split(argv[2 + pipex.here_doc + pipex.i], ' ');
		pipex.command = get_command_path(pipex.command_path_list, pipex.command_arg_list[0]);
		if (!pipex.command)
		{
			ft_printf("%s\n", pipex.command_arg_list[0]);
			free_pipex(&pipex);
			exit(1);
		}
		execve(pipex.command, pipex.command_arg_list, env);
	}
}

int	main(int argc, char **argv, char **env)
{
	t_pipex	pipex;

	pipex.i = -1;
	if (argc < check_command_arguments(argv[1], &pipex))
	{
		ft_printf("%s", INPUT_ERROR);
		exit (1);
	}
	pipex.command_count = argc - 3 - pipex.here_doc;
	pipex.pipes_count = 2 * (pipex.command_count -1);
	pipex.pipe_fds = (int *)malloc(sizeof(int) * pipex.pipes_count);
	if (!pipex.pipe_fds)
		return (0);
	open_input_file(argv, &pipex);
	//open_output_file(argv[argc - 1], &pipex);
	pipex.outfile = open(argv[argc - 1], O_TRUNC | O_CREAT | O_RDWR, 0000644);
	if (pipex.outfile < 0)
	{
		ft_printf("%s", OUTFILE_ERROR);
		exit (1);
	}
	// if (pipe(pipex.pipe_fds) < 0)
	// {
	// 	ft_printf("%s", PIPE_ERROR);
	// 	exit (1);
	// }
	pipex.path_var = find_path_env(env);
	pipex.command_path_list = ft_split(pipex.path_var, ':');
	if (!pipex.command_path_list)
		pipe_free(&pipex);
	create_pipes(&pipex);
	while (++pipex.i < pipex.command_count)
		run_commands(pipex, argv, env);
	// pipex.command1_pid = fork();
	// if (pipex.command1_pid == 0)
	// 	execute_output_fd_command(pipex, argc, argv, env);
	// pipex.command2_pid = fork();
	// if (pipex.command2_pid == 0)
	// 	execute_second_command(pipex, argc, argv, env);
	close_communication(&pipex);
	// waitpid(pipex.command1_pid, NULL, 0);
	// waitpid(pipex.command2_pid, NULL, 0);
	waitpid(-1, NULL, 0);
	free_ressources(&pipex);
	// close(pipex.infile);
	// close(pipex.outfile);
	// if (pipex.here_doc)
	// 	unlink(".heredoc_content");
	// while (pipex.command_path_list[i])
	// {
	// 	free(pipex.command_path_list[i]);
	// 	i++;
	// }
	// free(pipex.command_path_list);
	// free(pipex.pipe_fds);
}
