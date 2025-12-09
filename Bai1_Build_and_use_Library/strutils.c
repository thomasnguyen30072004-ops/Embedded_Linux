#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "strutils.h"

void str_reverse(char *s) {
    int len = strlen(s);
    for (int i = 0; i < len / 2; i++) {
        char temp = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = temp;
    }
}

void str_trim(char *s) {
    char *start = s;
    while (isspace((unsigned char)*start)) start++;

    char *end = s + strlen(s) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';

    memmove(s, start, end - start + 2);
}

int str_to_int(const char *s) {
    return atoi(s);
}

void str_copy_safe(char *dest, const char *src, int max_len) {
    if (dest == NULL || src == NULL || max_len <= 0) return;
    strncpy(dest, src, max_len - 1);
    dest[max_len - 1] = '\0';
}
