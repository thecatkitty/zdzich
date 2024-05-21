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
    DoPortu(zd4_generator *generator, const par::call_node &node);

    static bool
    Klawisz(zd4_generator *generator, const par::call_node &node);

    static bool
    Losowa16(zd4_generator *generator, const par::call_node &node);

    static bool
    Losowa8(zd4_generator *generator, const par::call_node &node);

    static bool
    Otworz(zd4_generator *generator, const par::call_node &node);

    static bool
    Pisz(zd4_generator *generator, const par::call_node &node);

    static bool
    PiszL(zd4_generator *generator, const par::call_node &node);

    static bool
    PiszZnak(zd4_generator *generator, const par::call_node &node);

    static bool
    Pisz8(zd4_generator *generator, const par::call_node &node);

    static bool
    Pozycja(zd4_generator *generator, const par::call_node &node);

    static bool
    PokazMysz(zd4_generator *generator, const par::call_node &node);

    static bool
    Przerwanie(zd4_generator *generator, const par::call_node &node);

    static bool
    Punkt(zd4_generator *generator, const par::call_node &node);

    static bool
    Tryb(zd4_generator *generator, const par::call_node &node);

    static bool
    TworzKatalog(zd4_generator *generator, const par::call_node &node);

    static bool
    TworzPlik(zd4_generator *generator, const par::call_node &node);

    static bool
    UkryjMysz(zd4_generator *generator, const par::call_node &node);

    static bool
    UsunKatalog(zd4_generator *generator, const par::call_node &node);

    static bool
    UsunPlik(zd4_generator *generator, const par::call_node &node);

    static bool
    Zamknij(zd4_generator *generator, const par::call_node &node);

    static bool
    ZmienKatalog(zd4_generator *generator, const par::call_node &node);

    static bool
    ZPortu(zd4_generator *generator, const par::call_node &node);

  private:
    static bool
    Pisz(zd4_generator *generator);

    static bool
    Pisz(zd4_generator *generator, const par::object_node &obj);

    static bool
    Pisz(zd4_generator *generator, const ustring &str);

    static bool
    Pisz(zd4_generator *generator, unsigned fileno, const ustring &str);

    static symbol *
    get_procedure(zd4_generator *generator,
                  const ustring &name,
                  const uint8_t *code,
                  unsigned       size);

    static bool
    file_operation(zd4_generator *generator, par::node &arg);

    static bool
    load_numeric_argument(zd4_generator    *generator,
                          par::cpu_register reg,
                          par::node        &arg);
};

} // namespace gen

} // namespace zd
