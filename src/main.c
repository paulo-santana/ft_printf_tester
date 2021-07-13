#include <stdio.h>
#include "../libtest/libtest.h"
#include "get_next_line.h"
#include "libftprintf.h"
#include "helpers.h"
#include "ft_printf_tester.h"

#include <fcntl.h>
char *program_name;
char *test;
int g_current_test;
int passed_tests = 0;
int g_test_nbr;
char g_user_fake_stdout[BUFSIZE];
char g_orig_fake_stdout[BUFSIZE];
int g_function_return;
char *g_test_params;
int g_tests_run;


int parse_arguments(int argc, char *argv[])
{
	int options = 0;

	if (argc > 32)
		argc = 32;
	// arg 0 is the program name
	for (int i = 1; i < argc; i++)
	{
		char *str = argv[i];
		if      (strncmp(str, "c", 1) == 0)
			options = options | CAT_C;
		else if (strncmp(str, "s", 1) == 0)
			options = options | CAT_S;
		else if (strncmp(str, "p", 1) == 0)
			options = options | CAT_P;
		else if (strncmp(str, "d", 1) == 0)
			options = options | CAT_D;
		else if (strncmp(str, "i", 1) == 0)
			options = options | CAT_I;
		else if (strncmp(str, "u", 1) == 0)
			options = options | CAT_U;
		else if (strncmp(str, "x", 1) == 0)
			options = options | CAT_X;
		else if (strncmp(str, "X", 1) == 0)
			options = options | CAT_BIG_X;
		else if (strncmp(str, "%", 1) == 0)
			options = options | CAT_PERCENT;
		else if (strncmp(str, "m", 1) == 0)
			options = options | CAT_MANDATORY;
		else if (strncmp(str, "b", 1) == 0)
			options = options | CAT_BONUS;
		else
			g_test_nbr = atoi(str);
	}
	return (options);
}

int main(int argc, char *argv[])
{
	int tests;
	program_name = argv[0];
	tests = parse_arguments(argc, argv);

	g_test_nbr = 0;
	if (argc == 2)
	{
		test = argv[1];
		g_test_nbr = atoi(test);
	}
	g_current_test = 1;

	run_tests(tests);

	tester_putstr("\n" RESET);
	if (g_test_nbr == 0)
	{
		printf("\nTests run: %d out of %d", g_tests_run, g_current_test - 1);
		printf("\nTests OK:  %d (%d%%)\n", passed_tests, (int)((float)passed_tests / g_tests_run * 100));
		//tester_putstr("\nTests passed: ");
		//tester_putnbr(passed_tests);
		//tester_putstr("/");
		//tester_putnbr(--g_current_test);
		//tester_putchar('\n');
	}
}
