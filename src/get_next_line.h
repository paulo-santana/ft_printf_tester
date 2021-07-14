/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psergio- <psergio-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 11:46:38 by psergio-          #+#    #+#             */
/*   Updated: 2021/06/08 07:57:49 by psergio-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# include <stdlib.h>

typedef struct s_buffer {
	int		start;
	int		next_nl;
	int		end;
	char	*data;
}			t_buffer;

size_t	ft_strlcat(char *dest, const char *src, size_t size);
size_t	gnl_ft_strlen(const char *str);
void	*ft_calloc(size_t count, size_t size);

/**
 * Retrieves a string that ends with a newline charactere from the file
 * descriptor `fd` and make the pointer `line` point to it
 * */

int		get_next_line(int fd, char **line);

# define GNL_ERROR -1
# define GNL_END_OF_FILE 0
# define GNL_LINE_READ 1
# define GNL_NO_NEWLINE 2

#endif
