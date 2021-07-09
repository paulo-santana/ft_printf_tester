#include "libtest.h"

int test_string(char *expected, char *got, int limit)
{
	int success = 1;
	//int len = 0;

	if (expected == NULL)
	{
		if (got != NULL)
			success = 0;
	}
	else if (got == NULL)
		success = 0;
	else if (memcmp(expected, got, limit) != 0)
	{
		success = 0;
		//len = strlen(expected) + 1;
	}
	//if (!success)
	//	print_string_diff(expected, got, len);
	return (success);
}
