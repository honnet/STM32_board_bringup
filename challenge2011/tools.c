#include "tools.h"

///////////////////////////////////////////////////////////////
uint32_t atoi(char* s)
{
    uint32_t val = 0;
    while (*s != '\0')                                  // check if end of string
    {
        val *= 10;                                      // "decimal left shift"
        val += *s - '0';                                // remove the ASCII offset
        s++;                                            // next caracter in the string
    }
    return val;
}

///////////////////////////////////////////////////////////////
int baseX2int(char c)
{
    int val = 34;

    if ( c >= '0' && c <= '9' ) // is it a base 10 number ?
    {
        val = (c - '0');
    }
    else
    if ( c >= 'A' && c <= 'W' ) // is it a base 16 or 33 value ?
    {
        val = (c - 'A' + 10);
    }

    return val;
}

///////////////////////////////////////////////////////////////
void itoa(uint16_t x, char* s) //only for the 12bits ADC!
{
    uint8_t i = 3;
    strcpy(s , "0000");

    while (x != 0)
    {
        s[i] = x % 10;
        x = (uint16_t)(x / 10);
        i--;
    }
}

///////////////////////////////////////////////////////////////
void wait_us(int x)
{
    int i, j;
    for(i=0; i<x; i++)
        for(j=0; j<50; j++);
}

