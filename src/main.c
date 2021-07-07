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
			if (i > 0 && isdigit(params[i - 1]) && !inside_string)
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

int check_leaks_sanitizer(int user_stderr)
{
	int n = 0;
	int error = 0;
	char *line;
	int result = get_next_line(user_stderr, &line);
	free(line);
	if (result == 0)
		return (0);
	result = get_next_line(user_stderr, &line); // get rid of the first line
	n = strlen(line);
	if (tester_strnstr(line, "heap-buffer-overflow", n))
	{
		error = ERRORS_BUFFER_OVERFLOW;
	}
	free(line);
	result = get_next_line(user_stderr, &line); // get rid of the first line
	n = strlen(line);
	if (tester_strnstr(line, "leaks", n))
		error = ERRORS_LEAK;
	else if (tester_strnstr(line, "SEGV", n))
		error = ERRORS_SIGSEGV;
	free(line);
	// get rid of the rest of the file
	char dummy_buffer[100];
	while (read(user_stderr, dummy_buffer, 100));
	get_next_line(user_stderr, &line);
	free(line);
	return (error);
}

int check_leaks_malloc_count(int user_stderr)
{
	int result;
	int leaked = 0;
	char *line;

	while (1)
	{
		result = get_next_line(user_stderr, &line);
		if (result <= 0)
		{
			free(line); break ;
		}
		if (!tester_strnstr(line, "current: 0", tester_strlen(line)))
		{
			leaked = 1;
		}
		free(line);
	}
	return (leaked);
}

int check_errors(char *params_used)
{
	int error = 0;
	(void) params_used;

	int user_stderr = open("files/user_stderr.txt", O_RDONLY);

	if (LEAK_CHECKER == LEAK_SANITIZER)
		error = check_leaks_sanitizer(user_stderr);
	else if (LEAK_CHECKER == MALLOC_COUNT)
		error = check_leaks_malloc_count(user_stderr);
	close(user_stderr);
	return (error);
}

int check_result(char *params_used)
{
	if (current_test == test_nbr || test_nbr == 0)
	{
		char *result;
		char *expected;
		int success = 1;
		int errors = 0;
		int wrong_return = 0;

		int orig_file = open("files/original_output.txt", O_RDONLY);
		int user_file = open("files/user_output.txt", O_RDONLY);
		get_next_line(user_file, &result);
		get_next_line(orig_file, &expected);

		errors = check_errors(params_used);
		if (!errors || errors == ERRORS_LEAK)
		{
			success = test_string(expected, result);
			wrong_return = check_return(user_file, orig_file);
		}

		if (success && !wrong_return && !errors)
			tester_putstr(GREEN);
		else
			tester_putstr(BOLD RED "\n  ");
		tester_putnbr(current_test);
		tester_putchar('.');

		if (success && !wrong_return && (!errors || errors == ERRORS_LEAK))
			tester_putstr(BOLD "OK" RESET);
		else if (errors) {
			if (errors == ERRORS_SIGSEGV)
				tester_putstr(BOLD "SIGSEGV!" RESET RED " - check files/user_stderr.txt");

		}
		else
			tester_putstr("KO (Wrong output)");

		if (wrong_return)
			tester_putstr(" (Wrong return)");
		if (errors == ERRORS_LEAK)
		{
			tester_putstr(RED " (LEAKS!)");
		}
		if (!success)
		{
			tester_putstr("\n");
			print_string_diff(expected, result, tester_strlen(expected) + 1);
		}
		else
			tester_putchar(' ');
		if (!success || errors || wrong_return)
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

	PRINTF(("1, 2, 3, -d test, testing, 0.4s sound, 1, 2, 3xp, sound, -*dtest"));

	describe("\nTest simple %c formats");

	PRINTF(("%c", 'a'));
	PRINTF(("%c small string", 'a'));
	PRINTF(("the char is: %c", 'a'));
	PRINTF(("n%cs", 'a'));
	PRINTF(("%c%c%c%c%c", 'a', 'i', 'u', 'e', 'o'));
	PRINTF(("l%cl%cl%cl%cl%c", 'a', 'i', 'u', 'e', 'o'));

	describe("\nTest simple %s formats");

	PRINTF(("%s", ""));
	PRINTF(("this is a %s", "test"));
	PRINTF(("this is 1 %s with %s %s", "test", "multiple", "strings"));
	PRINTF(("%s%s%s%s", "This ", "is", " an ugly ", "test"));
	PRINTF(("%s", "This is a rather simple test."));
	PRINTF(("%s", "-2"));
	PRINTF(("%s", "-24"));
	PRINTF(("%s", "-stop"));
	char *null_str = NULL;
	PRINTF(("%s", null_str));
	PRINTF(("%s", "h"));
	PRINTF(("t%st%s", "a", "u"));
	PRINTF(("%s%s%s%s%s%s", "a", "i", "u", "e", "o", "l"));

	describe("\nTest simple %p formats");

	int test = 42;
	PRINTF(("%p", &test));
	PRINTF(("%p is a virtual memory address", &test));
	PRINTF(("The address of the answer is %p", &test));
	PRINTF(("The address is %p, so what?", &test));
	int *ptr = &test;
	PRINTF(("A pointer at %p points to %p", &test, &ptr));
	PRINTF(("This %p is a very strange address", (void *)(long int)test));

	char *mallocked = malloc(2);
	PRINTF(("This %p is an address from the heap", mallocked); free(mallocked);); free(mallocked);
	PRINTF_EXPECTED(("%p", NULL),                       /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("The NULL macro represents the %p address", NULL), ("The NULL macro represents the 0x0 address"));
	PRINTF(("This %p is even stranger", (void *)-1));

	describe("\nTest simple %d formats");

	PRINTF(("%d", 0));
	PRINTF(("%d", 10));
	PRINTF(("%d, %d", 10, 20));
	PRINTF(("%d%d%d%d", 10, 20, 30, 5));
	PRINTF(("%d %d", 2147483647, (int)-2147483648));
	PRINTF(("42 - 84 is %d", -42));
	PRINTF(("%d C is the lowest temperature in the universe", -273));

	describe("\nTest simple %i formats");

	PRINTF(("%i", 0));
	PRINTF(("%i", 10));
	PRINTF(("%i, %i", 10, 23));
	PRINTF(("%i%i%i%i%i%i%i", 10, 23, -2, 37, 200, -9999, 977779));
	PRINTF(("%i %i", 2147483647, (int)-2147483648));
	PRINTF(("%iq%i", 21447, -21648));

	describe("\nTest some simple %u formats");

	PRINTF(("%u", 42));
	PRINTF(("%u", 0));
	PRINTF(("%u", 2147483647));
	PRINTF(("%u", (unsigned int)2147483648));
	PRINTF(("%u", (unsigned int)3147983649));
	PRINTF(("%u", (unsigned int)4294967295));
	PRINTF(("%u to the power of %d is %u", 2, 32, (unsigned int)4294967295));
	PRINTF(("%u%u%u%u", (unsigned int)429896724, 0, 32, (unsigned int)4294967295));

	describe("\nTest some simple %x formats");

	PRINTF(("%x", 0));
	PRINTF(("%x", 1));
	PRINTF(("%x", 10));
	PRINTF(("%x", 16));
	PRINTF(("%x", 160));
	PRINTF(("%x", 255));
	PRINTF(("%x", 256));
	PRINTF(("%x", (unsigned int)3735929054));
	PRINTF(("the password is %x", (unsigned int)3735929054));
	PRINTF(("%x is the definitive answer", (unsigned int)66));
	PRINTF(("this is the real number: %x", (unsigned int)-1));

	describe("\nTest some simple %X formats");

	PRINTF(("%X", 0));
	PRINTF(("%X", 1));
	PRINTF(("%X", 10));
	PRINTF(("%X", 16));
	PRINTF(("%X", 160));
	PRINTF(("%X", 255));
	PRINTF(("%X", 256));
	PRINTF(("%X", (unsigned int)3735929054));
	PRINTF(("the password is %X", (unsigned int)3735929054));
	PRINTF(("%X is the definitive answer", (unsigned int)66));
	PRINTF(("this is the real number: %X", (unsigned int)-1));

	describe("\nTest some simple %% formats");

	PRINTF(("%%"));
	PRINTF(("100%%"));
	PRINTF(("%%p is how you print a pointer in printf"));
	PRINTF(("the '%%%%' is used to print a %% in printf"));

	describe("\nTest printing %c with some widths");

	PRINTF(("%1c", 'a'))
	PRINTF(("%10c", 'b'))
	PRINTF(("%2c", 'c'))
	PRINTF(("there are 15 spaces between this text and the next char%15c", 'd'))
	PRINTF(("%4chis paragraph is indented", 't'))
	PRINTF(("The number %7c represents luck", '7'))

	describe("\nTest printing %s with some widths");

	PRINTF(("%1s", "a"));
	PRINTF(("%1s", "abc"));
	PRINTF(("%7s", "a"));
	PRINTF(("%7s", "abc"));
	PRINTF(("%1s", "-42"));
	PRINTF(("%2s", "-42"));
	PRINTF(("%3s", "-42"));
	PRINTF(("%4s", "-42"));
	PRINTF(("%5s", "-42"));
	PRINTF(("%6s", "-42"));
	PRINTF(("%1s", null_str));
	PRINTF(("%2s", null_str));
	PRINTF(("%5s", null_str));
	PRINTF(("%6s", null_str));
	PRINTF(("%7s", null_str));
	PRINTF(("%7s is as easy as %13s", "abc", "123"));
	PRINTF(("%13s are the three first letter of the %3s", "a, b and c", "alphabet"));
	PRINTF(("%s%13s%42s%3s", "a, b and c", " are letters", " of the", " alphabet"));
	PRINTF(("%sc%13sd%42sp%3sx", "a, b and c", " are letters", " of the", " alphabet"));
	PRINTF(("%sc%13sd%42sp%3sx", "a, b and c", " are letters", " of the", " alphabet"));

	describe("\nTest printing %p with some widths");

	char c;
	PRINTF(("%1p", &c));
	PRINTF(("%30p", &c));
	PRINTF(("%12p", (void *)0x7ffe6b8e60c6));
	PRINTF(("%13p", (void *)0x7ffe6b8e60c5));
	PRINTF(("%14p", (void *)0x7ffe6b8e60c4));
	PRINTF(("the address is %12p", (void *)0x7ffe6b8e60c7));
	PRINTF(("the address is %13p", (void *)0x7ffe6b8e60c8));
	PRINTF(("the address is %14p", (void *)0x7ffe6b8e60c9));
	PRINTF_EXPECTED(("the address is %1p", (void *)0), /* expected: */ ("the address is 0x0"));
	PRINTF_EXPECTED(("the address is %2p", (void *)0), /* expected: */ ("the address is 0x0"));
	PRINTF_EXPECTED(("the address is %3p", (void *)0), /* expected: */ ("the address is 0x0"));
	PRINTF_EXPECTED(("the address is %4p", (void *)0), /* expected: */ ("the address is  0x0"));
	PRINTF_EXPECTED(("the address is %8p", (void *)0), /* expected: */ ("the address is      0x0"));
	PRINTF(("%12p is the address", (void *)0x7ffe6b8e60c7));
	PRINTF(("%13p is the address", (void *)0x7ffe6b8e60c8));
	PRINTF(("%14p is the address", (void *)0x7ffe6b8e60c9));
	PRINTF_EXPECTED(("%1p is the address", (void *)0), /* expected: */ ("0x0 is the address"));
	PRINTF_EXPECTED(("%2p is the address", (void *)0), /* expected: */ ("0x0 is the address"));
	PRINTF_EXPECTED(("%3p is the address", (void *)0), /* expected: */ ("0x0 is the address"));
	PRINTF_EXPECTED(("%4p is the address", (void *)0), /* expected: */ (" 0x0 is the address"));
	PRINTF_EXPECTED(("%8p is the address", (void *)0), /* expected: */ ("     0x0 is the address"));

	describe("\nTest printing %d with some widths");

	PRINTF(("%1d", 0));
	PRINTF(("%1d", -4));
	PRINTF(("%10d", 42));
	PRINTF(("%42d", 42000));
	PRINTF(("%20d", -42000));
	PRINTF(("wait for it... %50d", 42));
	PRINTF(("%20d is how many tests are going to be made", 8000));
	PRINTF(("%5d", 2147483647));
	PRINTF(("%30d", 2147483647));
	PRINTF(("%10d", 2147483647));
	PRINTF(("%5d", (int)-2147483648));
	PRINTF(("%30d", (int)-2147483648));
	PRINTF(("%10d", (int)-2147483648));
	PRINTF(("%11d", (int)-2147483648));
	PRINTF(("%12d", (int)-2147483648));
	PRINTF(("%12d, %20d, %2d, %42d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%12d, %d, %2d, %42d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%14d%20d%2d%d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%14dc%20ds%2dx%du", (int)-2147483648, 3, 30, -1));

	describe("\nYes, I copied all of the tests above for %i, how did you know?");

	PRINTF(("%1i", 0));
	PRINTF(("%1i", -4));
	PRINTF(("%10i", 42));
	PRINTF(("%42i", 42000));
	PRINTF(("%20i", -42000));
	PRINTF(("wait for it... %50i", 42));
	PRINTF(("%20i is how many tests are going to be made", 8000));
	PRINTF(("%5i", 2147483647));
	PRINTF(("%30i", 2147483647));
	PRINTF(("%10i", 2147483647));
	PRINTF(("%5i", (int)-2147483648));
	PRINTF(("%30i", (int)-2147483648));
	PRINTF(("%10i", (int)-2147483648));
	PRINTF(("%11i", (int)-2147483648));
	PRINTF(("%12i", (int)-2147483648));
	PRINTF(("%12i, %20i, %2i, %42i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%12i, %i, %2i, %42i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%14i%20i%2i%i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%14ic%20is%2ix%du", (int)-2147483648, 3, 30, -1));

	describe("\nTest printing %u with some widths");

	PRINTF(("%1u", 0));
	PRINTF(("%2u", 1));
	PRINTF(("%1u", 1000));
	PRINTF(("%4u", 1000));
	PRINTF(("%30u", 1000));
	PRINTF(("%9u is the biggest unsigned int", (unsigned int)-1));
	PRINTF(("%10uis the biggest unsigned int", (unsigned int)-1));
	PRINTF(("%11uis the biggest unsigned int", (unsigned int)-1));
	PRINTF(("the biggest unsigned int is %9u", (unsigned int)-1));
	PRINTF(("the biggest unsigned int is %10u", (unsigned int)-1));
	PRINTF(("the biggest unsigned int is %11u", (unsigned int)-1));
	PRINTF(("Here are some numbers: %1u%2u%5u%3u%9u and %ui", 11, (unsigned int)-1, 2, 200, 3, 10));

	describe("\nTest simple %x with some widths");

	PRINTF(("%1x", 0));
	PRINTF(("%2x", 1));
	PRINTF(("%3x", 10));
	PRINTF(("%1x", 16));
	PRINTF(("%2x", 160));
	PRINTF(("%3x", 255));
	PRINTF(("%42x", 256));
	PRINTF(("%7x", (unsigned int)3735929054));
	PRINTF(("%8x", (unsigned int)3735929054));
	PRINTF(("%9x", (unsigned int)3735929054));
	PRINTF(("the password is %7x", (unsigned int)3735929054));
	PRINTF(("the password is %8x", (unsigned int)3735929054));
	PRINTF(("the password is %9x", (unsigned int)3735929054));
	PRINTF(("%1x is the definitive answer", (unsigned int)66));
	PRINTF(("%2x is the definitive answer", (unsigned int)66));
	PRINTF(("%3x is the definitive answer", (unsigned int)66));
	PRINTF(("this is the real number: %7x", (unsigned int)-1));
	PRINTF(("this is the real number: %8x", (unsigned int)-1));
	PRINTF(("this is the real number: %9x", (unsigned int)-1));
	PRINTF(("%1x%2x%9x", (unsigned int)-1, 0xf0ca, 123456));
	PRINTF(("%1xis doomed%2xpost%9xX args", (unsigned int)-1, 0xf0b1a, 7654321));

	describe("\nI did it again for %X");

	PRINTF(("%1X", 0));
	PRINTF(("%2X", 1));
	PRINTF(("%3X", 10));
	PRINTF(("%1X", 16));
	PRINTF(("%2X", 160));
	PRINTF(("%3X", 255));
	PRINTF(("%42X", 256));
	PRINTF(("%7X", (unsigned int)3735929054));
	PRINTF(("%8X", (unsigned int)3735929054));
	PRINTF(("%9X", (unsigned int)3735929054));
	PRINTF(("the password is %7X", (unsigned int)3735929054));
	PRINTF(("the password is %8X", (unsigned int)3735929054));
	PRINTF(("the password is %9X", (unsigned int)3735929054));
	PRINTF(("%1X is the definitive answer", (unsigned int)66));
	PRINTF(("%2X is the definitive answer", (unsigned int)66));
	PRINTF(("%3X is the definitive answer", (unsigned int)66));
	PRINTF(("this is the real number: %7X", (unsigned int)-1));
	PRINTF(("this is the real number: %8X", (unsigned int)-1));
	PRINTF(("this is the real number: %9X", (unsigned int)-1));
	PRINTF(("%1X%2X%9X", (unsigned int)-1, 0xf0ca, 123456));
	PRINTF(("%1Xis doomed%2Xpost%9Xx args", (unsigned int)-1, 0xf0b1a, 7654321));

	describe("\nTest printing %% with some widths");

	PRINTF(("%1%"));
	PRINTF_EXPECTED(("%5%"),                               /* expected: */ ("    %%"));
	PRINTF_EXPECTED(("%2%"),                               /* expected: */ (" %%"));
	PRINTF_EXPECTED(("the world is 2%10% more big today"), /* expected: */ ("the world is 2         %% more big today"));
	PRINTF_EXPECTED(("%2%%1%%%%5%%9%"),                    /* expected: */ (" %%%%%%    %%        %%"));
	PRINTF_EXPECTED(("%2%s%1%id%%10%5%%9%"),               /* expected: */ (" %%s%%id%%10    %%        %%"));
	PRINTF_EXPECTED(("hey, what's %5% up%%?"),             /* expected: */ ("hey, what's     %% up%%?"));

	describe("\nTest some simple precisions with %s");

	PRINTF(("%.s", "hi there"));
	PRINTF(("%.0s", "hi there"));
	PRINTF(("%.1s", "hi there"));
	PRINTF(("%.2s", "hi there"));
	PRINTF(("%.3s", "hi there"));
	PRINTF(("%.4s", "hi there"));
	PRINTF(("%.7s", "hi there"));
	PRINTF(("%.8s", "hi there"));
	PRINTF(("%.9s", "hi there"));
	PRINTF(("%.12s", "hi there"));
	PRINTF(("%.s", "-42"));
	PRINTF(("%.0s", "-42"));
	PRINTF(("%.1s", "-42"));
	PRINTF(("%.2s", "-42"));
	PRINTF(("%.3s", "-42"));
	PRINTF(("%.4s", "-42"));
	PRINTF(("%.7s", "-42"));
	PRINTF_EXPECTED(("%.1s", null_str), /* expected: */ ("("));
	PRINTF_EXPECTED(("%.2s", null_str), /* expected: */ ("(n"));
	PRINTF_EXPECTED(("%.5s", null_str), /* expected: */ ("(null"));
	PRINTF(("%.6s", null_str));
	PRINTF(("%.7s", null_str));
	PRINTF(("%.2s, motherfucker", "hi there"));
	PRINTF(("This %.3s a triumph ", "wasabi"));
	PRINTF(("%.4s making a %.4s here: %.13s", "I'm delighted", "notation", "HUGE SUCCESS!"));
	PRINTF(("It's %.4s to over%.50s my%s", "hardware", "state", " satisfaction"));
	PRINTF(("%.11s%.6s%.4s", "Aperture", " Scientists", "ce"));

	describe("\nTest some simple precisions with %d");

	PRINTF(("%.1d", 2))
	PRINTF(("%.2d", 3))
	PRINTF(("%.4d", 32))
	PRINTF(("%.3d", 420000))
	PRINTF(("%.0d", 420000))
	PRINTF(("%.3d", -1))
	PRINTF(("%.3d", -1234))
	PRINTF(("%.4d", -1234))
	PRINTF(("%.5d", -1234))
	PRINTF(("%.5d", (int)-2147483648))
	PRINTF(("%.9d", (int)-2147483648))
	PRINTF(("%.10d", (int)-2147483648))
	PRINTF(("%.11d", (int)-2147483648))
	PRINTF(("%.12d", (int)-2147483648))
	PRINTF(("%.13d", (int)-2147483648))
	PRINTF(("%.5d", 2147483647))
	PRINTF(("%.9d", 2147483647))
	PRINTF(("%.10d", 2147483647))
	PRINTF(("%.11d", 2147483647))
	PRINTF(("%.12d", 2147483647))
	PRINTF(("%.0d", 2))
	PRINTF(("%.0d", 2147483647))
	PRINTF(("%.0d", 0))
	PRINTF(("%.0d", 10))
	PRINTF(("%.d", 10))
	PRINTF(("%.d", 0))
	PRINTF(("I'm gonna watch %.3d", 7))
	PRINTF(("%.3d is the movie I'm gonna watch", 7))
	PRINTF(("Then take these %.7d things and get the hell out of here", 2))
	PRINTF(("Bla %.2di bla %.5dsbla bla %.dx bla %.d", 127, 42, 1023, 0))
	PRINTF(("%.4d%.2d%.20d%.0d%.0d%.d%.d%.d", 127, 0, 1023, 0, (int)-2147483648, 0, 1, (int)-2147483648))

	describe("\nTest some simple precisions with %i");

	PRINTF(("%.1i", 7))
	PRINTF(("%.3i", 7))
	PRINTF(("%.2i", 3))
	PRINTF(("%.4i", 32))
	PRINTF(("%.3i", 420000))
	PRINTF(("%.0i", 420000))
	PRINTF(("%.3i", -1))
	PRINTF(("%.3i", -1234))
	PRINTF(("%.4i", -1234))
	PRINTF(("%.5i", -1234))
	PRINTF(("%.5i", (int)-2147483648))
	PRINTF(("%.9i", (int)-2147483648))
	PRINTF(("%.10i", (int)-2147483648))
	PRINTF(("%.11i", (int)-2147483648))
	PRINTF(("%.12i", (int)-2147483648))
	PRINTF(("%.13i", (int)-2147483648))
	PRINTF(("%.5i", 2147483647))
	PRINTF(("%.9i", 2147483647))
	PRINTF(("%.10i", 2147483647))
	PRINTF(("%.11i", 2147483647))
	PRINTF(("%.12i", 2147483647))
	PRINTF(("%.0i", 2))
	PRINTF(("%.0i", 2147483647))
	PRINTF(("%.0i", 0))
	PRINTF(("%.0i", 10))
	PRINTF(("%.i", 10))
	PRINTF(("%.i", 0))
	PRINTF(("I'm gonna watch %.3i", 7))
	PRINTF(("%.3i is the movie I'm gonna watch", 7))
	PRINTF(("Then take these %.7i things and get the hell out of here", 2))
	PRINTF(("Bla %.2ii bla %.5isbla bla %.ix bla %.i", 127, 42, 1023, 0))
	PRINTF(("%.4i%.2i%.20i%.0i%.0i%.i%.i%.i", 127, 0, 1023, 0, (int)-2147483648, 0, 1, (int)-2147483648))

	describe("\nTest some simple precisions with %u");

	PRINTF(("%.1u", 1))
	PRINTF(("%.2u", 1))
	PRINTF(("%.2u", 0))
	PRINTF(("%.0u", 0))
	PRINTF(("%.u", 0))
	PRINTF(("%.2u", 30000))
	PRINTF(("%.20u", 30000))
	PRINTF(("%.0u", (unsigned int)-1))
	PRINTF(("%.5u", (unsigned int)-1))
	PRINTF(("%.9u", (unsigned int)-1))
	PRINTF(("%.10u", (unsigned int)-1))
	PRINTF(("%.11u", (unsigned int)-1))
	PRINTF(("%.10uis a big number", (unsigned int)-1))
	PRINTF(("%.0uis a big number", (unsigned int)-1))
	PRINTF(("%.4us a big number", (unsigned int)-1))
	PRINTF(("%.9uxs a big number", (unsigned int)-1))
	PRINTF(("%.11ups a big number", (unsigned int)-1))
	PRINTF(("the number is %.0u", (unsigned int)-1))
	PRINTF(("the number is %.u", (unsigned int)-1))
	PRINTF(("the number is %.5u", (unsigned int)-1))
	PRINTF(("the number is %.9u", (unsigned int)-1))
	PRINTF(("the number is %.10u", (unsigned int)-1))
	PRINTF(("the number is %.11u", (unsigned int)-1))
	PRINTF(("the number is %.11u", (unsigned int)-1))
	PRINTF(("%.0uis a big number", 0))
	PRINTF(("%.4us a big number", 0))
	PRINTF(("the number is %.0u", 0))
	PRINTF(("the number is %.u", 0))
	PRINTF(("the number is %.5u", 0))
	PRINTF(("%u%.5u%.0u%.u%.9u", 5, 55, 2, 0, 42))
	PRINTF(("%us%.5ui%.0uc%.up%.9ux", 5, 55, 2, 0, 42))

	describe("\nTest some simple precisions with %x");

	PRINTF(("%.1x", 0xa))
	PRINTF(("%.4x", 11))
	PRINTF(("%.0x", 0))
	PRINTF(("%.1x", -1))
	PRINTF(("%.10x", -1))
	PRINTF(("%.14x", -1))
	PRINTF(("%.8x", 0))
	PRINTF(("%.2x", 30000))
	PRINTF(("%.20x", 30000))
	PRINTF(("%.0x", (unsigned int)-1))
	PRINTF(("%.5x", (unsigned int)-1))
	PRINTF(("%.9x", (unsigned int)-1))
	PRINTF(("%.10x", (unsigned int)-1))
	PRINTF(("%.11x", (unsigned int)-1))
	PRINTF(("%.10xis a big number", (unsigned int)-1))
	PRINTF(("%.0xis a big number", (unsigned int)-1))
	PRINTF(("%.4xs a big number", (unsigned int)-1))
	PRINTF(("%.9xxs a big number", (unsigned int)-1))
	PRINTF(("%.11xps a big number", (unsigned int)-1))
	PRINTF(("the number is %.0x", (unsigned int)-1))
	PRINTF(("the number is %.x", (unsigned int)-1))
	PRINTF(("the number is %.5x", (unsigned int)-1))
	PRINTF(("the number is %.9x", (unsigned int)-1))
	PRINTF(("the number is %.10x", (unsigned int)-1))
	PRINTF(("the number is %.11x", (unsigned int)-1))
	PRINTF(("the number is %.11x", (unsigned int)-1))
	PRINTF(("%.0xis a big number", 0))
	PRINTF(("%.4xs a big number", 0))
	PRINTF(("the number is %.0x", 0))
	PRINTF(("the number is %.x", 0))
	PRINTF(("the number is %.5x", 0))
	PRINTF(("%x%.5x%.0x%.x%.9x", 5, 55, 2, 0, 42))
	PRINTF(("%xs%.5xi%.0xc%.xp%.9xu", 5, 55, 2, 0, 42))

	describe("\nTest the same tests above for %X");

	PRINTF(("%.1X", 0xa))
	PRINTF(("%.4X", 11))
	PRINTF(("%.0X", 0))
	PRINTF(("%.1X", -1))
	PRINTF(("%.10X", -1))
	PRINTF(("%.14X", -1))
	PRINTF(("%.8X", 0))
	PRINTF(("%.2X", 30000))
	PRINTF(("%.20X", 30000))
	PRINTF(("%.0X", (unsigned int)-1))
	PRINTF(("%.5X", (unsigned int)-1))
	PRINTF(("%.9X", (unsigned int)-1))
	PRINTF(("%.10X", (unsigned int)-1))
	PRINTF(("%.11X", (unsigned int)-1))
	PRINTF(("%.10Xis a big number", (unsigned int)-1))
	PRINTF(("%.0Xis a big number", (unsigned int)-1))
	PRINTF(("%.4Xs a big number", (unsigned int)-1))
	PRINTF(("%.9XXs a big number", (unsigned int)-1))
	PRINTF(("%.11Xps a big number", (unsigned int)-1))
	PRINTF(("the number is %.0X", (unsigned int)-1))
	PRINTF(("the number is %.X", (unsigned int)-1))
	PRINTF(("the number is %.5X", (unsigned int)-1))
	PRINTF(("the number is %.9X", (unsigned int)-1))
	PRINTF(("the number is %.10X", (unsigned int)-1))
	PRINTF(("the number is %.11X", (unsigned int)-1))
	PRINTF(("the number is %.11X", (unsigned int)-1))
	PRINTF(("%.0Xis a big number", 0))
	PRINTF(("%.4Xs a big number", 0))
	PRINTF(("the number is %.0X", 0))
	PRINTF(("the number is %.X", 0))
	PRINTF(("the number is %.5X", 0))
	PRINTF(("%X%.5X%.0X%.X%.9X", 5, 55, 2, 0, 42))
	PRINTF(("%Xs%.5Xi%.0Xc%.Xp%.9Xu", 5, 55, 2, 0, 42))

	describe("\nTest some simple precisions with %%");

	PRINTF(("%.3%"))
	PRINTF_EXPECTED(("%3.3%"), ("  %%"))
	PRINTF(("%.0%"))
	PRINTF(("you have 100%.10% chance of approval"))
	PRINTF(("%.9%%.5% is how you print %% in printf"))
	PRINTF(("%.9%i%.5%s%.%d%.3%p%.3%.6c%.6%u%.8%x"))

	describe("\n\n==> Flag testing");
	describe("\nTest %c with the '-' flag and variable widths");

	// this is literally a negative width '-'
	PRINTF(("%-c", 'p'));
	PRINTF(("%-1c", 'b'));
	PRINTF(("%-5c", 'w'));
	PRINTF((" kk daora%-5cblz", 'w'));
	PRINTF(("%-20carigatou", 'w'));
	PRINTF(("%-c%-c%-4c%-11c", 'a', 'b', 'c', 'd'));
	PRINTF(("%-ci%-cp%4cs%-11cx", 'a', 'b', 'c', 'd'));
	PRINTF(("%----ci%---cp%4cs%--11cx", 'a', 'b', 'c', 'd'));

	describe("\nTest %s with the '-' flag and variable widths and precisions");

	PRINTF(("%-9sScience!", "Aperture"));
	PRINTF(("We %-s what we %8s, %-2s we %-20s", "do", "must", "because", "can"));
	PRINTF(("%--4s %s %------------------9s of %s of %-5s", "for", "the", "goooood", "aaall", "us"));
	PRINTF(("%--4.1s %s %------------------9.3s of %s of %-5.7s", "for", "the", "goooood", "aaall", "us"));
	PRINTF(("%--.sp--.su kkkk", "pegadinha po"));
	PRINTF(("%-9sScience!", "-42"));

	describe("\nTest %p with the '-' flag and variable widths");

	PRINTF(("that's the way it %-20pis", ""));
	PRINTF(("as soon as %-10possible", (void *) -1));
	PRINTF(("as soon as %-16peasible", (void *) (((long int)3 << 42) + 15)));
	PRINTF(("as soon as %-16peasible", (void *) (((long int)3 << 42) + 15)));
	PRINTF(("thats %-psrobably not a good idea", (void *) 13));
	PRINTF(("%------21pwhoa wtf is that", (void *) 13));
	PRINTF(("%------21p yeah i'm %p running out %--p of ideas", (void *) 13, (void *) 65, (void *) -1));

	describe("\nTest %d with the '-' flag and variable widths and precisions");

	PRINTF(("%-d", 0));
	PRINTF(("%-d", 1));
	PRINTF(("%-d", 10));
	PRINTF(("%-d", -10));
	PRINTF(("%-d", 5000));
	PRINTF(("%-d", -5000));
	PRINTF(("%-d", (int)-2147483648));
	PRINTF(("%-d", 2147483647));
	PRINTF(("%-1d", 0));
	PRINTF(("%-1d", 1));
	PRINTF(("%-1d", 10));
	PRINTF(("%-1d", -10));
	PRINTF(("%-1d", 5000));
	PRINTF(("%-1d", -5000));
	PRINTF(("%-1d", (int)-2147483648));
	PRINTF(("%-1d", 2147483647));
	PRINTF(("%-10d", 0));
	PRINTF(("%-10d", 1));
	PRINTF(("%-10d", 10));
	PRINTF(("%-10d", -10));
	PRINTF(("%-10d", 5000));
	PRINTF(("%-10d", -5000));
	PRINTF(("%-10d", (int)-2147483648));
	PRINTF(("%-10d", 2147483647));
	PRINTF(("%-.d", 0));
	PRINTF(("%-.1d", 1));
	PRINTF(("%-.2d", 10));
	PRINTF(("%-.3d", -10));
	PRINTF(("%-.4d", 5000));
	PRINTF(("%-.5d", -5000));
	PRINTF(("%-.6d", (int)-2147483648));
	PRINTF(("%-.7d", 2147483647));
	PRINTF(("%-1.8d", 0));
	PRINTF(("%-1.9d", 1));
	PRINTF(("%-1.10d", 10));
	PRINTF(("%-1.0d", -10));
	PRINTF(("%-1.6d", 5000));
	PRINTF(("%-1.4d", -5000));
	PRINTF(("%-1.10d", (int)-2147483648));
	PRINTF(("%-1.12d", 2147483647));
	PRINTF(("%-10.d", 0));
	PRINTF(("%-10.10d", 1));
	PRINTF(("%-10.5d", 10));
	PRINTF(("%-10.2d", -10));
	PRINTF(("%-10.5d", 5000));
	PRINTF(("%-10.5d", -5000));
	PRINTF(("%-10.15d", (int)-2147483648));
	PRINTF(("%-10.5d", 2147483647));
	PRINTF(("%-15.d", 0));
	PRINTF(("%-15.10d", 1));
	PRINTF(("%-15.5d", 10));
	PRINTF(("%-15.2d", -10));
	PRINTF(("%-15.5d", 5000));
	PRINTF(("%-15.5d", -5000));
	PRINTF(("%-15.15d", (int)-2147483648));
	PRINTF(("%-15.5d", 2147483647));
	PRINTF(("%-4.5d%d%4d%-10d-d5%-.3d", 3, 4, 5, 6, 7));
	PRINTF(("%-4.5d%d%4d%-10d-d5%-.3d", 300000, 400000, 500000, 600000, 700000));
	PRINTF(("%-4.5d%d%4d%-10d-d5%-.3d", -300000, -400000, -500000, -600000, -700000));
	PRINTF(("%-4.5d%d%4d%-10d-d5%-.3d", 2147483647, 2141483647, 2141483647, 2141483647, 2141483647));
	PRINTF(("%-4.5d%d%4d%-10d-d5%-.3d", (int)-2147483648, (int)-2141483648, (int)-2141483648, (int)-2141483648, (int)-2141483648));

	describe("\nI probably shouldn't be copying all of the %d tests for %i");

	PRINTF(("%-i", 0));
	PRINTF(("%-i", 1));
	PRINTF(("%-i", 10));
	PRINTF(("%-i", -10));
	PRINTF(("%-i", 5000));
	PRINTF(("%-i", -5000));
	PRINTF(("%-i", (int)-2147483648));
	PRINTF(("%-i", 2147483647));
	PRINTF(("%-1i", 0));
	PRINTF(("%-1i", 1));
	PRINTF(("%-1i", 10));
	PRINTF(("%-1i", -10));
	PRINTF(("%-1i", 5000));
	PRINTF(("%-1i", -5000));
	PRINTF(("%-1i", (int)-2147483648));
	PRINTF(("%-1i", 2147483647));
	PRINTF(("%-10i", 0));
	PRINTF(("%-10i", 1));
	PRINTF(("%-10i", 10));
	PRINTF(("%-10i", -10));
	PRINTF(("%-10i", 5000));
	PRINTF(("%-10i", -5000));
	PRINTF(("%-10i", (int)-2147483648));
	PRINTF(("%-10i", 2147483647));
	PRINTF(("%-.i", 0));
	PRINTF(("%-.1i", 1));
	PRINTF(("%-.2i", 10));
	PRINTF(("%-.3i", -10));
	PRINTF(("%-.4i", 5000));
	PRINTF(("%-.5i", -5000));
	PRINTF(("%-.6i", (int)-2147483648));
	PRINTF(("%-.7i", 2147483647));
	PRINTF(("%-1.8i", 0));
	PRINTF(("%-1.9i", 1));
	PRINTF(("%-1.10i", 10));
	PRINTF(("%-1.0i", -10));
	PRINTF(("%-1.6i", 5000));
	PRINTF(("%-1.4i", -5000));
	PRINTF(("%-1.10i", (int)-2147483648));
	PRINTF(("%-1.12i", 2147483647));
	PRINTF(("%-10.i", 0));
	PRINTF(("%-10.10i", 1));
	PRINTF(("%-10.5i", 10));
	PRINTF(("%-10.2i", -10));
	PRINTF(("%-10.5i", 5000));
	PRINTF(("%-10.5i", -5000));
	PRINTF(("%-10.15i", (int)-2147483648));
	PRINTF(("%-10.5i", 2147483647));
	PRINTF(("%-15.i", 0));
	PRINTF(("%-15.10i", 1));
	PRINTF(("%-15.5i", 10));
	PRINTF(("%-15.2i", -10));
	PRINTF(("%-15.5i", 5000));
	PRINTF(("%-15.5i", -5000));
	PRINTF(("%-15.15i", (int)-2147483648));
	PRINTF(("%-15.5i", 2147483647));
	PRINTF(("%-4.5i%i%4i%-10i-i5%-.3i", 3, 4, 5, 6, 7));
	PRINTF(("%-4.5i%i%4i%-10i-i5%-.3i", 300000, 400000, 500000, 600000, 700000));
	PRINTF(("%-4.5i%i%4i%-10i-i5%-.3i", -300000, -400000, -500000, -600000, -700000));
	PRINTF(("%-4.5i%i%4i%-10i-i5%-.3i", 2147483647, 2141483647, 2141483647, 2141483647, 2141483647));
	PRINTF(("%-4.5i%i%4i%-10i-i5%-.3i", (int)-2147483648, (int)-2141483648, (int)-2141483648, (int)-2141483648, (int)-2141483648));

	describe("\nMaybe it's worth it for the %u");

	PRINTF(("%-u", 0));
	PRINTF(("%-u", 1));
	PRINTF(("%-u", 10));
	PRINTF(("%-u", -10));
	PRINTF(("%-u", 5000));
	PRINTF(("%-u", -5000));
	PRINTF(("%-u", (unsigned int)-1));
	PRINTF(("%-u", 2147483647));
	PRINTF(("%-1u", 0));
	PRINTF(("%-1u", 1));
	PRINTF(("%-1u", 10));
	PRINTF(("%-1u", -10));
	PRINTF(("%-1u", 5000));
	PRINTF(("%-1u", -5000));
	PRINTF(("%-1u", (unsigned int)-1));
	PRINTF(("%-1u", 2147483647));
	PRINTF(("%-10u", 0));
	PRINTF(("%-10u", 1));
	PRINTF(("%-10u", 10));
	PRINTF(("%-10u", -10));
	PRINTF(("%-10u", 5000));
	PRINTF(("%-10u", -5000));
	PRINTF(("%-10u", -1));
	PRINTF(("%-10u", 2147483647));
	PRINTF(("%-.u", 0));
	PRINTF(("%-.1u", 1));
	PRINTF(("%-.2u", 10));
	PRINTF(("%-.3u", -10));
	PRINTF(("%-.4u", 5000));
	PRINTF(("%-.5u", -5000));
	PRINTF(("%-.6u", -1));
	PRINTF(("%-.7u", 2147483647));
	PRINTF(("%-1.8u", 0));
	PRINTF(("%-1.9u", 1));
	PRINTF(("%-1.10u", 10));
	PRINTF(("%-1.0u", -10));
	PRINTF(("%-1.6u", 5000));
	PRINTF(("%-1.4u", -5000));
	PRINTF(("%-1.10u", -1));
	PRINTF(("%-1.12u", 2147483647));
	PRINTF(("%-10.u", 0));
	PRINTF(("%-10.10u", 1));
	PRINTF(("%-10.5u", 10));
	PRINTF(("%-10.2u", -10));
	PRINTF(("%-10.5u", 5000));
	PRINTF(("%-10.5u", -5000));
	PRINTF(("%-10.15u", -1));
	PRINTF(("%-10.5u", 2147483647));
	PRINTF(("%-15.u", 0));
	PRINTF(("%-15.10u", 1));
	PRINTF(("%-15.5u", 10));
	PRINTF(("%-15.2u", -10));
	PRINTF(("%-15.5u", 5000));
	PRINTF(("%-15.5u", -5000));
	PRINTF(("%-15.15u", -1));
	PRINTF(("%-15.5u", 2147483647));
	PRINTF(("%-4.5u%u%4u%-10u-u5%-.3u", 3, 4, 5, 6, 7));
	PRINTF(("%-4.5u%u%4u%-10u-u5%-.3u", 300000, 400000, 500000, 600000, 700000));
	PRINTF(("%-4.5u%u%4u%-10u-u5%-.3u", -300000, -400000, -500000, -600000, -700000));
	PRINTF(("%-4.5u%u%4u%-10u-u5%-.3u", 2147483647, 2141483647, 2141483647, 2141483647, 2141483647));
	PRINTF(("%-4.5u%u%4u%-10u-u5%-.3u", -1, -1, -1, -1, -1));

	describe("\nAnd for %x");

	PRINTF(("%-x", 0));
	PRINTF(("%-x", 1));
	PRINTF(("%-x", 10));
	PRINTF(("%-x", -10));
	PRINTF(("%-x", 5000));
	PRINTF(("%-x", -5000));
	PRINTF(("%-x", -1));
	PRINTF(("%-x", 2147483647));
	PRINTF(("%-1x", 0));
	PRINTF(("%-1x", 1));
	PRINTF(("%-1x", 10));
	PRINTF(("%-1x", -10));
	PRINTF(("%-1x", 5000));
	PRINTF(("%-1x", -5000));
	PRINTF(("%-1x", -1));
	PRINTF(("%-1x", 2147483647));
	PRINTF(("%-10x", 0));
	PRINTF(("%-10x", 1));
	PRINTF(("%-10x", 10));
	PRINTF(("%-10x", -10));
	PRINTF(("%-10x", 5000));
	PRINTF(("%-10x", -5000));
	PRINTF(("%-10x", -1));
	PRINTF(("%-10x", 2147483647));
	PRINTF(("%-.x", 0));
	PRINTF(("%-.1x", 1));
	PRINTF(("%-.2x", 10));
	PRINTF(("%-.3x", -10));
	PRINTF(("%-.4x", 5000));
	PRINTF(("%-.5x", -5000));
	PRINTF(("%-.6x", -1));
	PRINTF(("%-.7x", 2147483647));
	PRINTF(("%-1.8x", 0));
	PRINTF(("%-1.9x", 1));
	PRINTF(("%-1.10x", 10));
	PRINTF(("%-1.0x", -10));
	PRINTF(("%-1.6x", 5000));
	PRINTF(("%-1.4x", -5000));
	PRINTF(("%-1.10x", -1));
	PRINTF(("%-1.12x", 2147483647));
	PRINTF(("%-10.x", 0));
	PRINTF(("%-10.10x", 1));
	PRINTF(("%-10.5x", 10));
	PRINTF(("%-10.2x", -10));
	PRINTF(("%-10.5x", 5000));
	PRINTF(("%-10.5x", -5000));
	PRINTF(("%-10.15x", -1));
	PRINTF(("%-10.5x", 2147483647));
	PRINTF(("%-15.x", 0));
	PRINTF(("%-15.10x", 1));
	PRINTF(("%-15.5x", 10));
	PRINTF(("%-15.2x", -10));
	PRINTF(("%-15.5x", 5000));
	PRINTF(("%-15.5x", -5000));
	PRINTF(("%-15.15x", -1));
	PRINTF(("%-15.5x", 2147483647));
	PRINTF(("%-4.5x%x%4x%-10x-x5%-.3x", 3, 4, 5, 6, 7));
	PRINTF(("%-4.5x%x%4x%-10x-x5%-.3x", 300000, 400000, 500000, 600000, 700000));
	PRINTF(("%-4.5x%x%4x%-10x-x5%-.3x", -300000, -400000, -500000, -600000, -700000));
	PRINTF(("%-4.5x%x%4x%-10x-x5%-.3x", 2147483647, 2141483647, 2141483647, 2141483647, 2141483647));
	PRINTF(("%-4.5x%x%4x%-10x-x5%-.3x", -1, -1, -1, -1, -1));

	describe("\nAnd for %X");

	PRINTF(("%-X", 0));
	PRINTF(("%-X", 1));
	PRINTF(("%-X", 10));
	PRINTF(("%-X", -10));
	PRINTF(("%-X", 5000));
	PRINTF(("%-X", -5000));
	PRINTF(("%-X", -1));
	PRINTF(("%-X", 2147483647));
	PRINTF(("%-1X", 0));
	PRINTF(("%-1X", 1));
	PRINTF(("%-1X", 10));
	PRINTF(("%-1X", -10));
	PRINTF(("%-1X", 5000));
	PRINTF(("%-1X", -5000));
	PRINTF(("%-1X", -1));
	PRINTF(("%-1X", 2147483647));
	PRINTF(("%-10X", 0));
	PRINTF(("%-10X", 1));
	PRINTF(("%-10X", 10));
	PRINTF(("%-10X", -10));
	PRINTF(("%-10X", 5000));
	PRINTF(("%-10X", -5000));
	PRINTF(("%-10X", -1));
	PRINTF(("%-10X", 2147483647));
	PRINTF(("%-.X", 0));
	PRINTF(("%-.1X", 1));
	PRINTF(("%-.2X", 10));
	PRINTF(("%-.3X", -10));
	PRINTF(("%-.4X", 5000));
	PRINTF(("%-.5X", -5000));
	PRINTF(("%-.6X", -1));
	PRINTF(("%-.7X", 2147483647));
	PRINTF(("%-1.8X", 0));
	PRINTF(("%-1.9X", 1));
	PRINTF(("%-1.10X", 10));
	PRINTF(("%-1.0X", -10));
	PRINTF(("%-1.6X", 5000));
	PRINTF(("%-1.4X", -5000));
	PRINTF(("%-1.10X", -1));
	PRINTF(("%-1.12X", 2147483647));
	PRINTF(("%-10.X", 0));
	PRINTF(("%-10.10X", 1));
	PRINTF(("%-10.5X", 10));
	PRINTF(("%-10.2X", -10));
	PRINTF(("%-10.5X", 5000));
	PRINTF(("%-10.5X", -5000));
	PRINTF(("%-10.15X", -1));
	PRINTF(("%-10.5X", 2147483647));
	PRINTF(("%-15.X", 0));
	PRINTF(("%-15.10X", 1));
	PRINTF(("%-15.5X", 10));
	PRINTF(("%-15.2X", -10));
	PRINTF(("%-15.5X", 5000));
	PRINTF(("%-15.5X", -5000));
	PRINTF(("%-15.15X", -1));
	PRINTF(("%-15.5X", 2147483647));
	PRINTF(("%-4.5X%X%4X%-10X-X5%-.3X", 3, 4, 5, 6, 7));
	PRINTF(("%-4.5X%X%4X%-10X-X5%-.3X", 300000, 400000, 500000, 600000, 700000));
	PRINTF(("%-4.5X%X%4X%-10X-X5%-.3X", -300000, -400000, -500000, -600000, -700000));
	PRINTF(("%-4.5X%X%4X%-10X-X5%-.3X", 2147483647, 2141483647, 2141483647, 2141483647, 2141483647));
	PRINTF(("%-4.5X%X%4X%-10X-X5%-.3X", -1, -1, -1, -1, -1));

	describe("\nTest %% with the flag - with some widths and precisions");

	PRINTF(("%--.3%"))
	PRINTF(("---%.0%"))
	PRINTF(("you have 100%-.10% chance of approval"))
	PRINTF(("%---.9%%---.5% is how you print %% in printf"))
	PRINTF_EXPECTED(("%-3.3%"),                                           /* expected: */ ("%%  "))
	PRINTF_EXPECTED(("%----.9%i%.5%s%-10.%d%.3%p%.3%----.6c%.6%u%-.8%x"), /* expected: */ ("%%i%%s%%         d%%p%%----.6c%%u%%x"))

	describe("\nTest %d with the 0 flag");

	PRINTF(("%01d", 0));
	PRINTF(("%01d", -4));
	PRINTF(("%010d", 42));
	PRINTF(("%042d", 42000));
	PRINTF(("%020d", -42000));
	PRINTF(("wait for it... %050d", 42));
	PRINTF(("%020d is how many tests are going to be made", 8000));
	PRINTF(("%05d", 2147483647));
	PRINTF(("%030d", 2147483647));
	PRINTF(("%010d", 2147483647));
	PRINTF(("%05d", (int)-2147483648));
	PRINTF(("%030d", (int)-2147483648));
	PRINTF(("%010d", (int)-2147483648));
	PRINTF(("%011d", (int)-2147483648));
	PRINTF(("%012d", (int)-2147483648));
	PRINTF(("%012d, %20d, %2d, %42d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%012d, %d, %2d, %42d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%014d%020d%02d%0d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%014dc%020ds%02dx%0du", (int)-2147483648, 3, 30, -1));
	PRINTF(("%01.d", 0));
	PRINTF(("%01.0d", 0));
	PRINTF(("%02.0d", 0));
	PRINTF(("%03.0d", 0));
	PRINTF(("%01.1d", 0));
	PRINTF(("%01.2d", 0));
	PRINTF(("%01.3d", 0));
	PRINTF(("%01.0d", -4));
	PRINTF(("%01.1d", -4));
	PRINTF(("%01.2d", -4));
	PRINTF(("%01.3d", -4));
	PRINTF(("%01.0d", 4));
	PRINTF(("%01.1d", 4));
	PRINTF(("%01.2d", 4));
	PRINTF(("%01.3d", 4));
	PRINTF(("%010.20d", 42));
	PRINTF(("%042.2d", 42000));
	PRINTF(("%042.20d", 42000));
	PRINTF(("%042.42d", 42000));
	PRINTF(("%042.52d", 42000));
	PRINTF(("%020.10d", -42000));
	PRINTF(("%020.20d", -42000));
	PRINTF(("%020.30d", -42000));
	PRINTF(("wait for it... %050.50d", 42));
	PRINTF(("%020.19d is how many tests are going to be made", 8000));
	PRINTF(("%020.20d is how many tests are going to be made", 8000));
	PRINTF(("%020.21d is how many tests are going to be made", 8000));
	PRINTF(("%05d", 2147483647));
	PRINTF(("%030d", 2147483647));
	PRINTF(("%09d", 2147483647));
	PRINTF(("%010d", 2147483647));
	PRINTF(("%011d", 2147483647));
	PRINTF(("%05d", (int)-2147483648));
	PRINTF(("%030d", (int)-2147483648));
	PRINTF(("%010d", (int)-2147483648));
	PRINTF(("%011d", (int)-2147483648));
	PRINTF(("%012d", (int)-2147483648));
	PRINTF(("%012d, %20d, %2d, %000042d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%012d, %d, %002d, %42d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%0014.2d%020d%0002.d%000.5d", (int)-2147483648, 3, 30, -1));
	PRINTF(("%014dc%020ds%02dx%0du", (int)-2147483648, 3, 30, -1));

	describe("\nTest %i with the 0 flag");

	PRINTF(("%01i", 0));
	PRINTF(("%01i", -4));
	PRINTF(("%010i", 42));
	PRINTF(("%042i", 42000));
	PRINTF(("%020i", -42000));
	PRINTF(("wait for it... %050i", 42));
	PRINTF(("%020i is how many tests are going to be maie", 8000));
	PRINTF(("%05i", 2147483647));
	PRINTF(("%030i", 2147483647));
	PRINTF(("%010i", 2147483647));
	PRINTF(("%05i", (int)-2147483648));
	PRINTF(("%030i", (int)-2147483648));
	PRINTF(("%010i", (int)-2147483648));
	PRINTF(("%011i", (int)-2147483648));
	PRINTF(("%012i", (int)-2147483648));
	PRINTF(("%012i, %20i, %2i, %42i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%012i, %i, %2i, %42i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%014i%020i%02i%0i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%014ic%020is%02ix%0iu", (int)-2147483648, 3, 30, -1));
	PRINTF(("%01.i", 0));
	PRINTF(("%01.0i", 0));
	PRINTF(("%02.0i", 0));
	PRINTF(("%03.0i", 0));
	PRINTF(("%01.1i", 0));
	PRINTF(("%01.2i", 0));
	PRINTF(("%01.3i", 0));
	PRINTF(("%01.0i", -4));
	PRINTF(("%01.1i", -4));
	PRINTF(("%01.2i", -4));
	PRINTF(("%01.3i", -4));
	PRINTF(("%01.0i", 4));
	PRINTF(("%01.1i", 4));
	PRINTF(("%01.2i", 4));
	PRINTF(("%01.3i", 4));
	PRINTF(("%010.20i", 42));
	PRINTF(("%042.2i", 42000));
	PRINTF(("%042.20i", 42000));
	PRINTF(("%042.42i", 42000));
	PRINTF(("%042.52i", 42000));
	PRINTF(("%020.10i", -42000));
	PRINTF(("%020.20i", -42000));
	PRINTF(("%020.30i", -42000));
	PRINTF(("wait for it... %050.50i", 42));
	PRINTF(("%020.19i is how many tests are going to be made", 8000));
	PRINTF(("%020.20i is how many tests are going to be made", 8000));
	PRINTF(("%020.21i is how many tests are going to be made", 8000));
	PRINTF(("%05i", 2147483647));
	PRINTF(("%030i", 2147483647));
	PRINTF(("%09i", 2147483647));
	PRINTF(("%010i", 2147483647));
	PRINTF(("%011i", 2147483647));
	PRINTF(("%05i", (int)-2147483648));
	PRINTF(("%030i", (int)-2147483648));
	PRINTF(("%010i", (int)-2147483648));
	PRINTF(("%011i", (int)-2147483648));
	PRINTF(("%012i", (int)-2147483648));
	PRINTF(("%012i, %20i, %2i, %000042i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%012i, %i, %002i, %42i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%0014.2i%020i%0002.i%000.5i", (int)-2147483648, 3, 30, -1));
	PRINTF(("%014ic%020is%02ix%0iu", (int)-2147483648, 3, 30, -1));

	describe("\nTest %u with the 0 flag");

	PRINTF(("%01u", 0));
	PRINTF(("%01u", -4));
	PRINTF(("%010u", 42));
	PRINTF(("%042u", 42000));
	PRINTF(("%020u", -42000));
	PRINTF(("wait for it... %050u", 42));
	PRINTF(("%020u is how many tests are going to be maie", 8000));
	PRINTF(("%05u", 2147483647));
	PRINTF(("%030u", 2147483647));
	PRINTF(("%010u", 2147483647));
	PRINTF(("%05u", -1));
	PRINTF(("%030u", -1));
	PRINTF(("%010u", -1));
	PRINTF(("%011u", -1));
	PRINTF(("%012u", -1));
	PRINTF(("%012u, %20u, %2u, %42u", -1, 3, 30, -1));
	PRINTF(("%012u, %u, %2u, %42u", -1, 3, 30, -1));
	PRINTF(("%014u%020u%02u%0u", -1, 3, 30, -1));
	PRINTF(("%014uc%020us%02ux%0ui", -1, 3, 30, -1));
	PRINTF(("%01.u", 0));
	PRINTF(("%01.0u", 0));
	PRINTF(("%02.0u", 0));
	PRINTF(("%03.0u", 0));
	PRINTF(("%01.1u", 0));
	PRINTF(("%01.2u", 0));
	PRINTF(("%01.3u", 0));
	PRINTF(("%01.0u", 4));
	PRINTF(("%01.1u", 4));
	PRINTF(("%01.2u", 4));
	PRINTF(("%01.3u", 4));
	PRINTF(("%010.20u", 42));
	PRINTF(("%042.2u", 42000));
	PRINTF(("%042.20u", 42000));
	PRINTF(("%042.42u", 42000));
	PRINTF(("%042.52u", 42000));
	PRINTF(("wait for it... %050.50u", 42));
	PRINTF(("%020.19u is how many tests are going to be made", 8000));
	PRINTF(("%020.20u is how many tests are going to be made", 8000));
	PRINTF(("%020.21u is how many tests are going to be made", 8000));
	PRINTF(("%05u", 2147483647));
	PRINTF(("%030u", 2147483647));
	PRINTF(("%09u", 2147483647));
	PRINTF(("%010u", 2147483647));
	PRINTF(("%011u", 2147483647));
	PRINTF(("%05u",  -1));
	PRINTF(("%030u", -1));
	PRINTF(("%010u", -1));
	PRINTF(("%011u", -1));
	PRINTF(("%012u", -1));
	PRINTF(("%012u, %20u, %2u, %000042u", -1, 3, 30, -1));
	PRINTF(("%012u, %u, %002u, %42u", -1, 3, 30, -1));
	PRINTF(("%0014.2u%020u%0002.u%000.5u", -1, 3, 30, -1));
	PRINTF(("%014uc%020us%02ux%0ui", -1, 3, 30, -1));

	describe("\nTest %x with the 0 flag");

	PRINTF(("%01x", 0));
	PRINTF(("%01x", -4));
	PRINTF(("%010x", 42));
	PRINTF(("%042x", 42000));
	PRINTF(("%020x", -42000));
	PRINTF(("wait for it... %050x", 42));
	PRINTF(("%020x is how many tests are going to be made", 8000));
	PRINTF(("%05x", 2147483647));
	PRINTF(("%030x", 2147483647));
	PRINTF(("%010x", 2147483647));
	PRINTF(("%05x", -1));
	PRINTF(("%030x", -1));
	PRINTF(("%010x", -1));
	PRINTF(("%011x", -1));
	PRINTF(("%012x", -1));
	PRINTF(("%012x, %20x, %2x, %42x", -1, 3, 30, -1));
	PRINTF(("%012x, %x, %2x, %42x", -1, 3, 30, -1));
	PRINTF(("%014x%020x%02x%0x", -1, 3, 30, -1));
	PRINTF(("%014xc%020xs%02xX%0xi", -1, 3, 30, -1));
	PRINTF(("%01.x", 0));
	PRINTF(("%01.0x", 0));
	PRINTF(("%02.0x", 0));
	PRINTF(("%03.0x", 0));
	PRINTF(("%01.1x", 0));
	PRINTF(("%01.2x", 0));
	PRINTF(("%01.3x", 0));
	PRINTF(("%01.0x", 4));
	PRINTF(("%01.1x", 4));
	PRINTF(("%01.2x", 4));
	PRINTF(("%01.3x", 4));
	PRINTF(("%010.20x", 42));
	PRINTF(("%042.2x", 42000));
	PRINTF(("%042.20x", 42000));
	PRINTF(("%042.42x", 42000));
	PRINTF(("%042.52x", 42000));
	PRINTF(("wait for it... %050.50x", 42));
	PRINTF(("%020.19x is how many tests are going to be made", 8000));
	PRINTF(("%020.20x is how many tests are going to be made", 8000));
	PRINTF(("%020.21x is how many tests are going to be made", 8000));
	PRINTF(("%05x", 2147483647));
	PRINTF(("%030x", 2147483647));
	PRINTF(("%09x", 2147483647));
	PRINTF(("%010x", 2147483647));
	PRINTF(("%011x", 2147483647));
	PRINTF(("%05x",  -1));
	PRINTF(("%030x", -1));
	PRINTF(("%010x", -1));
	PRINTF(("%011x", -1));
	PRINTF(("%012x", -1));
	PRINTF(("%012x, %20x, %2x, %000042x", -1, 3, 30, -1));
	PRINTF(("%012x, %x, %002x, %42x", -1, 3, 30, -1));
	PRINTF(("%0014.2x%020x%0002.x%000.5x", -1, 3, 30, -1));
	PRINTF(("%014xc%020xs%02xx%0xi", -1, 3, 30, -1));

	describe("\nTest %X with the 0 flag");

	PRINTF(("%01X", 0));
	PRINTF(("%01X", -4));
	PRINTF(("%010X", 42));
	PRINTF(("%042X", 42000));
	PRINTF(("%020X", -42000));
	PRINTF(("wait for it... %050X", 42));
	PRINTF(("%020X is how many tests are going to be made", 8000));
	PRINTF(("%05X", 2147483647));
	PRINTF(("%030X", 2147483647));
	PRINTF(("%010X", 2147483647));
	PRINTF(("%05X", -1));
	PRINTF(("%030X", -1));
	PRINTF(("%010X", -1));
	PRINTF(("%011X", -1));
	PRINTF(("%012X", -1));
	PRINTF(("%012X, %20X, %2X, %42X", -1, 3, 30, -1));
	PRINTF(("%012X, %X, %2X, %42X", -1, 3, 30, -1));
	PRINTF(("%014X%020X%02X%0X", -1, 3, 30, -1));
	PRINTF(("%014Xc%020Xs%02XX%0Xi", -1, 3, 30, -1));
	PRINTF(("%01.X", 0));
	PRINTF(("%01.0X", 0));
	PRINTF(("%02.0X", 0));
	PRINTF(("%03.0X", 0));
	PRINTF(("%01.1X", 0));
	PRINTF(("%01.2X", 0));
	PRINTF(("%01.3X", 0));
	PRINTF(("%01.0X", 4));
	PRINTF(("%01.1X", 4));
	PRINTF(("%01.2X", 4));
	PRINTF(("%01.3X", 4));
	PRINTF(("%010.20X", 42));
	PRINTF(("%042.2X", 42000));
	PRINTF(("%042.20X", 42000));
	PRINTF(("%042.42X", 42000));
	PRINTF(("%042.52X", 42000));
	PRINTF(("wait for it... %050.50X", 42));
	PRINTF(("%020.19X is how many tests are going to be made", 8000));
	PRINTF(("%020.20X is how many tests are going to be made", 8000));
	PRINTF(("%020.21X is how many tests are going to be made", 8000));
	PRINTF(("%05X", 2147483647));
	PRINTF(("%030X", 2147483647));
	PRINTF(("%09X", 2147483647));
	PRINTF(("%010X", 2147483647));
	PRINTF(("%011X", 2147483647));
	PRINTF(("%05X",  -1));
	PRINTF(("%030X", -1));
	PRINTF(("%010X", -1));
	PRINTF(("%011X", -1));
	PRINTF(("%012X", -1));
	PRINTF(("%012X, %20X, %2X, %000042X", -1, 3, 30, -1));
	PRINTF(("%012X, %X, %002X, %42X", -1, 3, 30, -1));
	PRINTF(("%0014.2X%020X%0002.X%000.5X", -1, 3, 30, -1));
	PRINTF(("%014Xc%020Xs%02XX%0Xi", -1, 3, 30, -1));

	describe("\nTest %% with the 0 flag");

	PRINTF(("%05%"));

	describe("\nTest %c with some * widths");

	PRINTF(("%*c", 0, 'a'));
	PRINTF(("%*c", 1, 'a'));
	PRINTF(("%*c", 4, 'a'));
	PRINTF(("%*c%c", 4, 'a', 'b'));
	PRINTF(("%*c*%c", 8, 'a', 'b'));
	PRINTF(("%*c*%*c", 8, 'a', 2, 'b'));
	PRINTF(("%*c*%*c", -8, 'a', 2, 'b'));
	PRINTF(("%*c*%*c", -8, 'a', -2, 'b'));
	PRINTF(("%*c*%*c*", -8, 'a', -2, 'b'));
	PRINTF(("%*c*%*c*", 8, 'a', -1, 'b'));
	PRINTF(("%*c*%*c*", -1, 'a', 1, 'b'));
	PRINTF(("%-*c*%-*c*", 3, 'a', -3, 'b'));

	describe("\nTest %s with some * widths");

	PRINTF(("%*s", 1, "a"));
	PRINTF(("%*s", 2, "a"));
	PRINTF(("%*s", 3, "a"));
	PRINTF(("%*s", -1, "a"));
	PRINTF(("%*s", -2, "a"));
	PRINTF(("%*s", -3, "a"));
	PRINTF(("%-*s%s", 3, "a", "bc"));
	PRINTF(("%-*s%-s", -8, "a", "bc"));
	PRINTF(("%*s%-*s*", -8, "a", 8, "bc"));
	PRINTF(("%*s%-*s*", -8, "a", -8, "bc"));

	describe("\nTest %p with some * widths");

	char *str = "test";
	PRINTF(("%*p", 1, str));
	PRINTF(("%*p", 2, str));
	PRINTF(("%*p", 12, str));
	PRINTF(("%*p", 13, str));
	PRINTF(("%*p", 14, str));
	PRINTF(("%*p", 15, str));
	PRINTF(("%*p", 16, str));
	PRINTF(("%*p", 17, str));
	PRINTF(("%*p", 18, str));
	PRINTF_EXPECTED(("%*p", 0, NULL),   /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%*p", 2, NULL),   /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%*p", 3, NULL),   /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%*p", 4, NULL),   /* expected: */ (" 0x0"));
	PRINTF_EXPECTED(("%*p", 5, NULL),   /* expected: */ ("  0x0"));
	PRINTF_EXPECTED(("%*p", -2, NULL),  /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%*p", -3, NULL),  /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%*p", -4, NULL),  /* expected: */ ("0x0 "));
	PRINTF_EXPECTED(("%*p", -5, NULL),  /* expected: */ ("0x0  "));
	PRINTF_EXPECTED(("%-*p", 0, NULL),  /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%-*p", 2, NULL),  /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%-*p", 3, NULL),  /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%-*p", 4, NULL),  /* expected: */ ("0x0 "));
	PRINTF_EXPECTED(("%-*p", 5, NULL),  /* expected: */ ("0x0  "));
	PRINTF_EXPECTED(("%-*p", -2, NULL), /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%-*p", -3, NULL), /* expected: */ ("0x0"));
	PRINTF_EXPECTED(("%-*p", -4, NULL), /* expected: */ ("0x0 "));
	PRINTF_EXPECTED(("%-*p", -5, NULL), /* expected: */ ("0x0  "));
	PRINTF(("%*p", -1, str));
	PRINTF(("%*p", -2, str));
	PRINTF(("%*p", -12, str));
	PRINTF(("%*p", -13, str));
	PRINTF(("%*p", -14, str));
	PRINTF(("%*p", -15, str));
	PRINTF(("%*p", -16, str));
	PRINTF(("%*p", -17, str));
	PRINTF(("%*p", -18, str));

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
