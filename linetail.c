#include <stdio.h>

#include "shell_utils.h"

int main(int argc, char **argv)
{
    ensure(argc == 2, "Need exactly 1 argument (number of characters)");
    int n = strtol(argv[1], 0, 10);
    ensure(n >= 1, "Invalid length argument");
    line_buffer lb = {};
    while (fgets(lb, sizeof(lb), stdin))
    {
        trim_trailing_whitespace(lb);
        int length = strlen(lb);
        int start = length - n;
        if (start < 0)
        {
            start = 0;
        }
        printf(lb + start);
        printf("\n");
    }
    return 0;
}
