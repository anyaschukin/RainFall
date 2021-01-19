#include <stdlib.h> // malloc
#include <stdio.h> // printf, fopen, fgets, puts
#include <string.h> // strcpy
#include <time.h>

char	g_c[68];

void	m(void)
{
	printf("%s - %d\n", g_c, time(0));
	return ;
}

int	main(int argc, char **argv)
{
	char	*s1;
	char	*s2;

	s1 = (char *)malloc(8);
	s1[0] = 1;
	s1[1] = (int)malloc(8);

	s2 = (char *)malloc(8);
	s2[0] = 2;
	s2[1] = (int)malloc(8);

	strcpy(s1[1], argv[1]);
	strcpy(s2[1], argv[2]);

	fgets(g_c, 68, fopen("/home/user/level8/.pass", "r"));
	puts("~~");
	return (0);
}
