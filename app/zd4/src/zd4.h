#ifndef __ZD4_H__
#define __ZD4_H__

#ifdef __cplusplus
#include <list>
#include <zd.hpp>

struct zd4_arguments
{
    const char            *input;
    std::list<zd::ustring> inc_dirs;

    zd4_arguments(int argc, char *argv[]);
};

extern "C"
{
#endif

    extern void
    zd4_clear(void);

    extern int
    zd4_list_zdi(void);

#ifdef __cplusplus
}
#endif

#endif
