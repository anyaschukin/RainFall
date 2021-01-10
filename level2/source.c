#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	*p(void)
{
	char	buffer[64];
	void	*return_address;

	fflush(stdout);
	gets(buffer);
	return_address = __builtin_return_address(0);
	if (((unsigned int)return_address & 0xb0000000) == 0xb0000000)
	{
		printf("(%p)\n", return_address);
		exit(1);
	}
	puts(buffer);
	return (strdup(buffer));
}

int	main(void)
{
	return (int)p();
}
