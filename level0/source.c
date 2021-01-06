#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#define _GNU_SOURCE

int	main(int ac, char **av)
{
	int		password;
	char	*args[2];

	password = atoi(av[1]);
	if (password == 423)
	{
		args[0] = strdup("/bin/sh");
		args[1] = NULL;
		setresgid(getegid(), getegid(), getegid());
		setresuid(geteuid(), geteuid(), geteuid());
		execv("/bin/sh", args);
	}
	fwrite("No !\n", 1, 5, stderr);
	return (0);
}
