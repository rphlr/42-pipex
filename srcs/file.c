/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/18 11:43:42 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/18 12:00:28 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	open_input_file(char **argv, t_pipex *pipex)
{
	if (!ft_strncmp("here_doc", argv[1], 8))
		get_heredoc_file(argv[2], pipex);
	else
	{
		pipex->input = open(argv[1], O_RDONLY);
		if (pipex->input < 0)
		{
			ft_printf("%s", INPUT_ERROR);
			exit (1);
		}
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
		buf = get_next_line(0);
		if (get_next_line(0) < 0)
			exit(1);
		if (!ft_strncmp(limiter, buf, ft_strlen(limiter + 1)))
			break ;
		write(file, buf, ft_strlen(buf));
		write(file, "\n", 1);
		free(buf);
	}
	free(buf);
	close(file);
	pipex->input = open(".heredoc_content", O_RDONLY);
	if (pipex->input < 0)
	{
		unlink(".heredoc_content");
		ft_printf("%s", HERE_DOC_ERROR);
		exit (1);
	}
}

void	open_output_file(char *argv, t_pipex *pipex)
{
	if (pipex->has_here_doc)
		pipex->output = open(argv, O_WRONLY | O_CREAT | O_APPEND, 0000644);
	else
		pipex->output = open(argv, O_CREAT | O_RDWR | O_TRUNC, 0000644);
	if (pipex->output < 0)
	{
		ft_printf("%s", OUTPUT_ERROR);
		exit (1);
	}
}

int	check_command_arguments(char *output_fd_argument, t_pipex *pipex)
{
	if (!ft_strncmp("here_doc", output_fd_argument, 9))
	{
		pipex->has_here_doc = 1;
		return (6);
	}
	else
	{
		pipex->has_here_doc = 0;
		return (5);
	}
}
