#include "libtest.h"

void print_int_diff(int expected, int result)
{
	tester_putstr("        Expected: ");
	tester_putnbr(expected);
	tester_putstr("\n        Got     : " RED);
	tester_putnbr(result);
	tester_putstr(RESET "\n");
}

void	print_string_diff(char expected[], char result[], unsigned int expected_size, unsigned int result_size)
{
	int is_red_already = 0;

	tester_putstr(RESET "        Expected: [");
	for (unsigned int i = 0; i < expected_size; i++)
	{
		if (expected[i] == '\0')
			tester_putstr(MAGENTA "\\0" RESET);
		else if (expected[i] == '\n')
			tester_putstr(MAGENTA "\\n" RESET);
		else if (!isprint(expected[i]))
			tester_putstr(MAGENTA "*");
		else
			tester_putchar(expected[i]);
	}

	tester_putstr("]\n        Got:      [");
	unsigned int i = 0;
	int expected_ended = 0;
	while (i < result_size)
	{
		if (expected_ended || ((expected[i] != result[i]) && !is_red_already))
		{
			is_red_already = 1;
			tester_putstr(BOLD RED);
		}
		else if (expected[i] == result[i] && is_red_already)
		{
			tester_putstr(RESET);
			is_red_already = 0;
		}
		if (result[i] == '\0')
			tester_putstr(BOLD MAGENTA "\\0" RESET);
		else if (!isprint(result[i]))
			tester_putchar('*');
		else
			tester_putchar(result[i]);
		if (result[i] == '\0')
			break ;
		if (!expected_ended && expected[i] == '\0')
			expected_ended = 1;
		i++;
	}
	tester_putstr(RESET "]\n");
}
