#ifndef FT_PRINTF_TESTER_H
# define FT_PRINTF_TESTER_H

int check_return(int user_file, int orig_file);

# define ERRORS_LEAK 1
# define ERRORS_BUFFER_OVERFLOW 2
# define ERRORS_SIGSEGV 3

#endif
