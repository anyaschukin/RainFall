#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	main(int ac, char **av)
{
	int		n;
	char	buffer[40];

	n = atoi(av[1]);
	if (!(n <= 9))
		return (1);
	memcpy(buffer, av[2], n * 4);
	if (n == 0x574f4c46)
		execl("/bin/sh", "sh", NULL);
	return (0);
}
