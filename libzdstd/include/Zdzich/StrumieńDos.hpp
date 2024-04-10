#pragma once

#include <map>

#include <zd/sysio.hpp>

#include <Zdzich/Typy.hpp>

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
