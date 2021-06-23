#include <stdio.h>
#include <sys/wait.h>
#include <wchar.h>
#include "../libtest/libtest.h"
#include "../get_next_line/get_next_line.h"
#include "libftprintf.h"
#include <fcntl.h>

char *program_name;
char *test;
int current_test;
int test_nbr;

void pretty_printf(char *params)
{
	int i = 0;
	int inside_string = 0;

	ft_putstr(BLUE "ft_printf" RESET);
	while (params[i])
	{
		if (params[i] == '"' || params[i] == '\'')
		{
			if (inside_string)
				ft_putchar(params[i]), ft_putstr(RESET), inside_string = 0;
			else
				ft_putstr(GREEN), ft_putchar(params[i]), inside_string = 1;
		}
		else if (isdigit(params[i]) && !inside_string)
		{
			if (!isdigit(params[i - 1]))
				ft_putstr(YELLOW);
			ft_putchar(params[i]);
		}
		else
		{
			if (isdigit(params[i - 1]) && !inside_string)
				ft_putstr(RESET);
			ft_putchar(params[i]);

		}
		i++;
	}
	ft_putchar('\n');
}

int check_result(char *desc, char *params_used)
{
	if (current_test == test_nbr || test_nbr == 0)
	{
		char *result;
		char *expected;

		int orig_file = open("files/original_output.txt", O_RDONLY);
		int user_file = open("files/user_output.txt", O_RDONLY);
		get_next_line(user_file, &result);
		get_next_line(orig_file, &expected);
		ft_putnbr(current_test);
		ft_putchar('.');
		if (!test_string(desc, expected, result))
		{
			ft_putstr("        ");
			ft_putstr(BOLD "You can rerun this test with " RESET YELLOW);
			ft_putstr(program_name);
			ft_putstr(" ");
			ft_putnbr(current_test);
			ft_putstr(RESET "\n   ");
			ft_putstr("The function was called like this: ");
			pretty_printf(params_used);
		}
		else
			ft_putchar(' ');
		free(result);
		free(expected);
	}
	current_test++;
	return (0);
}

# define PRINTF(params, description) { \
	if (current_test == test_nbr || test_nbr == 0) \
	{ \
		int child = fork(); \
		if (child == 0) \
		{ \
			int file = open("files/original_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); \
			dup2(file, 1); \
			printf params; \
			return (0); \
		} \
		else \
		{ \
			waitpid(child, &wstatus, 0); \
		} \
		child = fork(); \
		if (child == 0) { \
			int file = open("files/user_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); \
			dup2(file, 1); \
			ft_printf params; \
			return (0); \
		} else { \
			waitpid(child, &wstatus, 0); \
			if (wstatus != 0) \
			{ \
				switch(wstatus - 128) { \
					case SIGSEGV: \
						printf(BOLD RED "%s.SIGSEGV! " RESET, test); \
						break; \
					case 256 * 7 - 128: \
						printf(BOLD "%s. retornou 0" RESET, test); \
						break ; \
					default: \
						printf(BOLD RED "%s.UNKNOWN CRASH! signal: (%d) " RESET, test, wstatus); \
				} \
			} \
		} \
		check_result(description, #params); \
	} \
}

void describe(char *test_title)
{
	if (test_nbr != 0)
		return ;
	ft_putstr(BOLD);
	ft_putstr(test_title);
	ft_putstr(RESET "\n   ");
}

int main(int argc, char *argv[])
{
	int wstatus;
	program_name = argv[0];
	if (argc > 2)
		return printf("WTF?\n");

	test_nbr = 0;
	if (argc == 2)
	{
		test = argv[1];
		test_nbr = atoi(test);
	}
	current_test = 1;

	describe("Basic test");
	PRINTF(("test"),
		"Print simple string");

	describe("\nTest simple %c formats");

	PRINTF(("%c", 'a'),
		"Test with a single char");

	describe("\nTest simple %s formats");

	PRINTF(("%s", ""),
		"Test printing an empty string");

	PRINTF(("this is a %s", "test"),
		"Print simple string with a single %s");

	PRINTF(("this is 1 %s with %s %s", "test", "multiple", "strings"),
		"Print simple string with multiple %s");

	PRINTF(("%s%s%s%s", "This ", "is", " an ugly ", "test"),
		"Test printing multiple, back to back %ss");

	PRINTF(("%s", "This is a rather simple test."),
		"Test printing only a %s");

	PRINTF(("t"),
		"Test printing a single char in the string");

	PRINTF(("%s", "h"),
		"Test printing a single char in the param string");

	PRINTF(("t%st%s", "a", "u"),
		"Test printing some single char parameters intercalated");

	PRINTF(("%s%s", "a", "u"),
		"Test printing some single char parameters sequentiated");

	PRINTF(("%d%s", 10, "u"),
		"Test printing a number");
	ft_putstr(RESET "\n");

}
