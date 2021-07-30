#include "libtest.h"

void print_int_diff(int expected, int result) {
	tester_putstr("        Expected: ");
	tester_putnbr(expected);
	tester_putstr("\n        Got     : " RED);
	tester_putnbr(result);
	tester_putstr(RESET "\n");
}

void print_non_print(char c) {
	char cx[3];
	int size = 0;
	char *hexmap = "0123456789abcdef";

	if (c == '\n') {
		cx[0] = '\\';
		cx[1] = 'n';
		size = 2;
	} else if (c >= 16) {
		cx[0] = '\\';
		cx[1] = hexmap[c / 16];
		cx[2] = hexmap[c % 16];
		size = 3;
	} else {
		cx[0] = '\\';
		cx[1] = hexmap[c % 16];
		size = 2;
	}
	tester_putstr(MAGENTA);
	write(1, cx, size);
	tester_putstr(RESET);
}
