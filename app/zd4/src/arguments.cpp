#include "zd4.h"

zd4_arguments::zd4_arguments(int argc, char *argv[])
    : input{nullptr}, inc_dirs{}
{
    for (int i = 1; i < argc; i++)
    {
        if ('/' != argv[i][0])
        {
            input = argv[i];
            continue;
        }

        if ('I' == argv[i][1])
        {
            inc_dirs.push_back(argv[i] + 2);
            continue;
        }
    }
}
