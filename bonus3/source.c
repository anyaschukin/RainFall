#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int	main(int argc, char **argv)
{
	int		ret;
	char	buffer[132];
	FILE	*passwd;

	passwd = fopen("/home/user/end/.pass", "r");
	memset(buffer, 0, 132);
	if ((!passwd) || (argc != 2))
		return (-1);
	else
	{
		fread(buffer, 1, 66, passwd);
		buffer[65] = 0;
		buffer[atoi(argv[1])] = 0;
		fread(&buffer[66], 1, 65, passwd);
		fclose(passwd);
		if (strcmp(buffer, argv[1]) == 0)
			execl("/bin/sh", "sh", NULL);
		else
			puts(&buffer[66]);
	}
	return (0);
}
