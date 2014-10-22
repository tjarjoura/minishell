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


/* Constants */ 

#define LINELEN 1024

/* Prototypes */

void processline (char *line);
char **argparse(char *line, int *sz);

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
	int    status;
	int    sz;
	char **argv = argparse(line, &sz);

	/* Start a new process to do the job. */
	cpid = fork();
	if (cpid < 0) {
		perror ("fork");
		return;
	}

	/* Check for who we are! */
	if (cpid == 0) {
		/* We are the child! */
		execvp (line, argv);
		perror ("exec");
		exit (127);
	}

	if (argv)
		free(argv); 
	/* Have the parent wait for child to complete */
	if (wait (&status) < 0)
		perror ("wait");
}

char **argparse(char *line, int *sz)
{
	int i, j, wc, in_arg; 
	char delimiter = ' ';
	char **argv, c;

	in_arg = wc = i = 0;

	/* get wc and fill in spaces with null bytes */
	while ((c = line[i]) != '\0') {
		if (c == delimiter && in_arg) {
			line[i] = '\0';
			in_arg = 0;
		}

		else if (c != delimiter && !in_arg) {
			wc++;
			in_arg = 1;
		}	

		i++;
	}
	
	if (wc)
		argv = malloc(sizeof(char *) * (wc + 1));
	else
		argv = NULL;

	i = j = 0;

	for (j = 0; j < wc; j++) {
		/* skip initial whitespace */
		while (line[i] == delimiter) 
			i++;

		argv[j] = (line + i);
		
		/* skip past terminating null byte */
		while (line[i] != '\0')
			i++;
		i++;
	}

	(*sz) = wc;
	argv[wc] = NULL;
	return argv;
}
