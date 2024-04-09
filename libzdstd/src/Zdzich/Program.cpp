#include <iostream>
#include <thread>

#include <Zdzich/Program.hpp>

#ifdef _WIN32
#include <conio.h>
#else
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

    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}
#endif

using namespace Zdzich;

static int
KolorNaAnsi(Bajt kolor)
{
    const int ansi[16] = {30, 34, 32, 36, 31, 35, 33, 37,
                          90, 94, 92, 96, 91, 95, 93, 97};
    return ansi[kolor & 0xF];
}

static int
TłoNaAnsi(Bajt kolor)
{
    const int ansi[16] = {40,  44,  42,  46,  41,  45,  43,  47,
                          100, 104, 102, 106, 101, 105, 103, 107};
    return ansi[kolor >> 4];
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
    std::printf("%d", liczba);
}

void
Program::PiszL(const Tekst &tekst)
{
    Pisz(tekst);
    std::cout << std::endl;
}

void
Program::Czyść(Bajt kolor)
{
    char bufor[16];
    std::snprintf(bufor, 16, "\x1B[2J\x1B[%d;%dm", KolorNaAnsi(kolor),
                  TłoNaAnsi(kolor));
    Pisz(bufor);
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
    return 0;
}

Słowo
Program::Losowa16()
{
    return 0;
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
