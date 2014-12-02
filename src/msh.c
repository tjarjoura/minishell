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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proto.h"

/* Shell main */

int cmdline_shift;
int cmdline_argc;
char **cmdline_argv;

int prev_status;

int main(int argc, char **argv)
{
	char buffer [LINELEN], *prompt;
	int len, interactive;
	FILE* input_stream;

	cmdline_shift = 1;	
	cmdline_argc = argc;
	cmdline_argv = argv;
	prev_status = 0;

	if (argc > 1) {
		if((input_stream = fopen(argv[1], "r")) == NULL) {
			perror("fopen");
			exit(0);
		}

		interactive = 0;

	} else {
		interactive = 1;
		input_stream = stdin;

	}

	while (1) {
		/* prompt and get line */
		if (interactive) {
            if ((prompt = getenv("P1")) != NULL)
                fprintf(stdout, "%s ", prompt);
            else
			    fprintf (stdout, "%% ");
        }
		
        if (fgets (buffer, LINELEN, input_stream) == NULL)
			break;

		/* Get rid of \n at end of buffer. */
		len = strlen(buffer);
		if (buffer[len-1] == '\n')
			buffer[len-1] = '\0';

		/* Run it ... */
        if (buffer[0] != '#')
		    processline (buffer);
	}

	if (!feof(input_stream))
		perror ("read");
    putchar('\n');

	return 0;		/* Also known as exit (0); */
}

static int pipes(char *line, char ***commands)
{
    int i, j, in_quote = 0, commandc = 1;

    for (i = 0; line[i] != '\0'; i++) {
        if (line[i] == '\"')
            in_quote = !in_quote;

        else if ((line[i] == '|') && !in_quote) {
            commandc++; 
            line[i] = '\0';
        }
    }

    *(commands) = malloc(sizeof(char *) * (commandc + 1));

    i = 0;
    for (j = 0; j < commandc; j++) {
        *(*(commands) + j) = line + i; 

        while (line[i] != '\0')
            i++;
        i++;
    }

    *(*(commands) + commandc) = NULL;
    return commandc;
}


static int redirect(char **argv, int *infile, int *outfile, int *errfile)
{
    int i, fd;

    for (i = 0; argv[i] != NULL; i++) {
        if (strncmp(">", argv[i], 2) == 0) {
            if ((fd = open(argv[i+1], 
                           O_WRONLY | O_CREAT, 
                           S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR)) < 0) {
                perror("open");
                return -1;
            }
            
            *outfile = fd;

        } else if (strncmp("<", argv[i], 2) == 0) {
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) {
                perror("open");
                return -1;
            }

            *infile = fd;

        } else if (strncmp("2>", argv[i], 3) == 0) {
           if ((fd = open(argv[i+2],
                          O_WRONLY | O_CREAT,
                          S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR)) < 0) {
               perror("open");
               return -1;
           }

           *errfile = fd;
        }
    }

    return 0;
}

void processline(char *line)
{
	pid_t cpid;
	int argc, commandc, rv, i, pipe_open;
	int pipe_fds[2];
    char **commands;
    char **argv = NULL;; 
	char expanded_line[LINELEN];

    /* get copies of stdin, stdout, and stderr file descriptors */
    const int stdin_orig = dup(STDIN_FILENO);
    const int stdout_orig = dup(STDOUT_FILENO);
    const int stderr_orig = dup(STDERR_FILENO);
    int infile = stdin_orig, outfile = stdout_orig, errfile = stderr_orig;

	if (expand(line, expanded_line, LINELEN) < 0) {
		printf("Error expanding.\n");
		return;
	}

    if ((commandc = pipes(expanded_line, &commands)) < 0) {
        printf("Error handling pipes\n"); 
        return;
    }
    
    for (i = 0; i < commandc; i++) {
        printf("Processing %s\n", commands[i]);
        if (infile != stdin_orig) {
            close(infile);
            infile = stdin_orig;
        }

        if (outfile != stdout_orig) {
            close(outfile);
            outfile = stdout_orig;
        }

        if (errfile != stderr_orig) {
            close(errfile);
            errfile = stderr_orig;
        }
        
        if (pipe_open)
            infile = pipe_fds[0];

        if ((i + 1) < commandc) {
            if (pipe(pipe_fds) < 0) {
                perror("pipe");
                return;
            }
            
            outfile = pipe_fds[1];
            pipe_open = 1;
        }
    
        if ((argc = argparse(commands[i], &argv)) < 0) { 
		    printf("Error parsing args.\n");
		    return;
	    } else if (argc == 0) /* empty line */
		    continue;

        if (redirect(argv, &infile, &outfile, &errfile) < 0)
            return;

        if ((rv = builtin(argv[0], argc, argv, infile, outfile, errfile)) >= 0) {
            if (argv)
                free(argv);
            prev_status = rv;
        }  
    
        if ((cpid = fork()) < 0) {
            perror ("fork");
            return;
        }
        
        if (cpid == 0) {
            if (dup2(infile, STDIN_FILENO) < 0) {
                perror("stdin dup2");
                return;
            }

            if (dup2(outfile, STDOUT_FILENO) < 0) {
                perror("stdout dup2");
                return;
            }

            if (dup2(errfile, STDERR_FILENO) < 0) {
                perror("stderr dup2");
                return;
            }
            
            execvp(argv[0], argv);
            perror("exec");
            exit(127);
        }
    }   

	if (argv)
		free(argv); 
	
    /* Have the parent wait for child to complete */
	if (wait (&prev_status) < 0)
		perror ("wait");
}
