#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct builtin {
	const char *cmd;
	void (*cmd_cb)(int argc, char **argv);
};

void shell_exit(int argc, char **argv);
void aecho(int argc, char **argv);

static struct builtin builtin_commands[] = {
					    {.cmd = "exit", .cmd_cb  = shell_exit},
					    {.cmd = "aecho", .cmd_cb = aecho}
};

static int n_builtin = 2;

int get_builtin(char *cmd)
{
	int i;

	for (i = 0; i < n_builtin; i++) 
		if (strcmp(cmd, builtin_commands[i].cmd) == 0)
			return i;
	return -1;
}

void run_builtin(int idx, int argc, char **argv)
{
	builtin_commands[idx].cmd_cb(argc, argv);
}

void shell_exit(int argc, char **argv)
{
	exit(0);
}

void aecho(int argc, char **argv)
{
	int i;

	for (i = 0; i < argc; i++) 
		printf("%s ", argv[i]);
	putchar('\n');
}
