#pragma once

#include <Zdzich/Typy.hpp>

namespace Zdzich
{

struct Rejestr
{
    virtual void
    Zeruj() = 0;
};

class Rejestr8 : public Rejestr
{
    Bajt _rejestr;

  public:
    Rejestr8
    operator=(Bajt wartość)
    {
        _rejestr = wartość;
        return *this;
    }

    operator Bajt &()
    {
        return _rejestr;
    }

    virtual void
    Zeruj() override
    {
        _rejestr = 0;
    }
};

class Rejestr16 : public Rejestr
{
    Słowo _rejestr;

  public:
    Rejestr16 &
    operator=(Słowo wartość)
    {
        _rejestr = wartość;
        return *this;
    }

    operator Słowo &()
    {
        return _rejestr;
    }

    virtual void
    Zeruj() override
    {
        _rejestr = 0;
    }
};

struct RejestrHL : public Rejestr16
{
    Rejestr8 &
    H()
    {
        return reinterpret_cast<Rejestr8 *>(this)[1];
    }

    Rejestr8 &
    L()
    {
        return reinterpret_cast<Rejestr8 *>(this)[0];
    }
};

class ParaRejestrów : public Rejestr
{
    Rejestr &_r1, &_r2;

  public:
    ParaRejestrów(Rejestr &r1, Rejestr &r2) : _r1{r1}, _r2{r2}
    {
    }

    virtual void
    Zeruj() override
    {
        _r1.Zeruj();
        _r2.Zeruj();
    }
};

class CzwórkaRejestrów : public Rejestr
{
    Rejestr &_r1, &_r2, &_r3, &_r4;

  public:
    CzwórkaRejestrów(Rejestr &r1, Rejestr &r2, Rejestr &r3, Rejestr &r4)
        : _r1{r1}, _r2{r2}, _r3{r3}, _r4{r4}
    {
    }

    virtual void
    Zeruj() override
    {
        _r1.Zeruj();
        _r2.Zeruj();
        _r3.Zeruj();
        _r4.Zeruj();
    }
};

struct Znacznik
{
    void
    operator--();

    void
    operator++();
};

} // namespace Zdzich
