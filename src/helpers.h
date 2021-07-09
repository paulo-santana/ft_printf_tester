#ifndef HELPERS_H
# define HELPERS_H

# include "../libtest/libtest.h"

# define PRINTF(params) __PRINTF_EXPECTED(params, params)

# define MALLOC_COUNT 1
# define LEAK_SANITIZER 2

#ifdef __linux__
# define PRINTF_EXPECTED(params, expected) __PRINTF_EXPECTED(params, expected)
# define LEAK_CHECKER LEAK_SANITIZER
#elif defined __APPLE__
# define PRINTF_EXPECTED(params, expected) __PRINTF_EXPECTED(params, params)
# define LEAK_CHECKER MALLOC_COUNT
#endif

# define READ 0
# define WRITE 1

#define __PRINTF_EXPECTED(params, expected) { \
	already_printed_help = 0; \
	if (g_current_test == test_nbr || test_nbr == 0) \
	{ \
		t_result orig_result, user_result; \
		int stdout_pipe[2]; \
		int rtrn_pipe[2]; \
		open_pipes(stdout_pipe, rtrn_pipe); \
		int child = fork(); \
		if (child == 0) { \
			prepare_test("files/original_stderr.txt", stdout_pipe, rtrn_pipe); \
			int result = printf expected; \
			finish_test(result, stdout_pipe, rtrn_pipe); \
		} \
		else \
		{ \
			fetch_result(&orig_result, g_orig_fake_stdout, stdout_pipe, rtrn_pipe); \
			waitpid(child, &wstatus, 0); \
		} \
		open_pipes(stdout_pipe, rtrn_pipe); \
		child = fork(); \
		if (child == 0) { \
			prepare_test("files/user_stderr.txt", stdout_pipe, rtrn_pipe); \
			alarm(1); \
			int result = ft_printf params; \
			finish_test(result, stdout_pipe, rtrn_pipe); \
		} \
		else \
		{ \
			waitpid(child, &wstatus, 0); \
			/* 30 is the status code for the leak sanitizer   */ \
			if (wstatus != 0 && wstatus != 256 * 30) { \
				handle_errors(wstatus); \
				print_help(#params); \
			} else { \
				fetch_result(&user_result, g_user_fake_stdout, stdout_pipe, rtrn_pipe); \
				check_result(user_result, orig_result, #params); \
			} \
		} \
	} \
	g_current_test++; \
}

#endif
