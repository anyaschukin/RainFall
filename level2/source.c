#include <stdio.h>
#include <string.h>
#include <unistd.h>

void	p(void)
{
	void	*return_address;
	char	buffer[76];

	fflush(stdout);
	gets(buffer);
	return_address = __builtin_return_address (0); // 0 = current function
	if (((unsigned int)return_address & 0xb0000000) == 0xb0000000)
	{
		printf("(%p)\n", return_address);
		_exit(1);
	}
	puts(buffer);
	strdup(buffer);
	return ;
}

int	main(void)
{
	p();
	return (0);
}
