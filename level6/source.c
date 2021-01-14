#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void(*t_func_ptr)(void);

void	m(void)
{
	puts("Nope");
}

void	n(void)
{
	system("/bin/cat /home/user/level7/.pass");
}

int	main(int argc, char **argv)
{
	char		*buffer;
	t_func_ptr	*func;

	buffer = malloc(64);
	func = malloc(4);
	*func = (void *)m;
	strcpy(buffer, argv[1]);
	(**func)();
	return (0);
}
