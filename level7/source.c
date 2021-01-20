#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char c[68];

void	m(void)
{
	printf("%s - %d\n", c, (int)time(0));
}

int		main(int argc, char **argv)
{
	int		*s1;
	int		*s2;

	s1 = malloc(8);
	s1[0] =1;
	s1[1] = malloc(8);

	s2 = malloc(8);
	s2[0] = 2;
	s2[1] = malloc(8);

	strcpy((char *)s1[1], argv[1]);
	strcpy((char *)s2[1], argv[2]);

	fgets(c, 68, fopen("/home/user/level8/.pass", "r"));

	puts("~~");

	return (0);
}
