#ifndef HELPERS_H
# define HELPERS_H

# define PRINTF(params, description) __PRINTF_EXPECTED(params, params, description)

# define MALLOC_COUNT 1
# define LEAK_SANITIZER 2

#ifdef __linux__
# define PRINTF_EXPECTED(params, expected, description) __PRINTF_EXPECTED(params, expected, description)
# define LEAK_CHECKER LEAK_SANITIZER
#elif defined __APPLE__
# define PRINTF_EXPECTED(params, expected, description) __PRINTF_EXPECTED(params, params, description)
# define LEAK_CHECKER MALLOC_COUNT
#endif

#define __PRINTF_EXPECTED(params, expected, description) { \
	already_printed_help = 0; \
	if (current_test == test_nbr || test_nbr == 0) \
	{ \
		int child = fork(); \
		if (child == 0) \
		{ \
			int result; \
			int file = open("files/original_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); \
			int err = open("files/original_stderr.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); /* get rid of real printf errors*/ \
			dup2(file, 1); \
			dup2(err, 2); \
			result = printf expected; \
			printf("\n%d", result); \
			return (0); \
		} \
		else \
		{ \
			waitpid(child, &wstatus, 0); \
		} \
		child = fork(); \
		if (child == 0) { \
			int result; \
			int file = open("files/user_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); \
			int err = open("files/user_stderr.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); \
			dup2(file, 1); \
			dup2(err, 2); \
			alarm(1); \
			result = ft_printf params; \
			tester_putchar('\n'); \
			tester_putnbr(result); \
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
				check_result(description, #params); \
			} \
		} \
	} \
	current_test++; \
}

#endif
