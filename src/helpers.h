#ifndef HELPERS_H
# define HELPERS_H

# include "../libtest/libtest.h"
# include <sys/wait.h>

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

/*
 * trying to make the define smaller
 * or = orig_result;
 * ur = user_result;
 * op = stdout_pipe;
 * rp = rtrn_pipe;
 * 
 * */

#ifndef ERROR_LIMIT
# define ERROR_LIMIT 0
#endif

extern int g_tests_failed;
extern int g_tests_run;

#define __PRINTF_EXPECTED(params, expected) { \
	already_printed_help = 0; \
	should_run = (g_current_test == g_test_nbr) || (g_test_nbr == 0); \
	if (should_run && right_cat) { \
		g_tests_run++; \
		t_result or, ur; \
		int op[2], rp[2]; \
		open_pipes(op, rp); \
		int child = fork(); \
		if (child == 0) { \
			prepare_test("files/original_stderr.txt", op, rp); \
			int result = printf expected; \
			finish_test(result, op, rp); \
		} else { \
			fetch_result(&or, g_orig_fake_stdout, op, rp); \
			waitpid(child, &wstatus, 0); \
		} \
		open_pipes(op, rp); \
		int intermediate_pid = fork(); \
		if (intermediate_pid == 0) { \
			int worker = fork(); \
			if (worker == 0) { \
				prepare_test("files/user_stderr.txt", op, rp); \
				int result = ft_printf params; \
				finish_test(result, op, rp); \
			} \
			int killer = fork(); \
			if (killer == 0) { \
				/* sleep well before killing lol */ \
				sleep(1); \
				_exit(0); \
			} \
			int exited_pid = wait(&wstatus); \
			if (exited_pid == worker) { \
				kill(killer, SIGKILL); \
				wait(NULL); \
			} else { \
				kill(worker, SIGKILL); \
				wait(&wstatus); \
			} \
			int status; \
			if (WIFEXITED(wstatus)) { \
				status = WEXITSTATUS(wstatus); \
			} else if(WIFSIGNALED(wstatus)) { \
				status = WTERMSIG(wstatus); \
			} else { status = -1; } \
			_exit(status); \
		} else { \
			waitpid(intermediate_pid, &wstatus, 0); \
			g_test_params = #params; \
			handle_errors(wstatus, &ur, &or, g_user_fake_stdout, op, rp); \
		} \
	} \
	if (ERROR_LIMIT > 0 && g_tests_failed > ERROR_LIMIT) \
		return (tester_putstr(BOLD RED "Failed on more than 10 tests, stopping...\n"), 5); \
	g_current_test++; \
}

#endif
