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

int check_return(int user_file, int orig_file);
void describe(char *test_title);
int check_result(t_result user_result, t_result orig_result, char *params_used);
void print_help(char *params_used);

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

#endif
