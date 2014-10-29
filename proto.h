/* Constants */ 

#define LINELEN 1024
#define MAX_ENV 50
/* Prototypes */

void processline(char *line);
int argparse(char *line, char ***argvp);
int builtin(char *cmd, int argc, char **argv);
int expand(char *old, char *new, int newsize);
