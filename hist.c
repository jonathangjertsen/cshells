#include <stdio.h>

#ifdef _WIN32
    #define HISTORY "doskey /history"
#else
    #define HISTORY "history"
#endif

int main(int argc, char **argv)
{
    return puts(HISTORY);
}
