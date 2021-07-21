#include <stdio.h>

#include "shell_utils.h"

int main(int argc, char **argv)
{
    line_buffer lb = {};
    int lineno = 1;
    while (fgets(lb, sizeof(lb), stdin))
    {
        printf("%6d | %s", lineno, lb);
        lineno++;
    }
    return 0;
}
