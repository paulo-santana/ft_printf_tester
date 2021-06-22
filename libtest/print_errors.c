#include "libtest.h"

void print_int_diff(int expected, int result)
{
	ft_putstr("        Expected: ");
	ft_putnbr(expected);
	ft_putstr("\n        Got     : " RED);
	ft_putnbr(result);
	ft_putstr(RESET "\n");
}

void	print_string_diff(char expected[], char result[], unsigned int size)
{
	int is_red_already = 0;

	ft_putstr(RESET "        Expected: [");
	for (unsigned int i = 0; i < size; i++)
	{
		if (expected[i] == '\0')
			ft_putstr(MAGENTA "\\0" RESET);
		else if (expected[i] == '\n')
			ft_putstr(MAGENTA "\\n" RESET);
		else if (!isprint(expected[i]))
			ft_putchar('*');
		else
			ft_putchar(expected[i]);
	}

	ft_putstr("]\n        Got:      [");
	for (unsigned int i = 0; i < size; i++)
	{
		if (expected[i] != result[i] && !is_red_already)
		{
			is_red_already = 1;
			ft_putstr(BOLD RED);
		}
		else if (expected[i] == result[i] && is_red_already)
		{
			ft_putstr(RESET);
			is_red_already = 0;
		}
		if (result[i] == '\0')
			ft_putstr(BOLD MAGENTA "\\0" RESET);
		else if (!isprint(result[i]))
			ft_putchar('*');
		else
			ft_putchar(result[i]);
	}
	ft_putstr(RESET "]\n");
}
