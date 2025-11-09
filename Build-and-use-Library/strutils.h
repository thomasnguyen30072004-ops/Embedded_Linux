#ifndef STRUTILS_H
#define STRUTILS_H

void str_reverse(char *s);
void str_trim(char *s);
void str_copy_safe(char *dest, const char *src, int max_len);
int str_to_int(const char *s);

#endif

