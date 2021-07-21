#ifndef LIBTEST_H
# define LIBTEST_H

# include <ctype.h>
# include <unistd.h>
# include <fcntl.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>

# define BOLD		"\x1b[1m"
# define RED		"\x1b[31m"
# define GREEN 		"\x1b[32m"
# define YELLOW		"\x1b[33m"
# define BLUE		"\x1b[34m"
# define MAGENTA	"\x1b[35m"
# define CYAN		"\x1b[36m"
# define RESET		"\x1b[0m"

size_t	tester_strlen(const char *str);
void tester_putchar(char c);
void tester_putstr(char *str);
void tester_putnbr(int nbr);
char *tester_strnstr(const char *big, const char *little, size_t len);
void print_int_diff(int expected, int result);
void print_string_diff(char expected[], char result[], unsigned int expected_size, unsigned int result_size);
int test_string(char *expected, char *got, int limit);
int test_int(char *description, int expected, int result);
void print_success(char *desc, int success);
void print_non_print(char c);

#endif
