#include "shell_utils.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CHAR2(A, B) (A | ((uint16_t)B) << 8)

void diff(char *filename)
{
    printf(
        "git diff:\n"
        "---------\n"
    );
    line_buffer command = {};
    snprintf(command, sizeof(command), "git diff -- \"%s\"", filename);
    system(command);
    printf(
        "---------\n"
        "\n"
        "git diff --cached:\n"
        "------------------\n"
    );
    snprintf(command, sizeof(command), "git diff --cached -- \"%s\"", filename);
    system(command);
    printf(
        "------------------\n"
    );
}

void make_message(void)
{
    printf(
        "git status:\n"
        "-----------\n"
    );
    system("git status");
    printf(
        "-----------\n"
        "Commit message [leave empty to skip] > "
    );

    line_buffer message = {};
    fgets(message, sizeof(message), stdin);

    trim_trailing_whitespace(message);
    if (message[0] == '\0')
    {
        printf("OK, skipping commit\n");
        return;
    }

    line_buffer command = {};
    snprintf(command, sizeof(command), "git commit -m \"%s\"", message);
    if (system(command))
    {
        printf("Sorry, git commit didn't work\n");
    }
}

int main(int argc, char **argv)
{
    FILE *fp = popen("git status --porcelain", "r");
    if (!fp)
    {
        printf("Couldn't run git status --porcelain\n");
        return -1;
    }

    line_buffer line;
    bool might_have_something_to_commit = false;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // Get status
        char *status = strtok(line, " ");
        if (!status)
        {
            continue;
        }

        // Get filename
        char *filename = strtok(0, " ");
        if (!filename)
        {
            continue;
        }
        trim_trailing_whitespace(filename);

        // Determine next action based on status
        bool handled = false;
        uint16_t status_code = CHAR2(status[0], status[1]);
        switch (status_code)
        {
            case CHAR2('A', ' '):
            case CHAR2('A', '\0'):
                printf("%s -- Added, not modified. Skipping\n", filename);
                handled = true;
                break;
            case CHAR2('M', ' '):
            case CHAR2('M', '\0'):
            case CHAR2('M', 'M'):
            case CHAR2('A', 'M'):
            case CHAR2('?', '?'):
                printf("%s -- Modified.\n", filename);
                break;
            default:
                printf("%s -- I don't know this status: (%d, %d), statuscode %d, status='%s'\n", filename, status[0], status[1], status_code, status);
                break;
        }

        while (!handled)
        {
            might_have_something_to_commit = true;

            // Prompt for next action
            printf("%s -- [A]dd, [P]atch, [D]iff, [S]kip, abor[T], [M]essage | ", filename);

            // Read action
            line_buffer user_input = {};
            fgets(user_input, sizeof(user_input), stdin);

            switch (user_input[0])
            {
                case 'a':
                case 'A':
                {
                    line_buffer command = {};
                    snprintf(command, sizeof(command), "git add %s", filename);
                    if (system(command))
                    {
                        printf("Sorry, that didn't work. Skipping\n");
                    }
                    handled = true;
                }
                    break;
                case 'p':
                case 'P':
                {
                    line_buffer command = {};
                    snprintf(command, sizeof(command), "git add --patch %s", filename);
                    if (system(command))
                    {
                        printf("Sorry, that didn't work. Skipping\n");
                    }
                    handled = true;
                }
                    break;
                case 'd':
                case 'D':
                {
                    diff(filename);
                    handled = false;
                }
                    break;
                case 'm':
                case 'M':
                    goto message_writing_part;
                case 's':
                case 'S':
                    handled = true;
                    break;
                case 't':
                case 'T':
                    goto cleanup;
                    break;
                default:
                    printf("I don't understand %s\n", user_input);
                    break;
            }
            printf("\n");
        }
    }

message_writing_part:
    if (might_have_something_to_commit)
    {
        make_message();
    }
    else
    {
        printf("Nothing to commit.\n");
    }

cleanup:
    if (fp)
    {
        pclose(fp);
    }
    printf("\n");
    return 0;
}
