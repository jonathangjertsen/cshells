#include <stdio.h>

#include "shell_utils.h"

static const char BREAKING_POINTS[] = ".,-:; \t";

static bool is_breaking_point(char c)
{
    for (int j = 0; j < ARRAY_SIZE(BREAKING_POINTS); j++)
    {
        if (c == BREAKING_POINTS[j])
        {
            return true;
        }
    }
    return false;
}

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
        int cursor = 0;
        while (cursor + n <= length)
        {
            line_buffer out = {};
            int breaking_point = -1;
            for (int i = n - 1; i >= 0; i--)
            {
                if (is_breaking_point(lb[cursor+i]))
                {
                    breaking_point = i + 1;
                    break;
                }
            }
            bool force_break = (breaking_point == -1);
            if (force_break)
            {
                breaking_point = n - 1;
            }
            strncpy(out, lb + cursor, breaking_point);
            printf(out);
            if (force_break)
            {
                printf("-");
            }
            printf("\n");
            cursor += breaking_point;
            while (lb[cursor] == ' ')
            {
                cursor++;
            }
        }
        printf(lb + cursor);
        printf("\n");
    }
    return 0;
}
