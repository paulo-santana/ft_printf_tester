#include <stdio.h>
#include <sys/wait.h>
#include "../libtest/libtest.h"
#include "get_next_line.h"
#include "libftprintf.h"
#include "helpers.h"
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

int already_printed_help = 0;
void print_help(char *params_used)
{
	if (already_printed_help)
		return ;
	already_printed_help = 1;
	ft_putstr("\n     ");
	ft_putstr(RESET BOLD "You can rerun this test with " RESET YELLOW "make ");
	ft_putnbr(current_test);
	ft_putstr(RESET "\n     ");
	ft_putstr("The function was called like this:\n   ");
	pretty_printf(params_used);
}

int check_leaks(int success, char *params_used)
{
	char *line;
	int result = 1;
	int leaked = 0;
	(void) params_used;

	int user_stderr = open("files/user_stderr.txt", O_RDONLY);

	while (result > 0)
	{
		result = get_next_line(user_stderr, &line);
		if (result != 1)
			break ;

		if (!tester_strnstr(line, "current: 0", tester_strlen(line)))
		{
			if (success)
				ft_putstr(BOLD RED " - But there were LEAKS!! " RESET);
			else
				ft_putstr(BOLD RED " - And there were LEAKS!! " RESET);
			//print_atomic_help(params_used);
			leaked = 1;
		}
		free(line);
	}
	free(line);
	return (leaked);
}

int check_return()
{
	int failed = 0;

	return (failed);
}

int check_result(char *desc, char *params_used)
{
	if (current_test == test_nbr || test_nbr == 0)
	{
		char *result;
		char *expected;
		int success = 1;
		int leaked = 0;
		int wrong_return = 1;

		int orig_file = open("files/original_output.txt", O_RDONLY);
		int user_file = open("files/user_output.txt", O_RDONLY);
		get_next_line(user_file, &result);
		get_next_line(orig_file, &expected);
		success = test_string(expected, result);
		wrong_return = check_return();
		if (success && !wrong_return)
			ft_putstr(GREEN);
		else
			ft_putstr(RED);
		ft_putnbr(current_test);
		ft_putchar('.');
		print_success(desc, success && !wrong_return);
		leaked = check_leaks(success, params_used);
		if (!success)
		{
			ft_putstr("\n");
			print_string_diff(expected, result, tester_strlen(expected) + 1);
		}
		else
			ft_putchar(' ');
		if (!success || leaked || wrong_return)
			print_help(params_used);
		free(result);
		free(expected);
	}
	return (0);
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
	PRINTF(("1, 2, 3, test, testing, sound, 1, 2, 3, sound, test"),
		"Print simple string");

	describe("\nTest simple %c formats");

	PRINTF(("%c", 'a'),
		"Test with a single char");

	PRINTF(("%c small string", 'a'),
		"Test with a single char in the begining of a small string");

	PRINTF(("the char is: %c", 'a'),
		"Test with a single char in the end of a small string");

	PRINTF(("n%cs", 'a'),
		"Test printing a single char in the middle of a 3 letter string");

	PRINTF(("%c%c%c%c%c", 'a', 'i', 'u', 'e', 'o'),
		"Test printing many sequentiated chars");

	PRINTF(("l%cl%cl%cl%cl%c", 'a', 'i', 'u', 'e', 'o'),
		"Test printing many intercalated chars");

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

	PRINTF(("%s%s%s%s%s%s", "a", "i", "u", "e", "o", "l"),
		"Test printing some single char parameters sequentiated");

	describe("\nTest simple %p formats");

	int test = 42;
	PRINTF(("%p", &test),
		"Test printing a simple pointer");

	PRINTF(("%p is a virtual memory address", &test),
		"Test printing a pointer in the beginning of a string");

	PRINTF(("The address of the answer is %p", &test),
		"Test printing a pointer in the end of a string");

	PRINTF(("The address is %p, so what?", &test),
		"Test printing a pointer in the middle of a string");

	int *ptr = &test;
	PRINTF(("A pointer at %p points to %p", &test, &ptr),
		"Test printing multiple pointers");

	PRINTF(("This %p is a very strange address", (void *)(long int)test),
		"Test printing a pointer in the beginning of a string");

	char *mallocked = malloc(2);
	PRINTF(("This %p is an address from the heap", mallocked); free(mallocked);,
		"Test printing a mallocked pointer, because why not?");
	free(mallocked);

	PRINTF_EXPECTED(("%p", NULL),
			("0x0"),
			"Test printing the NULL pointer");

	PRINTF_EXPECTED(("The NULL macro represents the %p address", NULL),
			("The NULL macro represents the 0x0 address"),
			"Test printing the NULL inside some text");

	describe("\nTest simple %d formats");

	PRINTF(("%d", 0),
		 "Test printing 0");

	PRINTF(("%d", 10),
		 "Test printing only a number");

	PRINTF(("%d, %d", 10, 20),
		 "Test printing two numbers");

	PRINTF(("%d%d%d%d", 10, 20, 30, 5),
		 "Test printing consecutive numbers");

	PRINTF(("%d %d", 2147483647, (int)-2147483648),
		 "Test printing INT_MAX and INT_MIN");

	PRINTF(("42 - 84 is %d", -42),
		 "Test printing a negative number in the end of a string");

	PRINTF(("%d C is the lowest temperature in the universe", -273),
		 "Test printing a negative number in the beginning of a string");

	describe("\nTest simple %i formats");

	PRINTF(("%i", 0),
			"Test printing 0");

	PRINTF(("%i", 10),
			"Test printing a single number");

	PRINTF(("%i, %i", 10, 23),
			"Test printing some more numbers");

	PRINTF(("%i%i%i%i%i%i%i", 10, 23, -2, 37, 200, -9999, 977779),
			"Test printing more consecutive numbers");

	PRINTF(("%i %i", 2147483647, (int)-2147483648),
			"Test printing INT_MAX and INT_MIN");

	PRINTF(("%iq%i", 21447, -21648),
			"Did you notice these tests are becoming repetitive?");

	describe("\nTest some simple %u formats");

	PRINTF(("%u", 42),
			"Another day, another positive int");

	PRINTF(("%u", 0),
			"Test printing 0");

	PRINTF(("%u", 2147483647),
			"Test printing INT_MAX");

	PRINTF(("%u", (unsigned int)2147483648),
			"Test printing INT_MAX + 1");

	ft_putstr(RESET "\n");

}
