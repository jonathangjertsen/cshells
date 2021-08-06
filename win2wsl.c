/**
 * To convert a path from Windows to the corresponding location in WSL
 * and put it into the clipboard, run:
 *
 *     win2wsl <path> | clip
 */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "shell_utils.h"

#define MOUNT_POINT "/mnt/"

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        line_buffer wsl = { 0 };
        int index_wsl = 0;

        line_buffer windows = { 0 };
        strncpy(windows, argv[i], sizeof(windows));
        int index_windows = 0;
        int len_windows = strlen(windows);

        bool is_abs_path = windows[1] == ':';
        if (is_abs_path)
        {
            char drive_letter_part[] = { tolower(windows[0]), '/', '\0' };
            strcat(wsl, MOUNT_POINT);
            strcat(wsl, drive_letter_part);
            index_wsl = strlen(wsl);
            index_windows = 3;
        }

        while (index_windows < len_windows)
        {
            char c = windows[index_windows];
            if (c == '\\')
            {
                c = '/';
            }
            wsl[index_wsl] = c;
            index_wsl++;
            index_windows++;
        }

        puts(wsl);
    }
}
