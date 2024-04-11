#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <zd/sysio.hpp>

#include <Zdzich/Program.hpp>

using namespace Zdzich;

static const int ANSI_FG[]{30, 34, 32, 36, 31, 35, 33, 37,
                           90, 94, 92, 96, 91, 95, 93, 97};

static const int ANSI_BG[]{40,  44,  42,  46,  41,  45,  43,  47,
                           100, 104, 102, 106, 101, 105, 103, 107};

Program::Program()
{
    std::srand(std::time(NULL));

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32
}

void
Program::Czekaj(Słowo czas)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(czas * 100));
}

void
Program::Koniec()
{
    std::exit(0);
}

void
Program::Czytaj(Tekst &tekst)
{
    std::getline(std::cin, tekst);
}

void
Program::Pisz(const Tekst &tekst)
{
    std::cout.write(tekst.c_str(), tekst.size());
}

void
Program::Pisz8(Słowo liczba)
{
    Pisz(std::to_string(liczba));
}

void
Program::PiszL(const Tekst &tekst)
{
    Pisz(tekst);
    Pisz();
}

void
Program::Czyść(Bajt kolor)
{
    Pisz("\x1B[" + std::to_string(ANSI_FG[kolor & 0xF]) + ";" +
         std::to_string(ANSI_BG[kolor >> 4]) + "m");
    Pisz("\x1B[2J");
    Pisz("\x1B[1H");
}

void
Program::Enter()
{
    while ('\n' != _getch())
        ;
}

void
Program::Esc()
{
    while ('\x1B' != _getch())
        ;
}

Bajt
Program::EscSpacjaEnter()
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
    return ch & 0xFF;
}

void
Program::GrubyKursor()
{
    Pisz("\x1B[1 q");
}

Bajt
Program::Klawisz()
{
    return _getch() & 0xFF;
}

Bajt
Program::KlawiszEsc(Bajt klawisz)
{
    int ch = 0;
    do
    {
        ch = _getch();
    } while (klawisz && (klawisz != ch));

    return ch & 0xFF;
}

void
Program::PiszZnak(Bajt znak, Bajt kolor, Słowo liczba)
{
}

void
Program::PokażKursor()
{
    Pisz("\x1B[?25h\x1B[5 q");
}

void
Program::PokażMysz()
{
}

void
Program::Pozycja(Bajt x, Bajt y)
{
}

void
Program::Spacja()
{
    while (' ' != _getch())
        ;
}

void
Program::StanPrzycisków()
{
}

void
Program::UkryjKursor()
{
    Pisz("\x1B[?25l");
}

void
Program::UkryjMysz()
{
}

void
Program::TwórzKatalog(const Tekst &nazwa)
{
}

void
Program::TwórzPlik(const Tekst &nazwa)
{
}

void
Program::UsuńKatalog(const Tekst &nazwa)
{
}

void
Program::UsuńPlik(const Tekst &nazwa)
{
}

void
Program::ZmieńKatalog(const Tekst &nazwa)
{
}

void
Program::ZmieńNazwę(const Tekst &nazwa1, const Tekst &nazwa2)
{
}

Bajt
Program::Losowa8()
{
    return std::rand() & 0xFF;
}

Słowo
Program::Losowa16()
{
    return std::rand() & 0xFFFF;
}

void
Program::Punkt(Słowo x, Słowo y, Bajt kolor)
{
}

void
Program::Tryb(Bajt tryb)
{
}

void
Program::Cisza()
{
}

void
Program::Głos()
{
}
