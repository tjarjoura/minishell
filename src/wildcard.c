#include <stdio.h>

int is_match(char *str, char *ptrn)
{
	char *star = NULL;
	char *saved = NULL;

	while (*str != '\0') {
		if ((*str == *ptrn) || (*ptrn == '?')) {
			str++;
			ptrn++;
		} else if (*ptrn == '*') {
			star = ptrn++;
			saved = str;
		} else if (star != NULL) {
			ptrn = star + 1;
			str = ++saved;
		} else {
			return 0;
		}
	}

	while (*ptrn == '*')
		ptrn++;

	return (*ptrn == '\0');
}
