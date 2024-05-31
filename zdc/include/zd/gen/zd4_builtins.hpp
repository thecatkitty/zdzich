#pragma once

#include <array>

#include <zd/gen/zd4_generator.hpp>
#include <zd/ustring.hpp>

namespace zd
{

namespace gen
{

template <typename T> struct member_function_traits;

template <typename R, typename C, typename... Args>
struct member_function_traits<R (C::*)(Args...)>
{
    using return_type = R;
    using class_type = C;
    using arg_types = std::tuple<Args...>;
    static constexpr std::size_t arg_count = sizeof...(Args);
};

template <typename R, typename C, typename... Args>
struct member_function_traits<R (C::*)(Args...) const>
{
    using return_type = R;
    using class_type = C;
    using arg_types = std::tuple<Args...>;
    static constexpr std::size_t arg_count = sizeof...(Args);
};

template <typename T>
using extract_args = typename member_function_traits<T>::arg_types;

struct zd4_byte
{
    const symbol     *sym;
    par::cpu_register reg;
    uint8_t           val;

    zd4_byte(symbol *sym_) : sym{sym_}, reg{par::cpu_register::invalid}, val{}
    {
    }

    zd4_byte(par::cpu_register reg_) : sym{nullptr}, reg{reg_}, val{}
    {
    }

    zd4_byte(uint8_t val_)
        : sym{nullptr}, reg{par::cpu_register::invalid}, val{val_}
    {
    }

    bool
    load(x86_assembler &as, par::cpu_register dst) const;
};

struct zd4_file
{
    uint16_t val;

    zd4_file(uint16_t val_) : val{val_}
    {
    }

    bool
    load(x86_assembler &as, par::cpu_register dst) const;
};

struct zd4_text
{
    const symbol  *sym;
    const ustring *val;

    zd4_text(const symbol *sym_) : sym{sym_}, val{nullptr}
    {
    }

    zd4_text(const ustring *val_) : sym{nullptr}, val{val_}
    {
    }

    bool
    load(x86_assembler    &as,
         par::cpu_register dst_buffer,
         par::cpu_register dst_size = par::cpu_register::invalid) const;

    bool
    loadd(x86_assembler &as, par::cpu_register dst) const;
};

struct zd4_word
{
    const symbol     *sym;
    par::cpu_register reg;
    uint16_t          val;

    zd4_word(const symbol *sym_)
        : sym{sym_}, reg{par::cpu_register::invalid}, val{}
    {
    }

    zd4_word(par::cpu_register reg_) : sym{nullptr}, reg{reg_}, val{}
    {
    }

    zd4_word(uint16_t val_)
        : sym{nullptr}, reg{par::cpu_register::invalid}, val{val_}
    {
    }

    bool
    load(x86_assembler &as, par::cpu_register dst) const;
};

class zd4_builtins
{
    bool
    Czekaj_w(const zd4_word &czas);

    bool
    Czysc()
    {
        return Czysc_b(7);
    }

    bool
    Czysc_b(const zd4_byte &atrybut);

    bool
    Czytaj_T(zd4_text &atrybut);

    bool
    DoPortu();

    bool
    Klawisz();

    bool
    Laduj();

    bool
    Losowa16();

    bool
    Losowa8();

    bool
    Nic();

    bool
    Otworz_ft(const zd4_file &plik, const zd4_text &nazwa)
    {
        return Otworz_ftb(plik, nazwa, 2);
    }

    bool
    Otworz_ftb(const zd4_file &plik,
               const zd4_text &nazwa,
               const zd4_byte &tryb);

    bool
    Pisz()
    {
        ustring str{"\r\n"};
        return Pisz_t(&str);
    }

    bool
    Pisz_t(const zd4_text &tekst);

    bool
    Pisz_tt(const zd4_text &tekst1, const zd4_text &tekst2);

    bool
    Pisz_f(const zd4_file &plik)
    {
        ustring str{"\r\n"};
        return Pisz_ft(plik, &str);
    }

    bool
    Pisz_ft(const zd4_file &plik, const zd4_text &tekst);

    bool
    Pisz_ftt(const zd4_file &plik,
             const zd4_text &tekst1,
             const zd4_text &tekst2);

    bool
    Pisz_w(const zd4_word &liczba);

    bool
    Pisz_fw(const zd4_file &plik, const zd4_word &liczba);

    bool
    PiszL()
    {
        ustring str{""};
        return PiszL_t(&str);
    }

    bool
    PiszL_t(const zd4_text &tekst);

    bool
    PiszL_tt(const zd4_text &tekst1, const zd4_text &tekst2);

    bool
    PiszL_f(const zd4_file &plik)
    {
        ustring str{""};
        return PiszL_ft(plik, &str);
    }

    bool
    PiszL_ft(const zd4_file &plik, const zd4_text &tekst);

    bool
    PiszL_ftt(const zd4_file &plik,
              const zd4_text &tekst1,
              const zd4_text &tekst2);

    bool
    PiszL_w(const zd4_word &liczba);

    bool
    PiszL_fw(const zd4_file &plik, const zd4_word &liczba);

    bool
    Pisz8()
    {
        return Pisz8_b(par::cpu_register::al);
    }

    bool
    Pisz8_b(const zd4_byte &liczba);

    bool
    Pisz8_t(const zd4_text &tekst);

    bool
    PiszZnak_b(const zd4_byte &znak)
    {
        return PiszZnak_bb(znak, 7);
    }

    bool
    PiszZnak_bb(const zd4_byte &znak, const zd4_byte &atrybut)
    {
        return PiszZnak_bbw(znak, atrybut, 1);
    }

    bool
    PiszZnak_bbw(const zd4_byte &znak,
                 const zd4_byte &atrybut,
                 const zd4_word &powtorzenia);

    bool
    Pozycja_bb(const zd4_byte &kolumna, const zd4_byte &wiersz);

    bool
    PokazMysz();

    bool
    Przerwanie_b(const zd4_byte &numer);

    bool
    Punkt_wwb(const zd4_word &kolumna,
              const zd4_word &wiersz,
              const zd4_byte &kolor);

    bool
    StanPrzyciskow();

    bool
    StanPrzyciskow_t(const zd4_text &tekst);

    bool
    Tryb_b(const zd4_byte &tryb);

    bool
    TworzKatalog_t(const zd4_text &tekst);

    bool
    TworzPlik_t(const zd4_text &tekst);

    bool
    UkryjMysz();

    bool
    UsunKatalog_t(const zd4_text &tekst);

    bool
    UsunPlik_t(const zd4_text &tekst);

    bool
    Zamknij_f(const zd4_file &plik);

    bool
    ZmienKatalog_t(const zd4_text &tekst);

    bool
    ZmienNazwe_tt(const zd4_text &stara, const zd4_text &nowa);

    bool
    ZmienNazwe_t(const zd4_text &stara_nowa);

    bool
    ZPortu();

    symbol *
    get_procedure(const ustring &name, const uint8_t *code, unsigned size);

  public:
    x86_assembler &as;
    zd4_generator &gen;

    zd4_builtins(zd4_generator &gen_) : as{gen_._as}, gen{gen_}
    {
    }

    bool
    dispatch(const ustring &name, const par::node_list &args);
};

} // namespace gen

} // namespace zd
