/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 15:29:52 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/16 20:23:12 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <string.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>
# include "../mylib/includes/mylib.h"

# define INPUT_ERROR "Invalid number of arguments.\n"
# define INFILE_ERROR "Invalid infile.\n"
# define OUTFILE_ERROR "Error with outfile.\n"
# define PIPE_ERROR "Error when creating a communication channel between the two processes.\n"
# define COMMAND1_ERROR "Command 1 not found.\n"
# define COMMAND2_ERROR "Command 2 not found.\n"

typedef struct s_pipex
{
	pid_t	command1_pid;
	pid_t	command2_pid;
	int		pipe_fds[2];
	char	*path_var;
	char	**command_path_list;
	char	**command_arg_list;
	char	*command;
	int		infile;
	int		outfile;
} t_pipex;

#endif