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
	int error = 0;
	char *line;
	int result = get_next_line(user_stderr, &line);
	free(line);
	if (result == 0)
		return (0);
	result = get_next_line(user_stderr, &line); // get rid of the first line
	if (tester_strnstr(line, "heap-buffer-overflow", strlen(line)))
	{
		error = ERRORS_BUFFER_OVERFLOW;
	}
	free(line);
	result = get_next_line(user_stderr, &line); // get rid of the first line
	if (tester_strnstr(line, "leaks", strlen(line)))
	{
		error = 1;
	}
	free(line);
	// get rid of the rest of the file
	char dummy_buffer[32];
	while (read(user_stderr, dummy_buffer, 32));
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
		else if (errors && errors != ERRORS_LEAK)
			tester_putstr(BOLD "CRASH!" RESET RED "- check files/user_stderr.txt");
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
	PRINTF(("1, 2, 3, test, testing, sound, 1, 2, 3, sound, test"));

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

	PRINTF(("t"));

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
	PRINTF(("This %p is an address from the heap", mallocked); free(mallocked););
	free(mallocked);

	PRINTF_EXPECTED(("%p", NULL),
			("0x0"));

	PRINTF_EXPECTED(("The NULL macro represents the %p address", NULL),
			("The NULL macro represents the 0x0 address"));

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

	PRINTF_EXPECTED(("the address is %1p", (void *)0),
			("the address is 0x0"));

	PRINTF_EXPECTED(("the address is %2p", (void *)0),
			("the address is 0x0"));

	PRINTF_EXPECTED(("the address is %3p", (void *)0),
			("the address is 0x0"));

	PRINTF_EXPECTED(("the address is %4p", (void *)0),
			("the address is  0x0"));

	PRINTF_EXPECTED(("the address is %8p", (void *)0),
			("the address is      0x0"));

	PRINTF(("%12p is the address", (void *)0x7ffe6b8e60c7));

	PRINTF(("%13p is the address", (void *)0x7ffe6b8e60c8));

	PRINTF(("%14p is the address", (void *)0x7ffe6b8e60c9));

	PRINTF_EXPECTED(("%1p is the address", (void *)0),
			("0x0 is the address"));

	PRINTF_EXPECTED(("%2p is the address", (void *)0),
			("0x0 is the address"));

	PRINTF_EXPECTED(("%3p is the address", (void *)0),
			("0x0 is the address"));

	PRINTF_EXPECTED(("%4p is the address", (void *)0),
			(" 0x0 is the address"));

	PRINTF_EXPECTED(("%8p is the address", (void *)0),
			("     0x0 is the address"));

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

	PRINTF_EXPECTED(("%5%"),
			("    %%"));

	PRINTF_EXPECTED(("%2%"),
			(" %%"));

	PRINTF_EXPECTED(("the world is 2%10% more big today"),
			("the world is 2         %% more big today"));

	PRINTF_EXPECTED(("%2%%1%%%%5%%9%"),
			(" %%%%%%    %%        %%"));

	PRINTF_EXPECTED(("%2%s%1%id%%10%5%%9%"),
			(" %%s%%id%%10    %%        %%"));

	PRINTF_EXPECTED(("hey, what's %5% up%%?"),
			("hey, what's     %% up%%?"));

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

	PRINTF_EXPECTED(("%3.3%"),
			("  %%"))

	PRINTF(("%.0%"))

	/* 387 */PRINTF(("you have 100%.10% chance of approval"))

	/* 388 */PRINTF(("%.9%%.5% is how you print %% in printf"))

	PRINTF(("%.9%i%.5%s%.%d%.3%p%.3%.6c%.6%u%.8%x"))

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
