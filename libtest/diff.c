#include "libtest.h"

void print_success(char *desc, int success)
{
	if (success)
		ft_putstr(BOLD GREEN "OK" RESET);
	else
	{
		ft_putstr(BOLD RED "KO\n      ");
		ft_putstr(desc);
		ft_putchar('\n');
	}
}

int test_string(char *description, char *expected, char *got)
{
	int success = 1;
	int len = 0;

	if (expected == NULL)
	{
		if (got != NULL)
			success = 0;
	}
	else if (got == NULL)
		success = 0;
	else if (strcmp(expected, got) != 0)
	{
		success = 0;
		len = strlen(expected) + 1;
	}
	print_success(description, success);
	if (!success)
		print_string_diff(expected, got, len);
	return (success);
}

int test_int(char *description, int expected, int result)
{
	int success = expected == result;

	if (!success)
	{
		print_success(description, success);
		print_int_diff(expected, result);
	}
	return (success);
}

