#include <string.h>
#include <stdlib.h>
#include "myUtils.h"

char* concat(const char* s1, const char* s2) {
    char* result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int endsWithSlash(char* str) {
	int len = strlen(str);
	char* lastChar = &str[len - 1];
	if (strcmp(lastChar, "/") == 0)
		return 1;
	return 0;
}

char* appendSlash(const char* s1) {
    return concat(s1, "/");
}
