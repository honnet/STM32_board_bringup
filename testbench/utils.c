#include <string.h>

#include "utils.h"

short sfabs(short i)
{
  return (i > 0) ? i : -i;
}

/* Source: Wikipédia */
/* reverse:  reverse string s in place */
void reverse(char s[])
{
  int i, j;
  char c;
  for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
  {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

/* Source: Wikipédia */
/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
  int i, sign;

  if ((sign = n) < 0)       /* record sign */
    n = -n;                 /* make n positive */
  i = 0;
  do                        /* generate digits in reverse order */
  {
    s[i++] = n % 10 + '0';  /* get next digit */
  } while ((n /= 10) > 0);  /* delete it */
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}

int only_numbers(const char* str, char eol)
{
  int i = 0;
  while (str[i] != eol)
  {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    i++;
  }
  return 1;
}

int str_to_int(const char* str, char eol)
{
  int value = 0, i = 0, tmp;
  while (str[i] != eol)
  {
    tmp = str[i];
    if (str[i] <= '9' && str[i] >= '0')
      tmp -= '0';
    else if (str[i] <= 'F' && str[i] >= 'A')
      tmp = tmp - 'A' + 10;
    else if (str[i] <= 'f' && str[i] >= 'a')
      tmp = tmp - 'a' + 10;

    value = 16 * value + tmp;
    i++;
  }
  return value;
}

int char33_to_int(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'W')
    return c - 'A' + 10;
  else if (c >= 'a' && c <= 'w')
    return c - 'a' + 10;
  return 0;
}

char hex_to_char(char hex)
{
  if (hex >= 0 && hex < 10)
    return hex + '0';
  else if (hex >= 10 && hex < 16)
    return hex - 10 + 'A';
  return 0;
}

void str_to_hex_str(char* hex_str, const char* str, int limit)
{
  int i = 0;
  while (str[i] != 0)
  {
    if (2 * i + 1 > limit)
      return;
    hex_str[2 * i + 0] = hex_to_char((str[i] & 0xf0) >> 4);
    hex_str[2 * i + 1] = hex_to_char(str[i] & 0x0f);
    i++;
  }
}

int atoi(char* s)
{
    int val = 0;
    int sign = 1;

    if (*s == '-')
    {
        sign = -1;
        ++s;
    }

    while (*s != '\0')             // check if end of string
    {
        val *= 10;                 // "decimal left shift"
        val += *s - '0';           // remove the ASCII offset
        ++s;                       // next caracter in the string
    }

    return val * sign;
}


