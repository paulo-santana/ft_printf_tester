#ifndef FT_PRINTF_TESTER_H
# define FT_PRINTF_TESTER_H
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>

typedef struct s_result {
	int return_value;
	char *output_str;
	ssize_t bytes_read;
} t_result;

int run_tests(int tests);

int check_return(int user_file, int orig_file);
void dscribe(char *test_title);
int check_result(t_result *user_result, t_result *orig_result, char *params_used);
void print_help(char *params_used);
void describe(char *test_title);

# define ERRORS_LEAK 1
# define ERRORS_BUFFER_OVERFLOW 2
# define ERRORS_SIGSEGV 3

# define BUFSIZE 100

void open_pipes(int *p1, int *p2);
void prepare_test(char *err_file, int *outpipe, int *retpipe);
void finish_test(int result, int *outpipe, int *retpipe);
void fetch_result(t_result *result, char *output_buffer, int *stdout_pipe, int *rtrn_pipe);
void handle_errors(int wstatus, t_result *user_r, t_result *orig_r,
		char *user_output, int *output_pipe, int *return_pipe);

# define CAT_C         0b00000000000001
# define CAT_S         0b00000000000010
# define CAT_P         0b00000000000100
# define CAT_D         0b00000000001000
# define CAT_I         0b00000000010000
# define CAT_U         0b00000000100000
# define CAT_X         0b00000001000000
# define CAT_BIG_X     0b00000010000000
# define CAT_PERCENT   0b00000100000000
# define CAT_MANDATORY 0b00001000000000
# define CAT_BONUS     0b00010000000000
# define CAT_BONUS_1   0b00100000000000
# define CAT_BONUS_2   0b01000000000000

#endif
