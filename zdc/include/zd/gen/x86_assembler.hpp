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
    procedure,
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
    zd4_section *_code;
    zd4_section &_data;

  public:
    x86_assembler(zd4_section &data) : _code{nullptr}, _data{data}
    {
    }

    zd4_section &
    bind_code(zd4_section &section)
    {
        auto old = _code;
        _code = &section;
        return *_code;
    }

    bool
    call(const symbol_ref &target);

    bool
    cmp(par::cpu_register left, unsigned right);

    bool
    cmp(const symbol_ref &left, uint16_t right);

    bool
    cmp(mreg left, unsigned right);

    bool
    dec(const symbol_ref &dst);

    bool
    jb(const symbol_ref &target);

    bool
    je(const symbol_ref &target);

    bool
    jmp(const symbol_ref &target);

    bool
    jne(const symbol_ref &target);

    bool
    mov(par::cpu_register dst, par::cpu_register src);

    bool
    mov(par::cpu_register dst, const std::vector<char> &src);

    bool
    mov(par::cpu_register dst, const symbol_ref &src);

    bool
    mov(mreg dst, uint16_t src);

    bool
    mov(par::cpu_register dst, mreg src);

    bool
    mov(const symbol_ref &dst, uint16_t src);

    bool
    mov(mreg dst, par::cpu_register src);

    bool
    mov(par::cpu_register dst, unsigned src);

    bool
    add(par::cpu_register dst, par::cpu_register src);

    bool
    add(const symbol_ref &dst, unsigned src);

    bool
    inc(par::cpu_register reg);

    bool
    inc(mreg reg);

    bool
    inc(const symbol_ref &dst);

    bool
    intr(unsigned num);

    bool
    ret();

    bool
    sub(const symbol_ref &dst, unsigned src);
};

} // namespace gen

} // namespace zd