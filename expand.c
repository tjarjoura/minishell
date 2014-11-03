#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include "proto.h"

static int expand_env(char **oldp, char **newp, int space_left, int brace_flag)
{
	int i, n = 0;
	char env_buffer[MAX_ENV], *env_val;
	char *old = *oldp;
	char *new = *newp;

	i = 0;

	if (!brace_flag)
		while ((*old != ' ') && (*old != '\0'))	
			env_buffer[i++] = *old++;
	else {
		old++;
		while ((*old != '}')) {
			if (*old == '\0')
				return ENOBRACE;
			env_buffer[i++] = *old++;
		}
		old++;
	}

	env_buffer[i] = '\0';

	env_val = getenv(env_buffer);

	if (env_val != NULL) {
		if ((n = strlen(env_val)) > space_left)
			return ENOROOM;

		while (*env_val != '\0')
			*new++ = *env_val++;
	}
	
	*oldp = old - 1;
	*newp = new;
	return n;
}

static int expand_pid(char **newp, int space_left)
{
	int i, n;
	pid_t pid;
	char pid_buffer[10];
	char *new = *newp;
	
	pid = getpid();
	snprintf(pid_buffer, 10, "%d", pid);

	if ((n = strlen(pid_buffer)) > space_left) 
		return ENOROOM;
	i = 0;
	while (pid_buffer[i] != '\0')
		*new++ = pid_buffer[i++];
	
	*newp = new;
	return n;
}

static int expand_argv(char **oldp, char **newp, int space_left)
{
	char arg_idx[4], *arg;
	char *old = *oldp;
	char *new = *newp;
	int argn, n, i = 0;
	
	while (isdigit(*old))
		arg_idx[i++] = *old++;
	arg_idx[i] = '\0';

	argn = atoi(arg_idx);

	/* replace $0 with the name of the shell in interactive mode */	
	if ((cmdline_argc < 2) && (argn == 0)) {
		n = strlen(cmdline_argv[0]);
		if (n > space_left)
			return ENOROOM;
		arg = cmdline_argv[0];
		while (*arg != '\0')
			*new++ = *arg++;
	}

	else if (argn < (cmdline_argc-1)) {
		n = strlen(cmdline_argv[argn + cmdline_shift]);
		if (n > space_left)
			return ENOROOM;
		arg = cmdline_argv[argn + cmdline_shift];
		while (*arg != '\0')
		       *new++ = *arg++;	
	}

	*oldp = (old - 1);
	*newp = new;

	return n;
}

static int expand_argc(char **newp, int space_left)
{
	char argc_buffer[10], *new = *newp;
	int n = snprintf(argc_buffer, 10, "%d", cmdline_argc - cmdline_shift);
	int i = 0;

	if (n > space_left)
		return ENOROOM;
	while (argc_buffer[i] != '\0')
		*new++ = argc_buffer[i++];

	*newp = new;
	return n;
}

int expand(char *old, char *new, int newsize)
{
	int dollar_flag = 0;
	int i = 0, rv, brace_flag;

	for ( ; *old != '\0'; old++) {
		if (dollar_flag) {
		        if ((*old) == '$') {
				if ((rv = expand_pid(&new, newsize - i)) > 0)
					i += rv;
			}
			else if ((*old) == ' ') {
				*new++ = '$';
				i++;
			}
			else if (isdigit(*old)) { 
				if ((rv = expand_argv(&old, &new, newsize - i)) > 0)
					i += rv;
			}
			else if ((*old) == '#') {
				if ((rv = expand_argc(&new, newsize - i)) > 0)
				i += rv;
			}
			else {
				brace_flag = ((*old) == '{');
				if ((rv = expand_env(&old, &new, newsize - i, brace_flag)) > 0) 
					i += rv;
			}
			dollar_flag = 0;
		}

		else {
			if (*old == '$')
				dollar_flag = 1;
			else {
				*new++ = *old;
				i++;
			}
		}

		if (i >= newsize)
			return ENOROOM;
	}
	
	*new = '\0';
	return 0;
}
