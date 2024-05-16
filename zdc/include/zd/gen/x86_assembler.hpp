#pragma once

#include <zd/gen/zd4_section.hpp>
#include <zd/par/node.hpp>

namespace zd
{

namespace gen
{

enum class symbol_type
{
    undefined,
    label,
    var_text,
    var_word,
};

struct symbol
{
    ustring     name;
    symbol_type type;
    unsigned    section;
    unsigned    address;

    symbol(ustring    &name_,
           symbol_type type_,
           unsigned    section_,
           unsigned    address_)
        : name{name_}, type{type_}, section{section_}, address{address_}
    {
    }
};

struct symbol_ref
{
    const symbol &sym;
    int           off;

    symbol_ref(const symbol &sym_, int off_ = 0) : sym{sym_}, off{off_}
    {
    }
};

struct mreg
{
    par::cpu_register reg;

    uint8_t
    encode() const;
};

class x86_assembler
{
    zd4_section &_code;
    zd4_section &_data;

  public:
    x86_assembler(zd4_section &code, zd4_section &data)
        : _code{code}, _data{data}
    {
    }

    bool
    call(const symbol_ref &target);

    bool
    cmp(par::cpu_register left, unsigned right);

    bool
    jne(const symbol_ref &target);

    bool
    mov(par::cpu_register dst, const ustring &src);

    bool
    mov(par::cpu_register dst, const symbol_ref &src);

    bool
    mov(mreg dst, uint16_t src);

    bool
    mov(par::cpu_register dst, mreg src);

    bool
    mov(mreg dst, par::cpu_register src);

    bool
    mov(par::cpu_register dst, unsigned src);

    bool
    add(par::cpu_register dst, par::cpu_register src);

    bool
    inc(par::cpu_register reg);

    bool
    inc(mreg reg);

    bool
    intr(unsigned num);
};

} // namespace gen

} // namespace zd