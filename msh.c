/* CS 352 -- Mini Shell!  
 *
 *   Sept 21, 2000,  Phil Nelson
 *   Modified April 8, 2001 
 *   Modified January 6, 2003
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proto.h"

/* Shell main */

int main (void)
{
	char   buffer [LINELEN];
	int    len;

	while (1) {

		/* prompt and get line */
		fprintf (stderr, "%% ");
		if (fgets (buffer, LINELEN, stdin) == NULL)
			break;

		/* Get rid of \n at end of buffer. */
		len = strlen(buffer);
		if (buffer[len-1] == '\n')
			buffer[len-1] = 0;

		/* Run it ... */
		processline (buffer);

	}

	if (!feof(stdin))
		perror ("read");

	return 0;		/* Also known as exit (0); */
}

void processline (char *line)
{
	pid_t  cpid;
	int    status, builtin;
	char **argv = NULL;
	int argc = argparse(line, &argv);

	if (argc == -1) {
		printf("Error.\n");
		return;
	}
	
	else if (argc == 0)
		return;

	/* check if it's builtin */
	builtin = get_builtin(argv[0]);
	if (builtin >= 0) {
		run_builtin(builtin, argc, argv);	
		return;
	}

	/* Start a new process to do the job. */
	cpid = fork();
	if (cpid < 0) {
		perror ("fork");
		return;
	}

	/* Check for who we are! */
	if (cpid == 0) {
		/* We are the child! */
		execvp (argv[0], argv);
		perror ("exec");
		exit (127);
	}

	if (argv)
		free(argv); 
	/* Have the parent wait for child to complete */
	if (wait (&status) < 0)
		perror ("wait");
}
