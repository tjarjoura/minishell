A minishell written in C.

### Current Features:

* pass arguments to commands
* environment variable expansion
* current pid expansion
* builtin commands

### Builtin Commands:
1. exit
	* exits the shell
2. aecho [arg1] [arg2]...
	* echos arguments passed to it, seperated by spaces
	* optional [-n] switch to determine whether or not a newline will be printed at the end
3. envset <name> <value>
	* sets an environment variable with the given name to the given value
4. envunset [name]
	* removes the environment variable with the given name
5. cd [dir]
	* changes the working directory to dir(relative or abolute pathname) or the user's home directory, if no argument given
