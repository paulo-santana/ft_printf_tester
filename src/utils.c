#include "get_next_line.h"
#include "libftprintf.h"
#include "../libtest/libtest.h"
#include "ft_printf_tester.h"
#include "helpers.h"

extern int g_current_test;
extern int test_nbr;
extern int passed_tests;

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
	tester_putnbr(g_current_test);
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

int check_result(t_result user_result, t_result orig_result, char *params_used)
{
	if (g_current_test == test_nbr || test_nbr == 0)
	{
		char *result;
		char *expected;
		int success = 1;
		int errors = 0;
		int wrong_return = 0;

		errors = check_errors(params_used);
		expected = orig_result.output_str;
		result = user_result.output_str;
		if (!errors || errors == ERRORS_LEAK)
		{
			success = test_string(expected, result, orig_result.bytes_read);
			wrong_return = user_result.return_value != orig_result.return_value;
		}

		if (success && !wrong_return && !errors)
			tester_putstr(GREEN);
		else
			tester_putstr(BOLD RED "\n  ");
		tester_putnbr(g_current_test);
		tester_putchar('.');

		if (success && !wrong_return && (!errors || errors == ERRORS_LEAK))
			tester_putstr(BOLD "OK" RESET);
		else if (errors) {
			if (errors == ERRORS_SIGSEGV)
				tester_putstr(BOLD "SIGSEGV!" RESET RED " - check files/user_stderr.txt");
		}
		else
			tester_putstr("KO");

		if (!success)
			tester_putstr(" (Wrong output)");
		if (wrong_return)
			tester_putstr(" (Wrong return)");
		if (errors == ERRORS_LEAK)
		{
			tester_putstr(RED " (LEAKS!)");
		}
		if (!success)
		{
			tester_putstr("\n");
			print_string_diff(expected, result, orig_result.bytes_read, user_result.bytes_read);
		}
		else
			tester_putchar(' ');
		if (!success || errors || wrong_return)
			print_help(params_used);
		else
			passed_tests++;
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

void open_pipes(int *p1, int *p2)
{
	if (pipe(p1) < 0)
	{
		tester_putstr("error opening stdout_pipe\n");
		exit(26);
	}
	if (pipe(p2) < 0)
	{
		tester_putstr("error opening rtrn_pipe\n");
		exit(24);
	}
}

void prepare_test(char *err_file, int *outpipe, int *retpipe) {
	/* the child don't need to read anything */
	close(outpipe[READ]);
	close(retpipe[READ]); 
	/* int file = open("files/original_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); */
	int err = open(err_file, O_CREAT | O_WRONLY | O_TRUNC, 0644); /* get rid of real printf errors*/
	dup2(outpipe[WRITE], 1);
	dup2(err, 2);
}

void finish_test(int result, int *outpipe, int *retpipe)
{
	write(retpipe[WRITE], &result, sizeof(int));
	close(outpipe[WRITE]);
	close(retpipe[WRITE]); 
	exit (0);

}

extern int g_function_return;

void fetch_result(t_result *result, char *output_buffer, int *stdout_pipe, int *rtrn_pipe)
{
	close(stdout_pipe[WRITE]);
	close(rtrn_pipe[WRITE]);
	int bytes_read = read(stdout_pipe[READ], output_buffer, BUFSIZE);
	read(rtrn_pipe[READ], &g_function_return, sizeof(int));
	close(stdout_pipe[READ]);
	close(rtrn_pipe[READ]);
	result->return_value = g_function_return;
	result->output_str = output_buffer;
	result->bytes_read = bytes_read;
}

void handle_errors(int wstatus) {
	tester_putstr(BOLD RED);
	tester_putnbr(g_current_test);
	switch(wstatus - 128) {
		case SIGSEGV: /* classic segfault */
			tester_putstr(".SIGSEGV! " RESET);
			break;
		case 14 - 128: /* timeout */
			tester_putstr(".TIMEOUT! " RESET);
			break;
		default: /* something yet to be discovered */
			tester_putstr(".CRASH! wstatus: ");
			tester_putnbr(wstatus);
			tester_putstr(RESET);
	}
}
