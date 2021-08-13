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
int g_all_bonus = 0;
int g_all_bonus_1 = 0;
int g_all_bonus_2 = 0;


int parse_arguments(int argc, char *argv[])
{
	int options = 0;

	if (argc > 32)
		argc = 32;
	// arg 0 is the program name
	for (int i = 1; i < argc; i++)
	{
		char *str = argv[i];
		if      (strncmp(str, "c", 2) == 0)
			options = options | CAT_C;
		else if (strncmp(str, "s", 2) == 0)
			options = options | CAT_S;
		else if (strncmp(str, "p", 2) == 0)
			options = options | CAT_P;
		else if (strncmp(str, "d", 2) == 0)
			options = options | CAT_D;
		else if (strncmp(str, "i", 2) == 0)
			options = options | CAT_I;
		else if (strncmp(str, "u", 2) == 0)
			options = options | CAT_U;
		else if (strncmp(str, "x", 2) == 0)
			options = options | CAT_X;
		else if (strncmp(str, "X", 2) == 0)
			options = options | CAT_BIG_X;
		else if (strncmp(str, "%", 2) == 0)
			options = options | CAT_PERCENT;
		else if (strncmp(str, "m", 2) == 0)
			options = options | CAT_MANDATORY;
		else if (strncmp(str, "b", 2) == 0)
			options = options | CAT_BONUS_1 | CAT_BONUS_2;
		else if (strncmp(str, "b1", 3) == 0)
			options = options | CAT_BONUS_1;
		else if (strncmp(str, "b2", 3) == 0)
			options = options | CAT_BONUS_2;
		else
			g_test_nbr = atoi(str);
	}
	if (options == CAT_BONUS_1)
		g_all_bonus = CAT_BONUS_1;
	else if (options == CAT_BONUS_2)
		g_all_bonus = CAT_BONUS_2;
	if (options == (CAT_BONUS_1 | CAT_BONUS_2))
		g_all_bonus = (CAT_BONUS_1 | CAT_BONUS_2);
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
	}
	return (g_tests_failed > 0);
}
