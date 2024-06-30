#include "zd4.h"

#if defined(__ia16__)

#define _BORLANDC_SOURCE
#include <conio.h>

void
zd4_clear(void)
{
    clrscr();
}

#elif defined(_WIN32)

#include <windows.h>

void
zd4_clear(void)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO console_buffer;
    if (!GetConsoleScreenBufferInfo(console, &console_buffer))
    {
        return;
    }

    DWORD console_size = console_buffer.dwSize.X * console_buffer.dwSize.Y;
    COORD position = {0, 0};
    DWORD ch_count;
    if (!FillConsoleOutputCharacterW(console, L' ', console_size, position,
                                     &ch_count))
    {
        return;
    }

    if (!FillConsoleOutputAttribute(console, console_buffer.wAttributes,
                                    console_size, position, &ch_count))
    {
        return;
    }

    SetConsoleCursorPosition(console, position);
}

#else

#include <stdio.h>

void
zd4_clear(void)
{
    fputs("\x1B[2J", stdout);
}

#endif
