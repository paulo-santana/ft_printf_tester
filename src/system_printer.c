#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

	if (argc == 2)
	{
		int file = open("./files/system_output.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
		dup2(file, 1);

		char *test_number = argv[1];
		if (strcmp(test_number, "1") == 0)
		{
			printf("This is a simple test\n");
		}
	}
}
