#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "proto.h"

int expand(char *old, char *new, int newsize)
{
	int old_idx, new_idx, env_idx, dollar, in_env, n, i;
	pid_t pid;
	char env_buffer[MAX_ENV], pid_buffer[20], *env_val;

	new_idx = dollar = in_env = 0;
	for (old_idx = 0; old[old_idx] != '\0'; old_idx++) {
		if (in_env) {
			if (old[old_idx] == '}') {
				/* end of environment variable name, copy the value to the new string */
				env_buffer[env_idx] = '\0';
				env_val = getenv(env_buffer);
				if (env_val != NULL) {
					n = strlen(env_val);
					if ((n + new_idx) >= newsize)
						return -1;
					for (env_idx = 0; env_idx < n; env_idx++)
						new[new_idx++] = env_val[env_idx];
				}
				in_env = 0;
			}

			else if (old[old_idx] == '\0')
				return -1;
			else {
				env_buffer[env_idx++] = old[old_idx]; 	
				if (env_idx >= MAX_ENV)
					return -1;
			}		
		}

		else {
			if (dollar) {
				if (old[old_idx] == '{') {
					in_env = 1;
					env_idx = 0;
					dollar = 0;
				}

				else if (old[old_idx] == '$') {
					pid = getpid();
					snprintf(pid_buffer, 20, "%d", pid);
					for (i = 0; pid_buffer[i] != '\0'; i++)
						new[new_idx++] = pid_buffer[i];
				}					
				else {
					dollar = (old[old_idx] == '$');
					new[new_idx++] = '$';
					if (!dollar)
						new[new_idx++] = old[old_idx];
				}
			}
			
			else {
				if (old[old_idx] == '$')
					dollar = 1;
				else
					new[new_idx++] = old[old_idx];
			}
		}
	}

	new[new_idx] = '\0';	
	return 0;
}
