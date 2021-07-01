#include "get_next_line.h"
#include "libftprintf.h"


int check_return(int user_file, int orig_file)
{
	int failed = 0;
	char *orig_return_str;
	char *user_return_str;
	int orig_return;
	int user_return;
	int result = 1;

	get_next_line(orig_file, &orig_return_str);
	result = get_next_line(user_file, &user_return_str);
	orig_return = atoi(orig_return_str);
	user_return = atoi(user_return_str);
	free(orig_return_str);
	free(user_return_str);
	if (user_return != orig_return)
		failed = 1;

	return (failed);
}
