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

/** The file descriptor for the user output */
int check_result(char *desc)
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
			ft_putstr(BOLD "You can rerun this test with " YELLOW);
			ft_putstr(program_name);
			ft_putstr(" ");
			ft_putnbr(current_test);
			ft_putstr(RESET "\n   ");
		}
		else
			ft_putchar(' ');
		free(result);
		free(expected);
	}
	current_test++;
	return (0);
}

# define PRINTF(params) { \
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
				return (0); \
			} \
		} \
	} \
}

void describe(char *test_title)
{
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
	PRINTF(("test"));
	check_result("Print simple string");

	describe("\nTest simple %c formats");

	PRINTF(("%c", 'a'));
	check_result("Print simple string");

	describe("\nTest simple %s formats");

	PRINTF(("%s", ""));
	check_result("Test printing an empty string");

	PRINTF(("this is a %s", "test"));
	check_result("Print simple string with a single %s");

	PRINTF(("this is a %s with %s %s", "test", "multiple", "strings"));
	check_result("Print simple string with multiple %s");

	PRINTF(("%s%s%s%s", "This ", "is", " an ugly ", "test"));
	check_result("Test printing multiple, back to back %ss");

	PRINTF(("%s", "This is a rather simple test."));
	check_result("Test printing only a %s");

	PRINTF(("t"));
	check_result("Test printing a single char in the string");

	PRINTF(("%s", "h"));
	check_result("Test printing a single char in the param string");

	PRINTF(("t%st%s", "a", "u"));
	check_result("Test printing some single char parameters intercalated");

	PRINTF(("%s%s", "a", "u"));
	check_result("Test printing some single char parameters intercalated");

	describe("\nTest %s with flags");

	ft_putstr(RESET "\n");

}
