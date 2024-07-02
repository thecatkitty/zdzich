#include "zd4.h"

static void
_process_arg(const char *arg, zd4_arguments *args)
{
    if ('/' != arg[0])
    {
        args->input = arg;
        return;
    }

    if ('I' == arg[1])
    {
        args->inc_dirs.push_back(arg + 2);
        return;
    }
}

zd4_arguments::zd4_arguments(int argc, char *argv[])
    : input{nullptr}, inc_dirs{}
{
    auto cfg = std::fopen("zd.cfg", "r");
    if (cfg)
    {
        char buff[PATH_MAX];
        while (std::fgets(buff, PATH_MAX, cfg))
        {
            auto len = std::strlen(buff);
            if ('\n' == buff[len - 1])
            {
                buff[len - 1] = 0;
            }
            _process_arg(buff, this);
        }
        std::fclose(cfg);
    }

    for (int i = 1; i < argc; i++)
    {
        _process_arg(argv[i], this);
    }
}
