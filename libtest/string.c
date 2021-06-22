#include "libtest.h"

void ft_putchar(char c)
{
	write(1, &c, 1);
}

void ft_putstr(char *str)
{
	size_t len = strlen(str);
	write(1, str, len);
}

void ft_putnbr(int nbr)
{
	long	number;
	char	digit;

	number = nbr;
	if (number < 0)
	{
		write(1, "-", 1);
		number *= -1;
	}
	if (number > 9)
	{
		ft_putnbr(number / 10);
		ft_putnbr(number % 10);
	}
	else
	{
		digit = number + '0';
		write(1, &digit, 1);
	}
}
