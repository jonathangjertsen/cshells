#include <stdlib.h>
#include <string.h>

#include "shell_utils.h"

int main(int argc, char **argv)
{
    const char *navdir = getenv("NAVDIR");
    if (navdir)
    {
        line_buffer command = {};
        strcat(command, "cd \"");
        strcat(command, navdir);
        strcat(command, "/");
        for (int i = 1; i < argc; i++)
        {
            strcat(command, argv[i]);
        }
        strcat(command, "\"");
        puts(command);
    }
    else
    {
        return 1;
    }
}
