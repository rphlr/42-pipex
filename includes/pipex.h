/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 15:29:52 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/17 20:34:04 by rrouille         ###   ########.fr       */
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
// # define COMMAND_ERROR "Command not found.\n"
# define HERE_DOC_ERROR "here_doc temporary file creation or editing error.\n"
# define ENV_ERROR "Environnement not found.\n"

typedef struct s_pipex
{
	int		infile;
	int		outfile;
	int		pipe_fds[2];
	char	*path_var;
	int		nbr_of_commands;
	int		here_doc;
	int		nbr_of_pipes;
	int		*pipe;
	char	**command_path_list;
	char	**command_arg_list;
	pid_t	pid;
	char	*command;
	int		i;
} t_pipex;

#endif