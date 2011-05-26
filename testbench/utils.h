#ifndef UTILS_H
# define UTILS_H

short sfabs(short i);
void reverse(char s[]);
void itoa(int n, char s[]);
int only_numbers(const char* str, char eol);
int str_to_int(const char* str, char eol);
int char33_to_int(char c);
char hex_to_char(char hex);
void str_to_hex_str(char* hex_str, const char* str, int limit);
int atoi(char* s);

#endif
