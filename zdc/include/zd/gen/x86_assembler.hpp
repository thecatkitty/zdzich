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
    var_text,
};

struct symbol
{
    ustring     name;
    symbol_type type;
    unsigned    section;
    unsigned    offset;

    symbol(ustring    &name_,
           symbol_type type_,
           unsigned    section_,
           unsigned    offset_)
        : name{name_}, type{type_}, section{section_}, offset{offset_}
    {
    }
};

struct symbol_ref
{
    const symbol &sym;
    unsigned      off;

    symbol_ref(const symbol &sym_, unsigned off_ = 0) : sym{sym_}, off{off_}
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
    intr(unsigned num);
};

} // namespace gen

} // namespace zd