#include <zd/gen/zd4_builtins.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define RETURN_IF_FALSE(expr)                                                  \
    if (!(expr))                                                               \
    {                                                                          \
        return false;                                                          \
    }

zd4_byte::zd4_byte(symbol *sym_, const par::node *node_)
    : sym{sym_}, reg{par::cpu_register::invalid}, val{}, node{node_}
{
}

zd4_byte::zd4_byte(par::cpu_register reg_, const par::node *node_)
    : sym{nullptr}, reg{reg_}, val{}, node{node_}
{
}

zd4_byte::zd4_byte(uint8_t val_, const par::node *node_)
    : sym{nullptr}, reg{par::cpu_register::invalid}, val{val_}, node{node_}
{
}

bool
zd4_byte::load(x86_assembler &as, cpu_register dst) const
{
    if (sym)
    {
        return as.mov(cpu_register::si, *sym) &&
               as.mov(dst, mreg{cpu_register::si});
    }

    if (cpu_register::invalid != reg)
    {
        if (reg == dst)
        {
            return true;
        }

        if (cpu_register_word == ((unsigned)reg & 0xFF00))
        {
            // TODO: warning
            return as.mov(dst,
                          static_cast<cpu_register>(((unsigned)reg & 0xFF) |
                                                    cpu_register_lbyte));
        }

        return as.mov(dst, reg);
    }

    return as.mov(dst, val);
}

zd4_file::zd4_file(uint16_t val_, const par::node *node_)
    : val{val_}, node{node_}
{
}

bool
zd4_file::load(x86_assembler &as, cpu_register dst) const
{
    return as.mov(dst, val);
}

zd4_text::zd4_text(const symbol *sym_, const par::node *node_)
    : sym{sym_}, val{nullptr}, node{node_}
{
}

zd4_text::zd4_text(const ustring *val_, const par::node *node_)
    : sym{nullptr}, val{val_}, node{node_}
{
}

bool
zd4_text::load(x86_assembler    &as,
               par::cpu_register dst_buffer,
               par::cpu_register dst_size) const
{
    RETURN_IF_FALSE(cpu_register_word == ((unsigned)dst_buffer & 0xFF00));

    if (sym)
    {
        if (cpu_register::invalid != dst_size)
        {
            RETURN_IF_FALSE(as.mov(cpu_register::si, {*sym, +1}));
            RETURN_IF_FALSE(as.mov(dst_size, mreg{cpu_register::si}));
        }
        return as.mov(dst_buffer, {*sym, +2});
    }

    RETURN_IF_FALSE(val);
    std::vector<char> data{};
    data.resize(val->size() + 2);

    auto ptr = val->encode(data.data(), text::encoding::ibm852);
    *(ptr++) = 0;
    *(ptr++) = '$';

    RETURN_IF_FALSE(as.mov(dst_buffer, data));
    if (cpu_register::invalid != dst_size)
    {
        RETURN_IF_FALSE(as.mov(dst_size, ptr - data.data() - 2));
    }
    return true;
}

bool
zd4_text::loadd(x86_assembler &as, par::cpu_register dst) const
{
    RETURN_IF_FALSE(cpu_register_word == ((unsigned)dst & 0xFF00));
    RETURN_IF_FALSE(val);

    std::vector<char> data{};
    data.resize(val->size() + 1);

    auto ptr = val->encode(data.data(), text::encoding::ibm852);
    *ptr = '$';

    return as.mov(dst, data);
}

zd4_word::zd4_word(const symbol *sym_, const par::node *node_)
    : sym{sym_}, reg{par::cpu_register::invalid}, val{}, node{node_}
{
}

zd4_word::zd4_word(par::cpu_register reg_, const par::node *node_)
    : sym{nullptr}, reg{reg_}, val{}, node{node_}
{
}

zd4_word::zd4_word(uint16_t val_, const par::node *node_)
    : sym{nullptr}, reg{par::cpu_register::invalid}, val{val_}, node{node_}
{
}

bool
zd4_word::load(x86_assembler &as, cpu_register dst) const
{
    if (sym)
    {
        return as.mov(cpu_register::si, *sym) &&
               as.mov(dst, mreg{cpu_register::si});
    }

    if (cpu_register::invalid != reg)
    {
        if (reg == dst)
        {
            return true;
        }

        return as.mov(dst, reg);
    }

    return as.mov(dst, val);
}
