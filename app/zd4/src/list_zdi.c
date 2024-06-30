#include <stdio.h>
#include <sys/stat.h>

#include "zd4.h"

#if defined(__ia16__)

#include <dos.h>

int
zd4_list_zdi(void)
{
    puts("");

    struct find_t find_buff;
    if (0 != _dos_findfirst("*.zdi", _A_NORMAL, &find_buff))
    {
        return 0;
    }

    do
    {
        printf("%s\t%u\n", find_buff.name, find_buff.size);
    } while (0 == _dos_findnext(&find_buff));

    return 0;
}

#elif defined(_WIN32)

#include <windows.h>

int
zd4_list_zdi(void)
{
    puts("");

    HANDLE           file = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW find_buff;
    if (INVALID_HANDLE_VALUE == (file = FindFirstFileW(L"*.zdi", &find_buff)))
    {
        return 0;
    }

    do
    {
        if (find_buff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }

        printf("%ls\t%u\n", find_buff.cFileName, find_buff.nFileSizeLow);
    } while (FindNextFileW(file, &find_buff));

    return 0;
}

#else

#include <glob.h>

int
zd4_list_zdi(void)
{
    puts("");

    glob_t glob_buff = {0};
    glob("*.zdi", GLOB_DOOFFS, 0, &glob_buff);
    for (size_t i = 0; i < glob_buff.gl_pathc; i++)
    {
        struct stat info;
        if (0 != stat(glob_buff.gl_pathv[i], &info))
        {
            continue;
        }

        if (!S_ISREG(info.st_mode))
        {
            continue;
        }

        printf("%s\t%u\n", glob_buff.gl_pathv[i], (unsigned)info.st_size);
    }

    return 0;
}

#endif
