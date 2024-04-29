#include <Zdzich/ProgramDos.hpp>

struct Program : public Zdzich::ProgramDos
{
    void
    Start()
    {
    Początek:
        Czyść();
        Pisz("Menu");
        Pisz();
        Pisz("1. Kurczak");
        Pisz();
        Pisz("2. Befsztyk");
        Pisz();
        Pisz("3. Kromka chleba");
        Pisz();
        Klawisz();
        Porównaj(AL, 49);
        Zdzich__SkokR OpcjaA;
        Porównaj(AL, 50);
        Zdzich__SkokR OpcjaB;
        Porównaj(AL, 51);
        Zdzich__SkokR OpcjaC;
        Czyść();
        Pisz("Nie jesteś głodny ?");
        Koniec();

    OpcjaA:
        Czyść();
        Pisz("Czy kurczak ci smakuje ?");
        Klawisz();
        Zdzich__Skok Początek;

    OpcjaB:
        Czyść();
        Pisz("Czy befsztyk nie jest za twardy ?");
        Klawisz();
        Zdzich__Skok Początek;

    OpcjaC:
        Czyść();
        Pisz("Czy najadłeś się kromką chleba ?");
        Klawisz();
        Zdzich__Skok Początek;
    }
};

int
main()
{
    Program{}.Start();

    return 0;
}
