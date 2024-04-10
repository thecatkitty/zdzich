#pragma once

#include <map>

#include <Zdzich/Typy.hpp>

#include <fcntl.h>
#ifdef _WIN32
#include <conio.h>
#include <io.h>

#define open   _open
#define close  _close
#define write  _write
#define O_RDWR _O_RDWR
#else
#include <unistd.h>
#endif

namespace Zdzich
{

class StrumieńDos
{
    int _uchwyt;

  public:
    StrumieńDos() : _uchwyt{-1}
    {
    }

    StrumieńDos(Bajt uchwyt) : _uchwyt{uchwyt}
    {
    }

    void
    Otwórz(const Tekst &nazwa, Bajt tryb = O_RDWR)
    {
        _uchwyt = open(nazwa.c_str(), tryb);
    }

    void
    Pisz(const Tekst &tekst = "\n")
    {
        write(_uchwyt, tekst.c_str(), tekst.length());
    }

    void
    PiszL(const Tekst &tekst = "")
    {
        Pisz(tekst);
        Pisz();
    }

    void
    Zamknij()
    {
        close(_uchwyt);
        _uchwyt = -1;
    }
};

std::map<Bajt, StrumieńDos> Strumienie;

} // namespace Zdzich
