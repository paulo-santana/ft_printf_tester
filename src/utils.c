#include "get_next_line.h"
#include "libftprintf.h"
#include "../libtest/libtest.h"
#include "ft_printf_tester.h"
#include "helpers.h"
#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>

extern int g_current_test;
extern int g_test_nbr;
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
	tester_putstr(";\n");
}

int already_printed_help = 0;
void print_help(char *params_used)
{
	if (already_printed_help)
		return ;
	already_printed_help = 1;
	tester_putstr("\n     ");
	tester_putstr(RESET BOLD "You can rerun this test with " RESET YELLOW "sh test ");
	tester_putnbr(g_current_test);
	tester_putstr(RESET "\n     ");
	tester_putstr("The function was called like this:\n   ");
	pretty_printf(params_used);
}

void	print_output(t_result *expected, t_result *user, unsigned int expected_size, unsigned int result_size)
//void	print_string_diff(char expected[], char result[], unsigned int expected_size, unsigned int result_size)
{
	int is_red_already = 0;

	tester_putstr(RESET "        Expected: [");
	for (unsigned int i = 0; i < expected_size; i++)
	{
		if (expected->output_str[i] == '\0')
			tester_putstr(MAGENTA "\\0" RESET);
		else if (expected->output_str[i] == '\n')
			tester_putstr(MAGENTA "\\n" RESET);
		else if (!isprint(expected->output_str[i]))
			print_non_print(expected->output_str[i]);
		else
			tester_putchar(expected->output_str[i]);
	}
	tester_putstr("], return: ");
	tester_putnbr(expected->return_value);

	tester_putstr("\n        Got:      [");
	unsigned int i = 0;
	int expected_ended = 0;
	while (i < result_size)
	{
		if (expected_ended || ((expected->output_str[i] != user->output_str[i]) && !is_red_already))
		{
			is_red_already = 1;
			tester_putstr(BOLD RED);
		}
		else if (expected->output_str[i] == user->output_str[i] && is_red_already)
		{
			tester_putstr(RESET);
			is_red_already = 0;
		}
		if (user->output_str[i] == '\0')
			tester_putstr(BOLD MAGENTA "\\0" RESET);
		else if (user->output_str[i] > 1 && user->output_str[i] < 32)
			print_non_print(user->output_str[i]);
		else
			tester_putchar(user->output_str[i]);
		if (!expected_ended && expected->output_str[i] == '\0')
			expected_ended = 1;
		i++;
	}
	tester_putstr(RESET "], return: ");
	tester_putnbr(user->return_value);
}

int check_leaks_sanitizer(int user_stderr)
{
	int n = 0;
	int error = 0;
	char *line;
	int result = get_next_line(user_stderr, &line); // get rid of the first line
	free(line);
	if (result <= 0)
		return (0);
	result = get_next_line(user_stderr, &line);
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

int g_tests_failed = 0;

# define MAX(x, y) ((x > y) ? x : y)

int check_result(t_result *user_result, t_result *orig_result, char *params_used)
{
	if (g_current_test == g_test_nbr || g_test_nbr == 0)
	{
		char *result;
		char *expected;
		int success = 1;
		int errors = 0;
		int wrong_return = 0;

		errors = check_errors(params_used);
		expected = orig_result->output_str;
		result = user_result->output_str;
		if (!errors || errors == ERRORS_LEAK)
		{
			success = test_string(expected, result, MAX(orig_result->bytes_read, user_result->bytes_read));
			wrong_return = user_result->return_value != orig_result->return_value;
		}

		if (success && !wrong_return && !errors)
			tester_putstr(GREEN);
		else
			tester_putstr(BOLD RED "\n  "), g_tests_failed++;
		tester_putnbr(g_current_test);
		tester_putchar('.');

		if (success && !wrong_return && !errors)
			tester_putstr(BOLD "OK" RESET);
		else if (errors && errors != ERRORS_LEAK) {
			if (errors == ERRORS_SIGSEGV || errors == ERRORS_BUFFER_OVERFLOW)
				tester_putstr(BOLD "SIGSEGV!");
			else
				tester_putstr(BOLD "UNKNOWN CRASH!");
		}
		else
			tester_putstr("KO");

		if (!success)
			tester_putstr(" (Wrong output)");
		if (wrong_return)
			tester_putstr(" (Wrong return)");
		if (errors == ERRORS_LEAK)
			tester_putstr(RED " (LEAKS!)");
		if (!success || wrong_return) {
			tester_putstr("\n");
			print_output(orig_result, user_result, orig_result->bytes_read, user_result->bytes_read);
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

extern int right_cat;

void describe(char *test_title)
{
	if (!right_cat || g_test_nbr != 0)
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

extern int g_function_return;

void prepare_test(char *err_file, int *outpipe, int *retpipe) {
	/* the child don't need to read anything */
	close(outpipe[READ]);
	close(retpipe[READ]); 
	/* int file = open("files/original_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); */
	int err = open(err_file, O_CREAT | O_WRONLY | O_TRUNC, 0644); /* get rid of real printf errors*/
	dup2(outpipe[WRITE], 1);
	dup2(err, 2);
	g_function_return = -10;
}

void finish_test(int result, int *outpipe, int *retpipe)
{
	write(retpipe[WRITE], &result, sizeof(int));
	close(outpipe[WRITE]);
	close(retpipe[WRITE]); 
	exit (0);

}

void fetch_result(t_result *result, char *output_buffer, int *stdout_pipe, int *rtrn_pipe)
{
	close(stdout_pipe[WRITE]);
	close(rtrn_pipe[WRITE]);
	int bytes_read = read(stdout_pipe[READ], output_buffer, BUFSIZE);
	g_function_return = -10;
	read(rtrn_pipe[READ], &g_function_return, sizeof(int));
	close(stdout_pipe[READ]);
	close(rtrn_pipe[READ]);
	result->return_value = g_function_return;
	result->output_str = output_buffer;
	result->bytes_read = bytes_read;
}

extern char *g_test_params;
extern char *g_user_fake_stdout;

void handle_errors(int wstatus, t_result *user_r, t_result *orig_r,
		char *user_output, int *output_pipe, int *return_pipe) {
	/* 30 is the status code for the leak sanitizer   */
	int child_exit_status = WEXITSTATUS(wstatus);
	if (child_exit_status && child_exit_status != 30) {
		tester_putstr(BOLD RED);
		tester_putnbr(g_current_test);
		switch(child_exit_status) {
			case SIGSEGV: /* classic segfault */
				tester_putstr(".SIGSEGV! " RESET);
				break;
			case SIGKILL: /* killed because of timeout */
				tester_putstr(".TIMEOUT! " RESET);
				break;
			case SIGABRT:
				tester_putstr(".SIGABRT! " RESET);
				break ;
			default: /* something yet to be discovered */
				tester_putstr(".CRASH! wstatus: ");
				tester_putnbr(child_exit_status);
				tester_putstr(RESET);
		}
		print_help(g_test_params);
	} else {
		fetch_result(user_r, user_output, output_pipe, return_pipe);
		check_result(user_r, orig_r, g_test_params);
	}
}
