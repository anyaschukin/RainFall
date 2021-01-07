#include <stdlib.h>
#include <stdio.h>

void	run(void)
{
	fwrite("Good...Wait What?", 1, 19, stdout);
	system("/bin/sh");
}

int	main(void)
{
	char	s[76];

	gets(s);
	return ;
}
