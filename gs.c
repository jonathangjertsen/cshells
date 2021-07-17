#include <stdlib.h>

int main(int argc, char **argv)
{
    int status = system("git status\n");
    return status;
}
