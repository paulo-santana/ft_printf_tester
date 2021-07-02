#include <stdio.h>
#include <sys/wait.h>
#include "../libtest/libtest.h"
#include "get_next_line.h"
#include "libftprintf.h"
#include "helpers.h"
#include "ft_printf_tester.h"
#include <fcntl.h>

char *program_name;
char *test;
int current_test;
int passed_tests = 0;
int test_nbr;

void pretty_printf(char *params)
{
	int i = 0;
	int inside_string = 0;

	tester_putstr(BLUE "ft_printf" RESET);
	while (params[i])
	{
		if (params[i] == '"' || params[i] == '\'')
		{
			if (inside_string)
				tester_putchar(params[i]), tester_putstr(RESET), inside_string = 0;
			else
				tester_putstr(GREEN), tester_putchar(params[i]), inside_string = 1;
		}
		else if (isdigit(params[i]) && !inside_string)
		{
			if (!isdigit(params[i - 1]))
				tester_putstr(YELLOW);
			tester_putchar(params[i]);
		}
		else
		{
			if (isdigit(params[i - 1]) && !inside_string)
				tester_putstr(RESET);
			tester_putchar(params[i]);

		}
		i++;
	}
	tester_putchar('\n');
}

int already_printed_help = 0;
void print_help(char *params_used)
{
	if (already_printed_help)
		return ;
	already_printed_help = 1;
	tester_putstr("\n     ");
	tester_putstr(RESET BOLD "You can rerun this test with " RESET YELLOW "make ");
	tester_putnbr(current_test);
	tester_putstr(RESET "\n     ");
	tester_putstr("The function was called like this:\n   ");
	pretty_printf(params_used);
}

int check_leaks(int success, char *params_used)
{
	char *line;
	int result = 1;
	int leaked = 0;
	(void) params_used;

	int user_stderr = open("files/user_stderr.txt", O_RDONLY);

	while (1)
	{
		result = get_next_line(user_stderr, &line);
		if (result <= 0)
		{
			free(line); break ;
		}
		if (!tester_strnstr(line, "current: 0", tester_strlen(line)))
		{
			if (success)
				tester_putstr(BOLD RED " - But there were LEAKS!! " RESET);
			else
				tester_putstr(BOLD RED " - And there were LEAKS!! " RESET);
			//print_atomic_help(params_used);
			leaked = 1;
		}
		free(line);
	}
	close(user_stderr);
	return (leaked);
}

int check_result(char *desc, char *params_used)
{
	if (current_test == test_nbr || test_nbr == 0)
	{
		char *result;
		char *expected;
		int success = 1;
		int leaked = 0;
		int wrong_return = 0;

		int orig_file = open("files/original_output.txt", O_RDONLY);
		int user_file = open("files/user_output.txt", O_RDONLY);
		get_next_line(user_file, &result);
		get_next_line(orig_file, &expected);
		success = test_string(expected, result);
		wrong_return = check_return(user_file, orig_file);
		if (success && !wrong_return)
			tester_putstr(GREEN);
		else
			tester_putstr(RED);
		tester_putnbr(current_test);
		tester_putchar('.');
		print_success(desc, success && !wrong_return);
		leaked = check_leaks(success, params_used);
		if (!success)
		{
			tester_putstr("\n");
			print_string_diff(expected, result, tester_strlen(expected) + 1);
		}
		else
			tester_putchar(' ');
		if (!success || leaked || wrong_return)
			print_help(params_used);
		else
			passed_tests++;
		free(result);
		free(expected);
		close(user_file);
		close(orig_file);
	}
	return (0);
}

void describe(char *test_title)
{
	if (test_nbr != 0)
		return ;
	tester_putstr(BOLD);
	tester_putstr(test_title);
	tester_putstr(": "RESET "\n  ");
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

	PRINTF(("This %p is even stranger", (void *)-1),
		"Test printing a pointer in the beginning of a string");

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

	PRINTF(("%u", (unsigned int)3147983649),
			"Test a bigger number");

	PRINTF(("%u", (unsigned int)4294967295),
			"Test printing the last number");

	PRINTF(("%u to the power of %d is %u", 2, 32, (unsigned int)4294967295),
			"Test printing multiple numbers");

	PRINTF(("%u%u%u%u", (unsigned int)429896724, 0, 32, (unsigned int)4294967295),
			"Test printing multiple numbers glued");

	describe("\nTest some simple %x formats");

	PRINTF(("%x", 0),
			"Another day, another positive int");

	PRINTF(("%x", 1),
			"Test printing the number 1");

	PRINTF(("%x", 10),
			"Test printing the number 10");

	PRINTF(("%x", 16),
			"Test printing the number 16");

	PRINTF(("%x", 160),
			"Test printing the number 16");

	PRINTF(("%x", 255),
			"Test printing the number 255");

	PRINTF(("%x", 256),
			"Test printing the number 256");

	PRINTF(("%x", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("the password is %x", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("%x is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("this is the real number: %x", (unsigned int)-1),
			"Test printing the max unsigned integer");

	describe("\nTest some simple %X formats");

	PRINTF(("%X", 0),
			"Yes, i'm gonna copy all of the tests for %x");

	PRINTF(("%X", 1),
			"Test printing the number 1");

	PRINTF(("%X", 10),
			"Test printing the number 10");

	PRINTF(("%X", 16),
			"Test printing the number 16");

	PRINTF(("%X", 160),
			"Test printing the number 160");

	PRINTF(("%X", 255),
			"Test printing the number 255");

	PRINTF(("%X", 256),
			"Test printing the number 256");

	PRINTF(("%X", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("the password is %X", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("%X is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("this is the real number: %X", (unsigned int)-1),
			"Test printing the max unsigned integer");

	describe("\nTest some simple %% formats");

	PRINTF(("%%"),
			"Test printing a simple %");

	PRINTF(("100%%"),
			"Test printing a simple % in the end of a string");

	PRINTF(("%%p is how you print a pointer in printf"),
			"Test printing a simple % in the beginning of a string");

	PRINTF(("the '%%%%' is used to print a %% in printf"),
			"Test printing consecutive %% in the middle of a string");

	describe("\nTest printing %c with some widths");

	PRINTF(("%1c", 'a'),
			"Test printing a single char with a small width")

	PRINTF(("%10c", 'b'),
			"Test printing a single char with a big width")

	PRINTF(("%2c", 'c'),
			"Test printing a single char with a slightly bigger width")

	PRINTF(("there are 15 spaces between this text and the next char%15c", 'd'),
			"Test printing a char with a big width in the end of a string")

	PRINTF(("%4chis paragraph is indented", 't'),
			"Test printing a char with a big width in the end of a string")

	PRINTF(("The number %7c represents luck", '7'),
			"Test printing a char with a big width in the middle of a string")

	describe("\nTest printing %s with some widths");

	PRINTF(("%1s", "a"),
			"Test printing a single char passing a width of 1");

	PRINTF(("%1s", "abc"),
			"Test printing a small string with a width of 1");

	PRINTF(("%7s", "a"),
			"Test printing a single char with a width of 7");

	PRINTF(("%7s", "abc"),
			"Test printing a small string with a width of 7");

	PRINTF(("%7s is as easy as %13s", "abc", "123"),
			"Test printing two small strings with variable lengths");

	PRINTF(("%13s are the three first letter of the %3s", "a, b and c", "alphabet"),
			"Test printing another two small strings with variable lengths");

	PRINTF(("%s%13s%42s%3s", "a, b and c", " are letters", " of the", " alphabet"),
			"Test printing glued strings with variable lengths");

	PRINTF(("%sc%13sd%42sp%3sx", "a, b and c", " are letters", " of the", " alphabet"),
			"Test printing glued strings with variable lengths");

	PRINTF(("%sc%13sd%42sp%3sx", "a, b and c", " are letters", " of the", " alphabet"),
			"Test printing glued strings with variable lengths");

	describe("\nTest printing %p with some widths");

	char c;
	PRINTF(("%1p", &c),
			"Test printing a %p with a width of 1");

	PRINTF(("%30p", &c),
			"Test printing a %p with a width of 30");

	PRINTF(("%12p", (void *)0x7ffe6b8e60c6),
			"Test printing a %p with a width of 12");

	PRINTF(("%13p", (void *)0x7ffe6b8e60c5),
			"Test printing a %p with a width of 13");

	PRINTF(("%14p", (void *)0x7ffe6b8e60c4),
			"Test printing a %p with a width of 14");

	PRINTF(("the address is %12p", (void *)0x7ffe6b8e60c7),
			"Test printing a %p with a width of 12 in the end of a string");

	PRINTF(("the address is %13p", (void *)0x7ffe6b8e60c8),
			"Test printing a %p with a width of 13 in the end of a string");

	PRINTF(("the address is %14p", (void *)0x7ffe6b8e60c9),
			"Test printing a %p with a width of 14 in the end of a string");

	PRINTF_EXPECTED(("the address is %1p", (void *)0),
			("the address is 0x0"),
			"Test printing the NULL with a width of 1 in the end of a string");

	PRINTF_EXPECTED(("the address is %2p", (void *)0),
			("the address is 0x0"),
			"Test printing the NULL with a width of 2 in the end of a string");

	PRINTF_EXPECTED(("the address is %3p", (void *)0),
			("the address is 0x0"),
			"Test printing the NULL with a width of 3 in the end of a string");

	PRINTF_EXPECTED(("the address is %4p", (void *)0),
			("the address is  0x0"),
			"Test printing the NULL with a width of 4 in the end of a string");

	PRINTF_EXPECTED(("the address is %8p", (void *)0),
			("the address is      0x0"),
			"Test printing the NULL with a width of 8 in the end of a string");

	PRINTF(("%12p is the address", (void *)0x7ffe6b8e60c7),
			"Test printing a %p with a width of 12 in the end of a string");

	PRINTF(("%13p is the address", (void *)0x7ffe6b8e60c8),
			"Test printing a %p with a width of 13 in the end of a string");

	PRINTF(("%14p is the address", (void *)0x7ffe6b8e60c9),
			"Test printing a %p with a width of 14 in the end of a string");

	PRINTF_EXPECTED(("%1p is the address", (void *)0),
			("0x0 is the address"),
			"Test printing the NULL with a width of 1 in the end of a string");

	PRINTF_EXPECTED(("%2p is the address", (void *)0),
			("0x0 is the address"),
			"Test printing the NULL with a width of 2 in the end of a string");

	PRINTF_EXPECTED(("%3p is the address", (void *)0),
			("0x0 is the address"),
			"Test printing the NULL with a width of 3 in the end of a string");

	PRINTF_EXPECTED(("%4p is the address", (void *)0),
			(" 0x0 is the address"),
			"Test printing the NULL with a width of 4 in the end of a string");

	PRINTF_EXPECTED(("%8p is the address", (void *)0),
			("     0x0 is the address"),
			"Test printing the NULL with a width of 8 in the end of a string");

	describe("\nTest printing %d with some widths");

	PRINTF(("%1d", 0),
			"Test printing 0 passing a width of 1");

	PRINTF(("%1d", -4),
			"Test printing a negative number with a width of 1");

	PRINTF(("%10d", 42),
			"Test printing a number with a width of 10");

	PRINTF(("%42d", 42000),
			"Test printing a number with a width of 42");
	
	PRINTF(("%20d", -42000),
			"Test printing a negative number with a width of 20");
	
	PRINTF(("wait for it... %50d", 42),
			"Test printing a number with a width of 50 in the end of a string");
	
	PRINTF(("%20d is how many tests are going to be made", 8000),
			"Test printing a number with a width of 50 in the beginning of a string");
	
	PRINTF(("%5d", 2147483647),
			"Test printing INT_MAX with a small width");
	
	PRINTF(("%30d", 2147483647),
			"Test printing INT_MAX with a big width");
	
	PRINTF(("%10d", 2147483647),
			"Test printing INT_MAX with its exact width");
	
	PRINTF(("%5d", (int)-2147483648),
			"Test printing INT_MIN with a small width");
	
	PRINTF(("%30d", (int)-2147483648),
			"Test printing INT_MIN with a big width");
	
	PRINTF(("%10d", (int)-2147483648),
			"Test printing INT_MIN with almost its exact width");
	
	PRINTF(("%11d", (int)-2147483648),
			"Test printing INT_MIN with its exact width");
	
	PRINTF(("%12d", (int)-2147483648),
			"Test printing INT_MIN with a slightly bigger width");
	
	PRINTF(("%12d, %20d, %2d, %42d", (int)-2147483648, 3, 30, -1),
			"Test printing some ints with varying widths");
	
	PRINTF(("%12d, %d, %2d, %42d", (int)-2147483648, 3, 30, -1),
			"Test printing some ints with varying widths and no width");
	
	PRINTF(("%14d%20d%2d%d", (int)-2147483648, 3, 30, -1),
			"Test printing some ints glued, varying widths");
	
	PRINTF(("%14dc%20ds%2dx%du", (int)-2147483648, 3, 30, -1),
			"Test printing some ints with some letters in between");

	describe("\nYes, I copied all of the tests above for %i, how did you know?");

	PRINTF(("%1i", 0),
			"Test printing 0 passing a width of 1");

	PRINTF(("%1i", -4),
			"Test printing a negative number with a width of 1");

	PRINTF(("%10i", 42),
			"Test printing a number with a width of 10");

	PRINTF(("%42i", 42000),
			"Test printing a number with a width of 42");

	PRINTF(("%20i", -42000),
			"Test printing a negative number with a width of 20");

	PRINTF(("wait for it... %50i", 42),
			"Test printing a number with a width of 50 in the end of a string");

	PRINTF(("%20i is how many tests are going to be made", 8000),
			"Test printing a number with a width of 50 in the beginning of a string");

	PRINTF(("%5i", 2147483647),
			"Test printing INT_MAX with a small width");

	PRINTF(("%30i", 2147483647),
			"Test printing INT_MAX with a big width");

	PRINTF(("%10i", 2147483647),
			"Test printing INT_MAX with its exact width");

	PRINTF(("%5i", (int)-2147483648),
			"Test printing INT_MIN with a small width");

	PRINTF(("%30i", (int)-2147483648),
			"Test printing INT_MIN with a big width");

	PRINTF(("%10i", (int)-2147483648),
			"Test printing INT_MIN with almost its exact width");

	PRINTF(("%11i", (int)-2147483648),
			"Test printing INT_MIN with its exact width");

	PRINTF(("%12i", (int)-2147483648),
			"Test printing INT_MIN with a slightly bigger width");

	PRINTF(("%12i, %20i, %2i, %42i", (int)-2147483648, 3, 30, -1),
			"Test printing some ints with varying widths");

	PRINTF(("%12i, %i, %2i, %42i", (int)-2147483648, 3, 30, -1),
			"Test printing some ints with varying widths and no width");

	PRINTF(("%14i%20i%2i%i", (int)-2147483648, 3, 30, -1),
			"Test printing some ints glued, varying widths");

	PRINTF(("%14ic%20is%2ix%du", (int)-2147483648, 3, 30, -1),
			"Test printing some ints with some letters in between");

	describe("\nTest printing %u with some widths");

	PRINTF(("%1u", 0),
			"Test printing a simple %u with a width of 1");

	PRINTF(("%2u", 1),
			"Test printing a simple %u with a width of 2");

	PRINTF(("%1u", 1000),
			"Test printing a bigger %u with a width of 1");

	PRINTF(("%4u", 1000),
			"Test printing a bigger %u with a width of 4");

	PRINTF(("%30u", 1000),
			"Test printing a bigger %u with a width of 4");

	PRINTF(("%9u is the biggest unsigned int", (unsigned int)-1),
			"Test printing the biggest %u with a width of 9");

	PRINTF(("%10uis the biggest unsigned int", (unsigned int)-1),
			"Test printing the biggest %u with a width of 10");

	PRINTF(("%11uis the biggest unsigned int", (unsigned int)-1),
			"Test printing the biggest %u with a width of 11");

	PRINTF(("the biggest unsigned int is %9u", (unsigned int)-1),
			"Test printing the biggest %u with a width of 9");

	PRINTF(("the biggest unsigned int is %10u", (unsigned int)-1),
			"Test printing the biggest %u with a width of 10");

	PRINTF(("the biggest unsigned int is %11u", (unsigned int)-1),
			"Test printing the biggest %u with a width of 11");

	PRINTF(("Here are some numbers: %1u%2u%5u%3u%9u and %ui", 11, (unsigned int)-1, 2, 200, 3, 10),
			"Test printing glued and separated numbers");

	describe("\nTest simple %x with some widths");

	PRINTF(("%1x", 0),
			"Another day, another positive int");

	PRINTF(("%2x", 1),
			"Test printing the number 1");

	PRINTF(("%3x", 10),
			"Test printing the number 10");

	PRINTF(("%1x", 16),
			"Test printing the number 16");

	PRINTF(("%2x", 160),
			"Test printing the number 16");

	PRINTF(("%3x", 255),
			"Test printing the number 255");

	PRINTF(("%42x", 256),
			"Test printing the number 256");

	PRINTF(("%7x", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("%8x", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("%9x", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("the password is %7x", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("the password is %8x", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("the password is %9x", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("%1x is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("%2x is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("%3x is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("this is the real number: %7x", (unsigned int)-1),
			"Test printing the max unsigned integer");

	PRINTF(("this is the real number: %8x", (unsigned int)-1),
			"Test printing the max unsigned integer");

	PRINTF(("this is the real number: %9x", (unsigned int)-1),
			"Test printing the max unsigned integer");

	PRINTF(("%1x%2x%9x", (unsigned int)-1, 0xf0ca, 123456),
			"Test printing some glued numbers");

	PRINTF(("%1xis doomed%2xpost%9xX args", (unsigned int)-1, 0xf0b1a, 7654321),
			"test printing some other numbers");

	describe("\nI did it again for %X");

	PRINTF(("%1X", 0),
			"Another day, another positive int");

	PRINTF(("%2X", 1),
			"Test printing the number 1");

	PRINTF(("%3X", 10),
			"Test printing the number 10");

	PRINTF(("%1X", 16),
			"Test printing the number 16");

	PRINTF(("%2X", 160),
			"Test printing the number 16");

	PRINTF(("%3X", 255),
			"Test printing the number 255");

	PRINTF(("%42X", 256),
			"Test printing the number 256");

	PRINTF(("%7X", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("%8X", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("%9X", (unsigned int)3735929054),
			"Test printing the number 3735929054");

	PRINTF(("the password is %7X", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("the password is %8X", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("the password is %9X", (unsigned int)3735929054),
			"Test printing a number in the end of the string");

	PRINTF(("%1X is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("%2X is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("%3X is the definitive answer", (unsigned int)66),
			"Test printing a number in the beginning of the string");

	PRINTF(("this is the real number: %7X", (unsigned int)-1),
			"Test printing the max unsigned integer");

	PRINTF(("this is the real number: %8X", (unsigned int)-1),
			"Test printing the max unsigned integer");

	PRINTF(("this is the real number: %9X", (unsigned int)-1),
			"Test printing the max unsigned integer");

	PRINTF(("%1X%2X%9X", (unsigned int)-1, 0xf0ca, 123456),
			"Test printing some glued numbers");

	PRINTF(("%1Xis doomed%2Xpost%9Xx args", (unsigned int)-1, 0xf0b1a, 7654321),
			"test printing some other numbers");

	describe("\nTest printing %% with some widths");

	PRINTF(("%1%"),
			"Test printing a simple % with a width of 1");

	PRINTF_EXPECTED(("%5%"),
			("    %%"),
			"Test printing a simple % with a width of 5");

	PRINTF_EXPECTED(("%2%"),
			(" %%"),
			"Test printing a simple % with a width of 2");

	PRINTF_EXPECTED(("the world is 2%10% more big today"),
			("the world is 2         %% more big today"),
			"Test printing a %% in the middle of the string");

	PRINTF_EXPECTED(("%2%%1%%%%5%%9%"),
			(" %%%%%%    %%        %%"),
			"What the fuck is that?");

	PRINTF_EXPECTED(("%2%s%1%id%%10%5%%9%"),
			(" %%s%%id%%10    %%        %%"),
			"WTFFFFFFFFFFFFFFFF???");

	PRINTF_EXPECTED(("hey, what's %5% up%%?"),
			("hey, what's     %% up%%?"),
			"Test printing a simple % with a width of 5");

	describe("\nTest some simple precisions with %s");

	PRINTF(("%.s", "hi there"),
			"Print a simple string with the precision not specified");

	PRINTF(("%.0s", "hi there"),
			"Print a simple string with a precision of 0");

	PRINTF(("%.1s", "hi there"),
			"Print a simple string with a precision of 1");

	PRINTF(("%.2s", "hi there"),
			"Print a simple string with a precision of 2");

	PRINTF(("%.3s", "hi there"),
			"Print a simple string with a precision of 3");

	PRINTF(("%.4s", "hi there"),
			"Print a simple string with a precision of 4");

	PRINTF(("%.7s", "hi there"),
			"Print a simple string with a precision of 7");

	PRINTF(("%.8s", "hi there"),
			"Print a simple string with a precision of 8");

	PRINTF(("%.9s", "hi there"),
			"Print a simple string with a precision of 9");

	PRINTF(("%.12s", "hi there"),
			"Print a simple string with a precision of 12");

	tester_putstr("\n" RESET);
	if (test_nbr == 0)
	{
		tester_putstr("\nTests passed: ");
		tester_putnbr(passed_tests);
		tester_putstr("/");
		tester_putnbr(--current_test);
		tester_putchar('\n');
	}
}
