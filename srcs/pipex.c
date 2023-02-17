/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 15:33:01 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/17 20:59:25 by rrouille         ###   ########.fr       */
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
		unlink(".heredoc_tmp");
	free(pipex->pipe);
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

// void	execute_first_command(t_pipex pipex, int argc, char *argv[], char *env[])
// {
// 	dup2(pipex.pipe_fds[1], 1);
// 	close(pipex.pipe_fds[0]);
// 	dup2(pipex.infile, 0);
// 	pipex.command_arg_list = ft_split(argv[2], ' ');
// 	pipex.command = get_command_path(pipex.command_path_list, pipex.command_arg_list[0]);
// 	if (!pipex.command)
// 	{
// 		free_pipex(&pipex);
// 		write(2, COMMAND1_ERROR, ft_strlen(COMMAND1_ERROR));
// 		exit(1);
// 	}
// 	execve(pipex.command, pipex.command_arg_list, env);
// }

// void	execute_second_command(t_pipex pipex, int argc, char *argv[], char *env[])
// {
// 	dup2(pipex.pipe_fds[0], 0);
// 	close(pipex.pipe_fds[1]);
// 	dup2(pipex.outfile, 1);
// 	pipex.command_arg_list = ft_split(argv[3], ' ');
// 	pipex.command = get_command_path(pipex.command_path_list, pipex.command_arg_list[0]);
// 	if (!pipex.command)
// 	{
// 		free_pipex(&pipex);
// 		write(2, COMMAND2_ERROR, ft_strlen(COMMAND2_ERROR));
// 		exit(1);
// 	}
// 	execve(pipex.command, pipex.command_arg_list, env);
// }

void	execute_commands(t_pipex pipex, int argc, char *argv[], char *env[])
{
	// dup2(pipex.pipe_fds[0], 0);
	// close(pipex.pipe_fds[1]);
	// dup2(pipex.outfile, 1);
	// pipex.command_arg_list = ft_split(argv[3], ' ');
	// pipex.command = get_command_path(pipex.command_path_list, pipex.command_arg_list[0]);
	// if (!pipex.command)
	// {
	// 	free_pipex(&pipex);
	// 	write(2, COMMAND2_ERROR, ft_strlen(COMMAND2_ERROR));
	// 	exit(1);
	// }
	// execve(pipex.command, pipex.command_arg_list, env);
}

void	close_communication(t_pipex *pipex)
{
	close(pipex->pipe_fds[0]);
	close(pipex->pipe_fds[1]);
}

void	here_doc(char *limiter, t_pipex *pipex)
{
	int		file;
	char	*buf;

	file = open(".heredoc_tmp", O_CREAT | O_WRONLY | O_TRUNC, 0000644);
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
	pipex->infile = open(".heredoc_tmp", O_RDONLY);
	if (pipex->infile < 0)
	{
		unlink(".heredoc_tmp");
		ft_printf("%s", HERE_DOC_ERROR);
		exit (1);
	}
}

void	get_infile(char **argv, t_pipex *pipex)
{
	if (!strncmp("here_doc", argv[1], 8))
		here_doc(argv[2], pipex);
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

int	arguments(char *first_argument, t_pipex *pipex)
{
	if (!ft_strncmp("here_doc", first_argument, 9))
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

void	parent_free(t_pipex *pipex)
{
	int	i;

	i = 0;
	close(pipex->infile);
	close(pipex->outfile);
	if (pipex->here_doc)
		unlink(".heredoc_tmp");
	while (pipex->command_path_list[i])
	{
		free(pipex->command_path_list[i]);
		i++;
	}
	free(pipex->command_path_list);
	free(pipex->pipe);
}

static void	create_pipes(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < pipex->nbr_of_commands - 1)
	{
		if (pipe(pipex->pipe + 2 * i) < 0)
			parent_free(pipex);
		i++;
	}
}

static void	sub_dup2(int zero, int first)
{
	dup2(zero, 0);
	dup2(first, 1);
}


void	child(t_pipex pipex, char **argv, char **env)
{
	pipex.pid = fork();
	if (!pipex.pid)
	{
		if (pipex.i == 0)
			sub_dup2(pipex.infile, pipex.pipe[1]);
		else if (pipex.i == pipex.nbr_of_commands - 1)
			sub_dup2(pipex.pipe[2 * pipex.i - 2], pipex.outfile);
		else
			sub_dup2(pipex.pipe[2 * pipex.i - 2], pipex.pipe[2 * pipex.i + 1]);
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
	if (argc < arguments(argv[1], &pipex))
	{
		ft_printf("%s", INPUT_ERROR);
		exit (1);
	}
	pipex.nbr_of_commands = argc - 3 - pipex.here_doc;
	pipex.nbr_of_pipes = 2 * (pipex.nbr_of_commands -1);
	pipex.pipe = (int *)malloc(sizeof(int) * pipex.nbr_of_pipes);
	if (!pipex.pipe)
		return (0);
	//ft_printf("%d\n", pipex.pipe);
	get_infile(argv, &pipex);
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
	while (++pipex.i < pipex.nbr_of_commands)
		child(pipex, argv, env);
	// pipex.command1_pid = fork();
	// if (pipex.command1_pid == 0)
	// 	execute_first_command(pipex, argc, argv, env);
	// pipex.command2_pid = fork();
	// if (pipex.command2_pid == 0)
	// 	execute_second_command(pipex, argc, argv, env);
	close_communication(&pipex);
	// waitpid(pipex.command1_pid, NULL, 0);
	// waitpid(pipex.command2_pid, NULL, 0);
	waitpid(-1, NULL, 0);
	parent_free(&pipex);
	// close(pipex.infile);
	// close(pipex.outfile);
	// if (pipex.here_doc)
	// 	unlink(".heredoc_tmp");
	// while (pipex.command_path_list[i])
	// {
	// 	free(pipex.command_path_list[i]);
	// 	i++;
	// }
	// free(pipex.command_path_list);
	// free(pipex.pipe);
}
