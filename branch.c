#include <stdlib.h>

int main(int argc, char **argv)
{
    return system("git rev-parse --abbrev-ref HEAD");
}
