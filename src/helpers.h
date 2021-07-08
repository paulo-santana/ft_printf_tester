#ifndef HELPERS_H
# define HELPERS_H

# include <unistd.h>

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
# define BUFSIZE 100

#define __PRINTF_EXPECTED(params, expected) { \
	already_printed_help = 0; \
	char user_fake_stdout[BUFSIZE]; \
	char orig_fake_stdout[BUFSIZE]; \
	int function_return; \
	if (current_test == test_nbr || test_nbr == 0) \
	{ \
		t_result orig_result, user_result; \
		int stdout_pipe[2]; \
		int rtrn_pipe[2]; \
		if (pipe(stdout_pipe) < 0) \
			return (tester_putstr("error opening stdout_pipe\n"), 26); \
		if (pipe(rtrn_pipe) < 0) \
			return (tester_putstr("error opening rtrn_pipe\n"), 24); \
		int child = fork(); \
		if (child == 0) \
		{ \
			/* the child don't need to read anything */ \
			close(stdout_pipe[READ]); \
			close(rtrn_pipe[READ]);  \
			/* int file = open("files/original_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); */ \
			int err = open("files/original_stderr.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); /* get rid of real printf errors*/ \
			dup2(stdout_pipe[WRITE], 1); \
			dup2(err, 2); \
			int result = printf expected; \
			write(rtrn_pipe[WRITE], &result, sizeof(int)); \
			close(stdout_pipe[WRITE]); \
			close(rtrn_pipe[WRITE]);  \
			close(err); \
			return (0); \
		} \
		else \
		{ \
			close(stdout_pipe[WRITE]); \
			close(rtrn_pipe[WRITE]); \
			int bytes_read = read(stdout_pipe[READ], orig_fake_stdout, BUFSIZE); \
			read(rtrn_pipe[READ], &function_return, sizeof(int)); \
			close(stdout_pipe[READ]); \
			close(rtrn_pipe[READ]); \
			orig_result.return_value = function_return; \
			orig_result.output_str = orig_fake_stdout; \
			orig_result.bytes_read = bytes_read; \
			waitpid(child, &wstatus, 0); \
		} \
		if (pipe(stdout_pipe) < 0) \
			return (tester_putstr("error opening stdout_pipe\n"), 26); \
		if (pipe(rtrn_pipe) < 0) \
			return (tester_putstr("error opening rtrn_pipe\n"), 24); \
		child = fork(); \
		if (child == 0) { \
			/* the child don't need to read anything */ \
			close(stdout_pipe[READ]); \
			close(rtrn_pipe[READ]);  \
			/*int file = open("files/user_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); */ \
			int err = open("files/user_stderr.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); \
			dup2(stdout_pipe[WRITE], 1); \
			dup2(err, 2); \
			alarm(1); \
			int result = ft_printf params; \
			write(rtrn_pipe[WRITE], &result, sizeof(int)); \
			close(stdout_pipe[WRITE]); \
			close(rtrn_pipe[WRITE]); \
			close(err); \
			return (0); \
		} \
		else \
		{ \
			waitpid(child, &wstatus, 0); \
			/* 30 is the status code for the leak sanitizer   */ \
			if (wstatus != 0 && wstatus != 256 * 30) \
			{ \
				tester_putstr(BOLD RED); \
				tester_putnbr(current_test); \
				switch(wstatus - 128) { \
					case SIGSEGV: /* classic segfault */ \
						tester_putstr(".SIGSEGV! " RESET); \
						break; \
					case 14 - 128: /* timeout */ \
						tester_putstr(".TIMEOUT! " RESET); \
						break; \
					default: /* something yet to be discovered */ \
						tester_putstr(".CRASH! wstatus: "); \
						tester_putnbr(wstatus); \
						tester_putstr(RESET); \
				} \
				print_help(#params); \
			} \
			else \
			{ \
				close(stdout_pipe[WRITE]); \
				close(rtrn_pipe[WRITE]); \
				int bytes_read = read(stdout_pipe[READ], user_fake_stdout, BUFSIZE); \
				read(rtrn_pipe[READ], &function_return, sizeof(int)); \
				close(stdout_pipe[READ]); \
				close(rtrn_pipe[READ]); \
				user_result.return_value = function_return; \
				user_result.output_str = user_fake_stdout; \
				user_result.bytes_read = bytes_read; \
				waitpid(child, &wstatus, 0); \
				check_result(user_result, orig_result, #params); \
			} \
		} \
	} \
	current_test++; \
}

#endif
