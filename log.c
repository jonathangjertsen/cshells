#include <stdlib.h>
#include <stdio.h>

#include "shell_utils.h"

int main(int argc, char **argv)
{
    long int max_n_lines = 0L;
    if (argc > 1)
    {
        max_n_lines = strtol(argv[1], 0, 10);
    }
    if ((max_n_lines == 0) || (max_n_lines > 1000))
    {
        max_n_lines = 10;
    }
    return head("git log --oneline --color --graph", max_n_lines);
}
