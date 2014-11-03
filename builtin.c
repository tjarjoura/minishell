#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "proto.h"

struct builtin {
	const char *cmd;
	void (*cmd_cb)(int argc, char **argv);
};

/* Command callback prototypes */
static void shell_exit(int argc, char **argv);
static void aecho(int argc, char **argv);
static void envset(int argc, char **argv);
static void envunset(int argc, char **argv);
static void cd(int argc, char **argv);
static void shift(int argc, char **argv);
static void unshift(int argc, char **argv);

static struct builtin builtin_commands[] = {
					    {.cmd = "exit", .cmd_cb  = shell_exit},
					    {.cmd = "aecho", .cmd_cb = aecho},
					    {.cmd = "envset", .cmd_cb = envset},
					    {.cmd = "envunset", .cmd_cb = envunset},
					    {.cmd = "cd", .cmd_cb = cd},
					    {.cmd = "shift", .cmd_cb = shift},
					    {.cmd = "unshift", .cmd_cb = unshift}
};

static int n_builtin = 7;

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
	if (argc < 2) 
		return;

	if (strcmp(argv[1], "-n") == 0) {
		i++;
		newline = 0;
	}
	
	for ( ; i < argc; i++) 
		dprintf(STDOUT_FILENO, "%s ", argv[i]);

	if (newline)
		dprintf(STDOUT_FILENO, "\n");
}

static void envset(int argc, char **argv)
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <name> <value>\n", argv[0]);
		return;
	}

	if (setenv(argv[1], argv[2], 1) < 0)
		perror("envset");
}

static void envunset(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <name>\n", argv[0]);		
		return;
	}

	if (unsetenv(argv[1]) < 0)
		perror("envunset");
}

static void cd(int argc, char **argv)
{
	char *home_dir, cwd[50];
	if (argc < 2) {
		home_dir = getenv("HOME");
		if (home_dir == NULL)
			fprintf(stderr, "Set HOME environment variable or give an argument.\n");
		else if (chdir(home_dir) < 0)
			perror("chdir");
		else {
			getcwd(cwd, 50);
			setenv("PWD", cwd, 1);
		}
		return;
	}

	if (chdir(argv[1]) < 0)
		perror("chdir");
	else {
		getcwd(cwd, 50);
		setenv("PWD", cwd, 1);
	}
}

static void shift(int argc, char **argv)
{
	cmdline_shift += atoi(argv[1]);
}

static void unshift(int argc, char **argv)
{
	cmdline_shift -= atoi(argv[1]);

	if (cmdline_shift < 0)
		cmdline_shift = 0;
}
