#pragma once

#include <Zdzich/Program.hpp>
#include <Zdzich/Rejestry.hpp>
#include <Zdzich/StrumieńDos.hpp>

namespace Zdzich
{

class ProgramDos : public Program
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

    // ----- Konsola -----

    virtual void
    Enter() override
    {
        Program::Enter();
        AL = '\n';
    }

    virtual void
    Esc() override
    {
        Program::Esc();
        AL = '\x1B';
    }

    virtual Bajt
    EscSpacjaEnter() override
    {
        return AL = Program::EscSpacjaEnter();
    }

    virtual Bajt
    Klawisz() override
    {
        return AL = Program::Klawisz();
    }

    virtual Bajt
    KlawiszEsc(Bajt klawisz) override
    {
        return AL = Program::KlawiszEsc(klawisz);
    }

    virtual void
    Spacja() override
    {
        Program::Spacja();
        AL = ' ';
    }

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

    // ----- Liczby pseudolosowe -----

    virtual Bajt
    Losowa8() override
    {
        return AL = Program::Losowa8();
    }

    virtual Słowo
    Losowa16() override
    {
        return AX = Program::Losowa16();
    }
};
} // namespace Zdzich
