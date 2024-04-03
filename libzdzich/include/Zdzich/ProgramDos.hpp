#pragma once

#include <cstdio>
#include <iostream>
#include <map>
#include <thread>

#include <conio.h>
#include <fcntl.h>
#include <io.h>

#include <Zdzich/Rejestry.hpp>
#include <Zdzich/Skoki.hpp>

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
    Otwórz(const Tekst &nazwa, Bajt tryb = _O_RDWR)
    {
        _uchwyt = _open(nazwa.c_str(), tryb);
    }

    void
    Pisz(const Tekst &tekst = "\n")
    {
        _write(_uchwyt, tekst.c_str(), tekst.length());
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
        _close(_uchwyt);
        _uchwyt = -1;
    }
};

std::map<Bajt, StrumieńDos> Strumienie;

int
KolorNaAnsi(Bajt kolor)
{
    const int ansi[16] = {30, 34, 32, 36, 31, 35, 33, 37,
                          90, 94, 92, 96, 91, 95, 93, 97};
    return ansi[kolor & 0xF];
}

int
TłoNaAnsi(Bajt kolor)
{
    const int ansi[16] = {40,  44,  42,  46,  41,  45,  43,  47,
                          100, 104, 102, 106, 101, 105, 103, 107};
    return ansi[kolor >> 4];
}

class ProgramDos
{
  protected:
    // ----- Rejestry -----

    RejestrHL AX, BX, CX, DX;
    Rejestr16 SI, DI;
    Znacznik  C, D, I;

    Rejestr8 &AL = AX.L(), &AH = AX.H();
    Rejestr8 &BL = BX.L(), &BH = BX.H();
    Rejestr8 &CL = CX.L(), &CH = CX.H();
    Rejestr8 &DL = DX.L(), &DH = DX.H();

    CzwórkaRejestrów X1{AX, BX, CX, DX};
    ParaRejestrów    X2{SI, DI};
    ParaRejestrów    X3{X1, X2};

    // ----- Wykaz strumieni -----

    StrumieńDos &
    operator[](Bajt uchwyt)
    {
        if (Strumienie.end() == Strumienie.find(uchwyt))
        {
            Strumienie[uchwyt] = StrumieńDos(uchwyt);
        }

        return Strumienie[uchwyt];
    }

    // ----- Cykl życia programu -----

    void
    Czekaj(Słowo czas)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(czas * 100));
    }

    void
    Koniec()
    {
        std::exit(0);
    }

    // ----- Wejście/wyjście -----

    void
    Czytaj(Tekst &tekst)
    {
        std::getline(std::cin, tekst);
    }

    void
    Pisz(const Tekst &tekst = "\n")
    {
        operator[](1).Pisz(tekst);
    }

    void
    Pisz8(Słowo liczba)
    {
        std::printf("%d", liczba);
    }

    void
    PiszL(const Tekst &tekst = "")
    {
        operator[](1).PiszL(tekst);
    }

    // ----- Konsola -----

    void
    Czyść(Bajt kolor = 0x07)
    {
        char bufor[16];
        std::snprintf(bufor, 16, "\x1B[2J\x1B[%d;%dm", KolorNaAnsi(kolor),
                      TłoNaAnsi(kolor));
        Pisz(bufor);
        Pisz("\x1B[1H");
    }

    void
    Enter()
    {
        while ('\n' != _getch())
            ;
    }

    void
    Esc()
    {
        while ('\x1B' != _getch())
            ;
    }

    void
    EscSpacjaEnter()
    {
        int ch = 0;
        while (('\n' != ch) && (' ' != ch))
        {
            ch = _getch();
            if ('\x1B' == ch)
            {
                Koniec();
            }
        }
        AL = ch & 0xFF;
    }

    void
    GrubyKursor();

    void
    Klawisz()
    {
        AL = _getch() & 0xFF;
    }

    void
    KlawiszEsc(Bajt klawisz = 0)
    {
        int ch = 0;
        do
        {
            ch = _getch();
        } while (klawisz && (klawisz != ch));

        AL = ch & 0xFF;
    }

    void
    PiszZnak(Bajt znak, Bajt kolor = 0x07, Słowo liczba = 1);

    void
    PokażKursor();

    void
    PokażMysz();

    void
    Pozycja(Bajt x, Bajt y);

    void
    Spacja();

    void
    StanPrzycisków();

    void
    UkryjKursor();

    void
    UkryjMysz();

    // ----- Operacja na rejestrach oraz portach we/wy -----

    void
    DoPortu();

    void
    ŁadujBajt();

    void
    ŁadujSłowo();

    void
    Przerwanie(Bajt numer);

    void
    UstawBajt();

    void
    UstawSłowo();

    void
    ZamianaRej(Rejestr &r1, Rejestr &r2);

    void
    ZerujRej(Rejestr &rejestr);

    void
    ZPortu();

    // ----- Operacja plikowe -----

    void
    TwórzKatalog(const Tekst &nazwa);

    void
    TwórzPlik(const Tekst &nazwa);

    void
    UsuńKatalog(const Tekst &nazwa);

    void
    UsuńPlik(const Tekst &nazwa);

    void
    ZmieńKatalog(const Tekst &nazwa);

    void
    ZmieńNazwę(const Tekst &nazwa1, const Tekst &nazwa2);

    // ----- Liczby pseudolosowe -----

    void
    Losowa8();

    void
    Losowa16();

    // ----- Tryb graficzny -----

    void
    Punkt(Słowo x, Słowo y, Bajt kolor);

    void
    Tryb(Bajt tryb);

    // ----- Dźwięk -----

    void
    Cisza();

    void
    Głos();

    // ----- Porównanie -----

    int __wynik = 0;

    void
    Porównaj(Słowo b1, Słowo b2)
    {
        __wynik = b1 - b2;
    }
};
} // namespace Zdzich
