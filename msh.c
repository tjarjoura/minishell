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
		fprintf (stdout, "%% ");
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
	pid_t cpid;
	int status, argc, rv;
	char **argv = NULL;
	char expanded_line[LINELEN];

	if (expand(line, expanded_line, LINELEN) < 0) {
		printf("Error.\n");
		return;
	}

	if ((argc = argparse(expanded_line, &argv)) < 0) {
		printf("Error.\n");
		return;
	}
	
	if (argc == 0)
		return;

	/* check if it's builtin */
	rv = builtin(argv[0], argc, argv);
	if (rv >= 0) {
		if (argv)
			free(argv);
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
