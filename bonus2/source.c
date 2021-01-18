#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int	g_language = 0;

void	greetuser(char *user)
{
	char	greeting[64];

	if (g_language == 1)
		strcpy(greeting, "Hyvää päivää ");
	else if (g_language == 2)
		strcpy(greeting, "Goedemiddag! ");
	else if (g_language == 0)
		strcpy(greeting, "Hello ");
	puts(strcat(greeting, user));
}

int	main(int argc, char **argv)
{
	char	user[72];
	char	*env;

	env = NULL;
	if (argc != 3)
		return (1);
	memset(user, 0, 72);
	strncpy(user, argv[1], 40);
	strncpy(&user[40], argv[2], 32);
	env = getenv("LANG");
	if (env)
	{
		if (memcmp(env, "fi", 2) == 0)
			g_language = 1;
		else if (memcmp(env, "nl", 2) == 0)
			g_language = 2;
	}
	greetuser(user);
	return (0);
}
