#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include "proto.h"

struct builtin {
	const char *cmd;
	int (*cmd_cb)(int argc, char **argv);
};

/* Command callback prototypes */
static int shell_exit(int argc, char **argv);
static int aecho(int argc, char **argv);
static int envset(int argc, char **argv);
static int envunset(int argc, char **argv);
static int cd(int argc, char **argv);
static int shift(int argc, char **argv);
static int unshift(int argc, char **argv);
static int sstat(int argc, char **argv);

static struct builtin builtin_commands[] = {
					    {.cmd = "exit", .cmd_cb  = shell_exit},
					    {.cmd = "aecho", .cmd_cb = aecho},
					    {.cmd = "envset", .cmd_cb = envset},
					    {.cmd = "envunset", .cmd_cb = envunset},
					    {.cmd = "cd", .cmd_cb = cd},
					    {.cmd = "shift", .cmd_cb = shift},
					    {.cmd = "unshift", .cmd_cb = unshift},
                        {.cmd = "sstat", .cmd_cb = sstat}
};

static int n_builtin = 8;

int builtin(char *cmd, int argc, char **argv, int infile, int outfile, int errfile)
{
	int i;

	for (i = 0; i < n_builtin; i++) 
		if (strcmp(cmd, builtin_commands[i].cmd) == 0) {
            /* redirect standard files */
            if (dup2(infile, STDIN_FILENO) < 0) {
                perror("dup2");
                return -1;
            }
            
            if (dup2(outfile, STDOUT_FILENO) < 0) {
                perror("dup2");
                return -1;
            }

            if (dup2(errfile, STDERR_FILENO) < 0) {
                perror("dup2");
                return -1;
            }

			return builtin_commands[i].cmd_cb(argc, argv);
        }

	return -1;
}

static int shell_exit(int argc, char **argv)
{
	int exit_val;

	if (argc < 2)
		exit_val = 0;
	else
		exit_val = atoi(argv[1]);
	
	exit(exit_val);
}

static int aecho(int argc, char **argv)
{
	int i, newline;
	i = 1;
	if (argc < 2) 
		return 0;

	if (strcmp(argv[1], "-n") == 0) {
		i++;
		newline = 0;
	}
	
	for ( ; i < argc; i++) 
		dprintf(STDOUT_FILENO, "%s ", argv[i]);

	if (newline)
		dprintf(STDOUT_FILENO, "\n");

	return 0;
}

static int envset(int argc, char **argv)
{
	if (argc < 3) {
		dprintf(STDERR_FILENO, "Usage: %s <name> <value>\n", argv[0]);
		return 1;
	}

	if (setenv(argv[1], argv[2], 1) < 0) {
		perror("envset");
		return 1;
	}

	return 0;
}

static int envunset(int argc, char **argv)
{
	if (argc < 2) {
		dprintf(STDERR_FILENO, "Usage: %s <name>\n", argv[0]);		
		return 1;
	}

	if (unsetenv(argv[1]) < 0) {
		perror("envunset");
		return 1;
	}

	return 0;
}

static int cd(int argc, char **argv)
{
	char *home_dir, cwd[50];
	if (argc < 2) {
		home_dir = getenv("HOME");
		if (home_dir == NULL) {
			dprintf(STDERR_FILENO, "Set HOME environment variable or give an argument.\n");
			return 1;
		} else if (chdir(home_dir) < 0) {
			perror("chdir");
			return 1;
		} else {
			getcwd(cwd, 50);
			setenv("PWD", cwd, 1);
		}
		
		return 0;
	}

	if (chdir(argv[1]) < 0) {
		perror("chdir");
		return 1;
	} else {
		getcwd(cwd, 50);
		setenv("PWD", cwd, 1);
	}

	return 0;
}

static int shift(int argc, char **argv)
{
	cmdline_shift += atoi(argv[1]);

	return 0;
}

static int unshift(int argc, char **argv)
{
	if (argc < 2) {
		cmdline_shift = 1;
		return 0;
	}

	cmdline_shift -= atoi(argv[1]);

	if (cmdline_shift < 1)
		cmdline_shift = 1;

	return 0;
}

static void get_perm(mode_t mode, char *perm_string)
{
    if (S_ISREG(mode))
        perm_string[0] = '-';
    else if (S_ISDIR(mode))
        perm_string[0] = 'd';
    else if (S_ISCHR(mode))
        perm_string[0] = 'c';
    else if (S_ISBLK(mode))
        perm_string[0] = 'b';
    else if (S_ISLNK(mode))
        perm_string[0] = 'l';

    perm_string[1] = (S_IRUSR & mode) ? 'r' : '-';
    perm_string[2] = (S_IWUSR & mode) ? 'w' : '-';
    perm_string[3] = (S_IXUSR & mode) ? 'x' : '-';

    perm_string[4] = (S_IRGRP & mode) ? 'r' : '-';
    perm_string[5] = (S_IWGRP & mode) ? 'w' : '-';
    perm_string[6] = (S_IXGRP & mode) ? 'x' : '-';

    perm_string[7] = (S_IROTH & mode) ? 'r' : '-';
    perm_string[8] = (S_IWOTH & mode) ? 'w' : '-';
    perm_string[9] = (S_IXOTH & mode) ? 'x' : '-';

    perm_string[10] = '\0';
}

static int sstat(int argc, char **argv)
{
    int i;
    struct stat s;
    struct passwd *p;
    struct group *g;
    char perm_string[11];
    struct tm* ts;

    if (argc < 2) {
        dprintf(STDERR_FILENO, "Usage: sstat file [file...]\n");
        return 1;
    }
    
    for (i = 1; i < argc; i++) {
        if (stat(argv[i], &s) < 0) {
            perror("stat");
            continue;
        }
        
        if ((p = getpwuid(s.st_uid)) == NULL) {
            perror("getpwuid");
            continue;
        }

        if ((g = getgrgid(s.st_gid)) == NULL) {
            perror("getgrgid");
            continue;
        }

        get_perm(s.st_mode, perm_string);
        ts = localtime(&s.st_mtime);
        
        dprintf(STDOUT_FILENO, "%s %s %s %s %d %d %s", argv[i], p->pw_name, g->gr_name, perm_string, (int) s.st_nlink, (int) s.st_size, asctime(ts));
    }

    return 0;
}
