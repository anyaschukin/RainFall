#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char	*g_auth = NULL;
char	*g_service = NULL;

int	main(void)
{
	char	buffer[128];

	while (1)
	{
		printf("%p, %p\n", g_auth, g_service);
		if (!fgets(buffer, 128, stdin))
			break ;
		if (!strncmp(buffer, "auth ", 5))
		{
			g_auth = malloc(4);
			g_auth[0] = 0;
			if (strlen(buffer + 5) <= 30)
				strcpy(g_auth, buffer + 5);
		}
		if (!strncmp(buffer, "reset", 5))
			free(g_auth);
		if (!strncmp(buffer, "service", 6))
			g_service = strdup(buffer + 7);
		if (!strncmp(buffer, "login", 5))
		{
			if (g_auth[32] != 0)
				system("/bin/sh");
			else
				fwrite("Password:\n", 10, 1, stdout);
		}
	}
	return (0);
}
