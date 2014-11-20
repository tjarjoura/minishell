#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "proto.h"

/* remove every instance of a character c from a string */
static void remove_char(char *str, char c, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		if (str[i] == c) {
			memmove(str+i, str+i+1, n - i);
			n--;
		}
	}
}

/* argparse with quotes */
int argparse(char *line, char ***argvp)
{
	int in_arg, in_quotes, quotes_cnt, i, j, wc;
	char c;
	char delimiter = ' ';
	int n = strlen(line);

	in_arg = quotes_cnt = i = wc = 0;
	for (i = 0; i < n; i++) {
		c = line[i];
		in_quotes = (quotes_cnt % 2);
		
		if (c == '\"')
			quotes_cnt++;
		else if (c == delimiter && in_arg && !in_quotes) {
			line[i] = '\0';
			in_arg = 0;
		}
		else if (c != delimiter && !in_arg) {
			in_arg = 1;
			wc++;
		}
	}
		
		if (quotes_cnt % 2 != 0) 
			return -1;

		remove_char(line, '\"', n);

		*(argvp) = malloc((wc + 1) * sizeof(char *));

		i = j = 0;
		for (j = 0; j < wc; j++) {
			while (line[i] == delimiter)
				i++;

			*(*(argvp) + j) = (line + i);
			
			while (line[i] != '\0')
				i++;
			i++;
		}
		
		*(*(argvp) + wc) = NULL;		
		return wc;
}
