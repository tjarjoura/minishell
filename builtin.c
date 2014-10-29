#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct builtin {
	const char *cmd;
	void (*cmd_cb)(int argc, char **argv);
};

static void shell_exit(int argc, char **argv);
static void aecho(int argc, char **argv);

static struct builtin builtin_commands[] = {
					    {.cmd = "exit", .cmd_cb  = shell_exit},
					    {.cmd = "aecho", .cmd_cb = aecho}
};

static int n_builtin = 2;

int builtin(char *cmd, int argc, char **argv)
{
	int i;

	for (i = 0; i < n_builtin; i++) 
		if (strcmp(cmd, builtin_commands[i].cmd) == 0) {
			builtin_commands[i].cmd_cb(argc, argv);
			return i;	
		}

	return -1;
}

static void shell_exit(int argc, char **argv)
{
	int exit_val;

	if (argc < 2)
		exit_val = 0;
	else
		exit_val = atoi(argv[1]);
	
	exit(exit_val);
}

static void aecho(int argc, char **argv)
{
	int i, newline;
	i = 1;
	if (strcmp(argv[1], "-n") == 0) {
		i++;
		newline = 0;
	}
	
	for ( ; i < argc; i++) 
		dprintf(STDOUT_FILENO, "%s ", argv[i]);

	if (newline)
		dprintf(STDOUT_FILENO, "\n");
}
