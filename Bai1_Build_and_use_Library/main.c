#include <stdio.h>
#include "strutils.h"

int main() {
    char s1[] = "   Hello World   ";
    str_trim(s1);
    printf("Trimmed: '%s'\n", s1);

    str_reverse(s1);
    printf("Reversed: '%s'\n", s1);

    printf("String to int: %d\n", str_to_int("12345"));
    char s2[11];
    str_copy_safe(s2, "Hello world!", sizeof(s2));
    printf("Copied safely: '%s'\n", s2);

    return 0;
}

