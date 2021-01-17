#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char	*p(char *s, char *str)
{
	char	buffer[4096];

	puts(str);
	read(0, buffer, 4096);
	*strchr(buffer, '\n') = 0;
	return (strncpy(s, buffer, 20));
}

char	*pp(char *buffer)
{
	char			b[20];
	char			a[20];
	unsigned int	len;

	p(a, " - ");
	p(b, " - ");
	strcpy(buffer, a);
	len = strlen(buffer);
	buffer[len] = ' ';
	buffer[len + 1] = 0;
	return (strcat(buffer, b));
}

int	main(void)
{
	char	buffer[42];

	pp(buffer);
	puts(buffer);
	return (0);
}
