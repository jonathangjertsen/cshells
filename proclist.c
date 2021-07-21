#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "shell_utils.h"

#ifdef _WIN32

#define COLNO_NAME 0
#define COLNO_PID 1
#define COLNO_MEM 4

const char mem_usage_descriptions[][4] = { " B", "kB", "MB", "GB", "TB" };

typedef struct {
    char name[400];
    int pid;
    long long int mem_usage;
} proc_t;

typedef struct {
    char name[400];
    int n_pids;
    long long int mem_usage;
} program_t;

typedef struct {
    program_t programs[1000];
    int size;
} program_list_t;

void proc_update_item(proc_t *proc, line_buffer item, int item_len, int column)
{
    switch (column)
    {
        case COLNO_NAME:
            strncpy(proc->name, item, sizeof(proc->name)-1);
            break;
        case COLNO_PID:
            proc->pid = strtol(item, 0, 10);
            break;
        case COLNO_MEM:
            line_buffer mem_usage = {};
            int mem_usage_len = 0;
            for (int j = 0; j < item_len; j++)
            {
                switch (item[j])
                {
                    case 'K':
                    case ',':
                        break;
                    default:
                        mem_usage[mem_usage_len] = item[j];
                        mem_usage_len++;
                        break;
                }
            }
            proc->mem_usage = strtol(mem_usage, 0, 10) * 1024LL;
            break;
    }
}

int main(int argc, char **argv)
{
    bool next_is_threshold_mb = false;
    long int threshold_mb = 0;
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "--threshold") == 0))
        {
            next_is_threshold_mb = true;
        }
        else
        {
            if (next_is_threshold_mb)
            {
                threshold_mb = strtol(argv[i], 0, 10);
                next_is_threshold_mb = false;
            }
        }
    }
    long long int threshold_b = threshold_mb * 1024 * 1024;

    program_list_t prog_list = {};
    FILE *fp = popen("tasklist /FO csv", "r");
    line_buffer line = {};
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int line_len = strlen(line);
        line_buffer item = {};
        int item_len = 0;
        bool in_item = false;
        int column = 0;
        proc_t proc = {};
        for (int i = 0; i < line_len; i++)
        {
            char curr = line[i];
            if (in_item)
            {
                if (curr == '"')
                {
                    in_item = false;
                    proc_update_item(&proc, item, item_len, column);
                    memset(item, 0, sizeof(item));
                    item_len = 0;
                    column++;
                }
                else
                {
                    item[item_len] = curr;
                    item_len++;
                }
            }
            else
            {
                if (curr == '"')
                {
                    in_item = true;
                }
            }
        }

        bool found = false;
        for (int i = 0; i < prog_list.size; i++)
        {
            if (strcmp(prog_list.programs[i].name, proc.name) == 0)
            {
                prog_list.programs[i].n_pids++;
                prog_list.programs[i].mem_usage += proc.mem_usage;
                found = true;
                break;
            }
        }
        if (!found)
        {
            prog_list.programs[prog_list.size].n_pids = 1;
            prog_list.programs[prog_list.size].mem_usage = proc.mem_usage;
            memcpy(
                prog_list.programs[prog_list.size].name,
                proc.name,
                sizeof(prog_list.programs[prog_list.size].name)
            );
            prog_list.size++;
            if (prog_list.size >= ARRAY_SIZE(prog_list.programs))
            {
                printf("WARNING: too many processes, some were skipped\n");
                break;
            }
        }
    }

    printf(
        "+----------------------------------------+---------+-----------+\n"
        "|                                Program |   Usage | Processes |\n"
        "+----------------------------------------+---------+-----------+\n"
    );
    for (int i = 0; i < prog_list.size; i++)
    {
        int usage_char_no = 0;
        long long int usage = prog_list.programs[i].mem_usage;

        if (usage < threshold_b)
        {
            continue;
        }

        while (usage > 1024 && usage_char_no < ARRAY_SIZE(mem_usage_descriptions))
        {
            usage /= 1024;
            usage_char_no++;
        }

        printf(
            "|%39s | %4lld %s | %9d |\n",
            prog_list.programs[i].name,
            usage,
            mem_usage_descriptions[usage_char_no],
            prog_list.programs[i].n_pids
        );
    }
    printf("+----------------------------------------+---------+-----------+\n");

    return -1;
}
#else
int main(int argc, char **argv)
{
    printf("Not implemented\n");
    return -1;
}
#endif
