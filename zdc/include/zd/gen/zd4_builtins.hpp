#pragma once

#include <zd/gen/zd4_generator.hpp>
#include <zd/par/node.hpp>

namespace zd
{

namespace gen
{

class zd4_generator;

struct zd4_builtins
{
    static bool
    Czekaj(zd4_generator *generator, const par::call_node &node);

    static bool
    Czysc(zd4_generator *generator, const par::call_node &node);

    static bool
    Czytaj(zd4_generator *generator, const par::call_node &node);

    static bool
    Klawisz(zd4_generator *generator, const par::call_node &node);

    static bool
    Pisz(zd4_generator *generator, const par::call_node &node);

    static bool
    PiszL(zd4_generator *generator, const par::call_node &node);

    static bool
    PiszZnak(zd4_generator *generator, const par::call_node &node);

    static bool
    Pozycja(zd4_generator *generator, const par::call_node &node);

    static bool
    Punkt(zd4_generator *generator, const par::call_node &node);

    static bool
    Tryb(zd4_generator *generator, const par::call_node &node);

  private:
    static bool
    Pisz(zd4_generator *generator);

    static bool
    Pisz(zd4_generator *generator, const par::object_node &obj);

    static bool
    Pisz(zd4_generator *generator, const ustring &str);
};

} // namespace gen

} // namespace zd
