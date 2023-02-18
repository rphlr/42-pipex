/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 15:29:52 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/18 14:18:01 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>
# include "../mylib/includes/mylib.h"

# define RED "\e[31m"
# define RESET "\e[K"

# define ARGUMENT_ERROR RED"Invalid number of arguments.\n"RESET
# define INPUT_ERROR RED"Invalid input file.\n"RESET
# define OUTPUT_ERROR RED"Invalid output file.\n"RESET
# define HERE_DOC_ERROR RED"Temporary file creation or editing error for \
here-document.\n"RESET
# define MISSING_ENV RED"Missing environment variable.\n"RESET

typedef struct s_pipex
{
	int		input;
	int		output;
	int		*pipe_fds;
	int		pipes_count;
	int		command_count;
	char	*command;
	char	**command_arg_list;
	char	**command_path_list;
	char	*path_var;
	int		has_here_doc;
	pid_t	pid;
	int		i;
}	t_pipex;

void	free_ressources(t_pipex *pipex);
void	free_pipex(t_pipex *pipex);
void	get_heredoc_file(char *limiter, t_pipex *pipex);
int		check_command_arguments(char *output_fd_argument, t_pipex *pipex);
void	open_input_file(char **argv, t_pipex *pipex);
void	open_output_file(char *argv, t_pipex *pipex);
char	*find_path_env(char **env);
void	pipe_free(t_pipex *pipex);
void	create_pipes(t_pipex *pipex);
void	run_commands(t_pipex pipex, char **argv, char **env);
void	close_communication(t_pipex *pipex);

#endif