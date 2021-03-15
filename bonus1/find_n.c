/*
	find_n.c
*/
#include <stdio.h>
#include <limits.h>

int	main(void)
{
	int	n;

	n = INT_MIN;
	while (n < 0)
	{
		if (n * 4 >= 44)
		{
			printf("n = %d, n * 4 = %d\n", n, n * 4);
			break ;
		}
		n++;
	}
	return (0);
}
