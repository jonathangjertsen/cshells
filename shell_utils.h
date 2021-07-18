#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include <ctype.h>
#include <string.h>

#define MAX_LINE_SIZE 2048

typedef char line_buffer[MAX_LINE_SIZE];

static inline void trim_trailing_whitespace(char *string)
{
    int length = strlen(string);
    for (int i = length-1; i >= 0; i--)
    {
        if (isspace(string[i]))
        {
            string[i] = '\0';
        }
        else
        {
            break;
        }
    }
}

#endif
