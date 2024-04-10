#pragma once

#include <fcntl.h>
#ifdef _WIN32
#include <conio.h>
#include <io.h>

#define open   _open
#define close  _close
#define write  _write
#define O_RDWR _O_RDWR
#else
#include <cstdio>
#include <termios.h>
#include <unistd.h>

static int
_getch()
{
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);

    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    int ch = std::getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}
#endif
