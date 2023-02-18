/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrouille <rrouille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/18 11:46:40 by rrouille          #+#    #+#             */
/*   Updated: 2023/02/18 13:16:36 by rrouille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @brief	Finds the PATH environment variable in the array of environment
 * 			variables and returns its value.
 * @param	env		A pointer to the array of environment variables.
 * @return	A pointer to the value of the PATH environment variable.
 */
char	*find_path_env(char **env)
{
	while (ft_strncmp("PATH", *env, 4))
		env++;
	return (*env + 5);
}
