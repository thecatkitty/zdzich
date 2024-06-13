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
    const par::node *node;

    const symbol     *sym;
    par::cpu_register reg;
    uint8_t           val;

    zd4_byte(symbol *sym_, const par::node *node_ = nullptr)
        : sym{sym_}, reg{par::cpu_register::invalid}, val{}, node{node_}
    {
    }

    zd4_byte(par::cpu_register reg_, const par::node *node_ = nullptr)
        : sym{nullptr}, reg{reg_}, val{}, node{node_}
    {
    }

    zd4_byte(uint8_t val_, const par::node *node_ = nullptr)
        : sym{nullptr}, reg{par::cpu_register::invalid}, val{val_}, node{node_}
    {
    }

    bool
    load(x86_assembler &as, par::cpu_register dst) const;
};

struct zd4_file
{
    const par::node *node;

    uint16_t val;

    zd4_file(uint16_t val_, const par::node *node_ = nullptr)
        : val{val_}, node{node_}
    {
    }

    bool
    load(x86_assembler &as, par::cpu_register dst) const;
};

struct zd4_text
{
    const par::node *node;

    const symbol  *sym;
    const ustring *val;

    zd4_text(const symbol *sym_, const par::node *node_ = nullptr)
        : sym{sym_}, val{nullptr}, node{node_}
    {
    }

    zd4_text(const ustring *val_, const par::node *node_ = nullptr)
        : sym{nullptr}, val{val_}, node{node_}
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
    const par::node *node;

    const symbol     *sym;
    par::cpu_register reg;
    uint16_t          val;

    zd4_word(const symbol *sym_, const par::node *node_ = nullptr)
        : sym{sym_}, reg{par::cpu_register::invalid}, val{}, node{node_}
    {
    }

    zd4_word(par::cpu_register reg_, const par::node *node_ = nullptr)
        : sym{nullptr}, reg{reg_}, val{}, node{node_}
    {
    }

    zd4_word(uint16_t val_, const par::node *node_ = nullptr)
        : sym{nullptr}, reg{par::cpu_register::invalid}, val{val_}, node{node_}
    {
    }

    bool
    load(x86_assembler &as, par::cpu_register dst) const;
};

class zd4_builtins
{
    error
    Czekaj_w(const zd4_word &czas);

    error
    Czysc()
    {
        return Czysc_b(7);
    }

    error
    Czysc_b(const zd4_byte &atrybut);

    error
    Czytaj_T(zd4_text &atrybut);

    error
    DoPortu();

    error
    Klawisz();

    error
    Laduj();

    error
    Losowa16();

    error
    Losowa8();

    error
    Nic();

    error
    Otworz_ft(const zd4_file &plik, const zd4_text &nazwa)
    {
        return Otworz_ftb(plik, nazwa, 2);
    }

    error
    Otworz_ftb(const zd4_file &plik,
               const zd4_text &nazwa,
               const zd4_byte &tryb);

    error
    Pisz()
    {
        ustring str{"\r\n"};
        return Pisz_t(&str);
    }

    error
    Pisz_t(const zd4_text &tekst);

    error
    Pisz_tt(const zd4_text &tekst1, const zd4_text &tekst2);

    error
    Pisz_f(const zd4_file &plik)
    {
        ustring str{"\r\n"};
        return Pisz_ft(plik, &str);
    }

    error
    Pisz_ft(const zd4_file &plik, const zd4_text &tekst);

    error
    Pisz_ftt(const zd4_file &plik,
             const zd4_text &tekst1,
             const zd4_text &tekst2);

    error
    Pisz_w(const zd4_word &liczba);

    error
    Pisz_fw(const zd4_file &plik, const zd4_word &liczba);

    error
    PiszL()
    {
        ustring str{""};
        return PiszL_t(&str);
    }

    error
    PiszL_t(const zd4_text &tekst);

    error
    PiszL_tt(const zd4_text &tekst1, const zd4_text &tekst2);

    error
    PiszL_f(const zd4_file &plik)
    {
        ustring str{""};
        return PiszL_ft(plik, &str);
    }

    error
    PiszL_ft(const zd4_file &plik, const zd4_text &tekst);

    error
    PiszL_ftt(const zd4_file &plik,
              const zd4_text &tekst1,
              const zd4_text &tekst2);

    error
    PiszL_w(const zd4_word &liczba);

    error
    PiszL_fw(const zd4_file &plik, const zd4_word &liczba);

    error
    Pisz8()
    {
        return Pisz8_b(par::cpu_register::al);
    }

    error
    Pisz8_b(const zd4_byte &liczba);

    error
    Pisz8_t(const zd4_text &tekst);

    error
    PiszZnak_b(const zd4_byte &znak)
    {
        return PiszZnak_bb(znak, 7);
    }

    error
    PiszZnak_bb(const zd4_byte &znak, const zd4_byte &atrybut)
    {
        return PiszZnak_bbw(znak, atrybut, 1);
    }

    error
    PiszZnak_bbw(const zd4_byte &znak,
                 const zd4_byte &atrybut,
                 const zd4_word &powtorzenia);

    error
    Pozycja_bb(const zd4_byte &kolumna, const zd4_byte &wiersz);

    error
    PokazMysz();

    error
    Przerwanie_b(const zd4_byte &numer);

    error
    Punkt_wwb(const zd4_word &kolumna,
              const zd4_word &wiersz,
              const zd4_byte &kolor);

    error
    StanPrzyciskow();

    error
    StanPrzyciskow_t(const zd4_text &tekst);

    error
    Tryb_b(const zd4_byte &tryb);

    error
    TworzKatalog_t(const zd4_text &tekst);

    error
    TworzPlik_t(const zd4_text &tekst);

    error
    UkryjMysz();

    error
    UsunKatalog_t(const zd4_text &tekst);

    error
    UsunPlik_t(const zd4_text &tekst);

    error
    Zamknij_f(const zd4_file &plik);

    error
    ZmienKatalog_t(const zd4_text &tekst);

    error
    ZmienNazwe_tt(const zd4_text &stara, const zd4_text &nowa);

    error
    ZmienNazwe_t(const zd4_text &stara_nowa);

    error
    ZPortu();

    std::pair<unsigned, unsigned>
    emit(const uint8_t *code, unsigned size);

    template <size_t N>
    void
    emit(const uint8_t (&code)[N])
    {
        emit(code, N);
    }

#ifdef __ia16__
    void
    emit(std::initializer_list<uint8_t> list)
    {
        emit(list.begin(), list.size());
    }
#endif

    symbol *
    get_procedure(const ustring &name, const uint8_t *code, unsigned size);

  public:
    x86_assembler &as;
    zd4_generator &gen;

    zd4_builtins(zd4_generator &gen_) : as{gen_._as}, gen{gen_}, _node{nullptr}
    {
    }

    error
    dispatch(const par::call_node &node);

  private:
    const par::call_node *_node;
};

} // namespace gen

} // namespace zd
