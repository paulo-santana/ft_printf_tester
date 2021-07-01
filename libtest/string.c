#include "libtest.h"

size_t	tester_strlen(const char *str)
{
	size_t	len;

	len = 0;
	while (*str++)
		len++;
	return (len);
}

static int	str_matches(char *str, const char *target)
{
	int	matches;
	int	to_find_pos;

	matches = 0;
	to_find_pos = 0;
	while (target[to_find_pos] != '\0')
	{
		if (target[to_find_pos] != str[to_find_pos])
		{
			matches = 0;
			to_find_pos++;
			break ;
		}
		else
		{
			matches = 1;
			to_find_pos++;
		}
	}
	return (matches);
}

char	*tester_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	limit;
	int		matches;
	int		little_len;
	char	*match_start;

	i = 0;
	matches = 0;
	little_len = tester_strlen((char *)little);
	if (little_len == 0)
		return ((char *)big);
	if (len == 0)
		return (NULL);
	limit = len - little_len;
	while (i <= limit)
	{
		if (big[i] == 0)
			break ;
		match_start = (char *)&big[i];
		matches = str_matches(match_start, little);
		if (matches)
			return (match_start);
		i++;
	}
	return (0);
}

void tester_putchar(char c)
{
	write(1, &c, 1);
}

void tester_putstr(char *str)
{
	size_t len = strlen(str);
	write(1, str, len);
}

void tester_putnbr(int nbr)
{
	long	number;
	char	digit;

	number = nbr;
	if (number < 0)
	{
		write(1, "-", 1);
		number *= -1;
	}
	if (number > 9)
	{
		tester_putnbr(number / 10);
		tester_putnbr(number % 10);
	}
	else
	{
		digit = number + '0';
		write(1, &digit, 1);
	}
}
