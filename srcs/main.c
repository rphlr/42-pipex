/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 15:33:01 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/18 11:59:13 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	main(int argc, char **argv, char **env)
{
	t_pipex	pipex;

	pipex.i = -1;
	if (argc < check_command_arguments(argv[1], &pipex))
	{
		ft_printf("%s", ARGUMENT_ERROR);
		exit (1);
	}
	pipex.command_count = argc - 3 - pipex.has_here_doc;
	pipex.pipes_count = 2 * (pipex.command_count -1);
	pipex.pipe_fds = (int *)malloc(sizeof(int) * pipex.pipes_count);
	if (!pipex.pipe_fds)
		return (0);
	open_input_file(argv, &pipex);
	open_output_file(argv[argc - 1], &pipex);
	if (pipex.output < 0)
	{
		ft_printf("%s", OUTPUT_ERROR);
		exit (1);
	}
	pipex.path_var = find_path_env(env);
	pipex.command_path_list = ft_split(pipex.path_var, ':');
	if (!pipex.command_path_list)
		pipe_free(&pipex);
	create_pipes(&pipex);
	while (++pipex.i < pipex.command_count)
		run_commands(pipex, argv, env);
	close_communication(&pipex);
	waitpid(-1, NULL, 0);
	free_ressources(&pipex);
}
