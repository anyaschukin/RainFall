#include <stdio.h>
#include <stdlib.h>

int	g_m;

void	v(void)
{
	char	buffer[512];

	fgets(buffer, 512, stdin);
	printf(buffer);
	if (g_m == 64)
	{
		fwrite("Wait what?!\n", 1, 12, stdout);
		system("/bin/sh");
	}
	return ;
}

int	main(void)
{
	v();
	return (0);
}
