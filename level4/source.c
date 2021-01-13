#include <stdio.h>
#include <stdlib.h>

int		g_m = 0;

int	p(char *buffer)
{
	return (printf(buffer));
}

void	n(void)
{
	char	buffer[512];

	fgets(buffer, 512, stdin);
	p(buffer);
	if (g_m == 16930116)
		system("/bin/cat /home/user/level5/.pass");
	return ;
}

int	main(void)
{
	n();
	return (0);
}
