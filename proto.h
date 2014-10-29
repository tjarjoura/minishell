/* Constants */ 

#define LINELEN 1024

/* Prototypes */

void processline(char *line);
int argparse(char *line, char ***argvp);
int get_builtin(char *cmd);
int run_builtin(int idx, int argc, char **argv);
