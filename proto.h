/* Constants */ 

#define LINELEN 1024
#define MAX_ENV 50

/* Error codes */
#define ENOROOM -2
#define ENOENV -3
#define ENOBRACE -4

/* Prototypes */
void processline(char *line);
int argparse(char *line, char ***argvp);
int builtin(char *cmd, int argc, char **argv);
int expand(char *old, char *new, int newsize);
char *get_arg(int idx);
int get_argc();

/* command line arguments */
extern int    cmdline_shift;
extern int    cmdline_argc;
extern char **cmdline_argv;
