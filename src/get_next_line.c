/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psergio- <psergio-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/05 18:06:57 by psergio-          #+#    #+#             */
/*   Updated: 2021/06/17 02:55:33 by psergio-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	clear_buffer(char **buffer)
{
	if (*buffer)
	{
		free(*buffer);
	}
	*buffer = NULL;
}

static char	*merge_buffer(char *dest, t_buffer *buf)
{
	size_t	dest_len;
	size_t	result_len;
	char	*result;
	size_t	i;
	int		j;

	dest_len = 0;
	if (dest)
		dest_len = gnl_ft_strlen(dest);
	result_len = dest_len + buf->next_nl - buf->start;
	result = malloc(sizeof(char) * (result_len + 1));
	if (result == NULL)
		return (NULL);
	i = 0;
	while (i < dest_len)
	{
		result[i] = dest[i];
		i++;
	}
	j = buf->start;
	while (j < buf->next_nl)
		result[i++] = buf->data[j++];
	result[i] = '\0';
	return (result);
}

/**
 * Fetches `BUFFER_SIZE` characters from file `fd` and append them to the
 * string *new_line. If a \n is found during the read() call, it appends only
 * the text until this \n and returns `GNL_LINE_READ`. If no newline character
 * is found, it appends the whole buffer and returns `GNL_NO_NEWLINE`. If an
 * error occurs, `GNL_ERROR` will be returned. And if we reached `fd`s end,
 * `GNL_END_OF_FILE` will be returned.
 * */

static int	append_next_chunk(int fd, char **new_line, t_buffer *buf)
{
	char		*merged_str;

	if (buf->start >= buf->end)
	{
		buf->end = read(fd, buf->data, BUFFER_SIZE);
		buf->start = 0;
	}
	if (buf->end == -1)
		return (GNL_ERROR);
	buf->next_nl = buf->start;
	while (buf->next_nl < buf->end && buf->data[buf->next_nl] != '\n')
		buf->next_nl++;
	merged_str = merge_buffer(*new_line, buf);
	buf->start = buf->next_nl + 1;
	if (merged_str == NULL)
		return (GNL_ERROR);
	if (*new_line)
		free(*new_line);
	*new_line = merged_str;
	if (buf->next_nl < buf->end)
		return (GNL_LINE_READ);
	if (buf->end < BUFFER_SIZE)
		return (GNL_END_OF_FILE);
	return (GNL_NO_NEWLINE);
}

/**
 * Fetches the next string from file `fd` and put it in *line. The string
 * will be truncated on the first \n encountered, or by the end of file.
 * */

int	get_next_line(int fd, char **line)
{
	char			*new_line;
	int				result;
	static t_buffer	buffers[FD_SETSIZE];

	new_line = NULL;
	*line = new_line;
	if (fd < 0 || fd >= FD_SETSIZE)
		return (GNL_ERROR);
	if (buffers[fd].data == NULL)
	{
		buffers[fd].data = malloc((BUFFER_SIZE) * sizeof(char));
		if (buffers[fd].data == NULL)
			return (GNL_ERROR);
		buffers[fd].start = BUFFER_SIZE;
	}
	result = GNL_NO_NEWLINE;
	while (result == GNL_NO_NEWLINE)
	{
		result = append_next_chunk(fd, &new_line, &buffers[fd]);
		*line = new_line;
	}
	if (result == GNL_END_OF_FILE || result == GNL_ERROR)
		clear_buffer(&(buffers[fd].data));
	return (result);
}
