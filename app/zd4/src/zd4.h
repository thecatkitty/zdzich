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

#define MSG_ERROR         0x8000
#define MSG_HEADER        0x8010
#define MSG_COPYRIGHT     0x8011
#define MSG_INPUT_PROMPT  0x8020
#define MSG_SUMMARY_INOUT 0x8030
#define MSG_SUMMARY_CODE  0x8031
#define MSG_SUMMARY_DATA  0x8032

#endif
