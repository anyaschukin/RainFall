#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void	o(void)
{
	system("/bin/sh");
	_exit(1);
}

void	n(void)
{
	char	buffer[512];

	fgets(buffer, 512, stdin);
	printf(buffer);
	exit(1);
}

int	main(void)
{
	n();
	return (0);
}
