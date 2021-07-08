#ifndef FT_PRINTF_TESTER_H
# define FT_PRINTF_TESTER_H
# include <unistd.h>

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

#endif
