#ifndef LIBTEST_H
# define LIBTEST_H

# include <ctype.h>
# include <unistd.h>
# include <fcntl.h>
# ifdef __linux__
#  include <bsd/string.h>
#  include <bsd/stdlib.h>
#  include <stdio.h>
#  endif
# ifdef __APPLE__
#  include <string.h>
#  include <stdlib.h>
#  include <stdio.h>
# endif

# define BOLD		"\x1b[1m"
# define RED		"\x1b[31m"
# define GREEN 		"\x1b[32m"
# define YELLOW		"\x1b[33m"
# define BLUE		"\x1b[34m"
# define MAGENTA	"\x1b[35m"
# define CYAN		"\x1b[36m"
# define RESET		"\x1b[0m"

size_t	tester_strlen(const char *str);
void ft_putchar(char c);
void ft_putstr(char *str);
void ft_putnbr(int nbr);
char *tester_strnstr(const char *big, const char *little, size_t len);
void print_int_diff(int expected, int result);
void print_string_diff(char expected[], char result[], unsigned int size);
int test_string(char *expected, char *got);
int test_int(char *description, int expected, int result);
void print_success(char *desc, int success);

#endif
