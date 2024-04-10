#pragma once

#include <Zdzich/Skoki.hpp>
#include <Zdzich/Typy.hpp>

namespace Zdzich
{

class Program
{
  protected:
    Program();

    // ----- Cykl życia programu -----

    virtual void
    Czekaj(Słowo czas);

    virtual void
    Koniec();

    // ----- Wejście/wyjście -----

    virtual void
    Czytaj(Tekst &tekst);

    virtual void
    Pisz(const Tekst &tekst = "\n");

    virtual void
    Pisz8(Słowo liczba);

    virtual void
    PiszL(const Tekst &tekst = "");

    // ----- Konsola -----

    virtual void
    Czyść(Bajt kolor = 0x07);

    virtual void
    Enter();

    virtual void
    Esc();

    virtual Bajt
    EscSpacjaEnter();

    virtual void
    GrubyKursor();

    virtual Bajt
    Klawisz();

    virtual Bajt
    KlawiszEsc(Bajt klawisz = 0);

    virtual void
    PiszZnak(Bajt znak, Bajt kolor = 0x07, Słowo liczba = 1);

    virtual void
    PokażKursor();

    virtual void
    PokażMysz();

    virtual void
    Pozycja(Bajt x, Bajt y);

    virtual void
    Spacja();

    virtual void
    StanPrzycisków();

    virtual void
    UkryjKursor();

    virtual void
    UkryjMysz();

    // ----- Operacja plikowe -----

    virtual void
    TwórzKatalog(const Tekst &nazwa);

    virtual void
    TwórzPlik(const Tekst &nazwa);

    virtual void
    UsuńKatalog(const Tekst &nazwa);

    virtual void
    UsuńPlik(const Tekst &nazwa);

    virtual void
    ZmieńKatalog(const Tekst &nazwa);

    virtual void
    ZmieńNazwę(const Tekst &nazwa1, const Tekst &nazwa2);

    // ----- Liczby pseudolosowe -----

    virtual Bajt
    Losowa8();

    virtual Słowo
    Losowa16();

    // ----- Tryb graficzny -----

    virtual void
    Punkt(Słowo x, Słowo y, Bajt kolor);

    virtual void
    Tryb(Bajt tryb);

    // ----- Dźwięk -----

    virtual void
    Cisza();

    virtual void
    Głos();

    // ----- Porównanie -----

    int __wynik = 0;

    inline void
    Porównaj(Słowo b1, Słowo b2)
    {
        __wynik = b1 - b2;
    }
};

} // namespace Zdzich
