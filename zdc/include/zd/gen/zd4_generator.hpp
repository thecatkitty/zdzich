#pragma once

#include <list>

#include <zd/gen/generator.hpp>
#include <zd/gen/zd4_builtins.hpp>
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
    ustring           name;
    symbol_type       type;
    zd4_known_section section;
    unsigned          offset;

    symbol(ustring          &name_,
           symbol_type       type_,
           zd4_known_section section_,
           unsigned          offset_)
        : name{name_}, type{type_}, section{section_}, offset{offset_}
    {
    }
};

class zd4_generator : public generator
{
    zd4_section _code;
    zd4_section _data;
    zd4_section _udat;

    std::list<symbol> _symbols;
    unsigned          _symbol_num;

  public:
    zd4_generator() : _code{}, _data{}, _udat{false}, _symbols{}, _symbol_num{0}
    {
    }

    bool
    process(const par::assignment_node &node) override
    {
        return false;
    }

    bool
    process(const par::call_node &node) override;

    bool
    process(const par::condition_node &node) override
    {
        return false;
    }

    bool
    process(const par::declaration_node &node) override;

    bool
    process(const par::end_node &node) override;

    bool
    process(const par::emit_node &node) override
    {
        return false;
    }

    bool
    process(const par::include_node &node) override
    {
        return false;
    }

    bool
    process(const par::jump_node &node) override
    {
        return false;
    }

    bool
    process(const par::label_node &node) override
    {
        return false;
    }

    bool
    process(const par::number_node &node) override
    {
        return false;
    }

    bool
    process(const par::object_node &node) override
    {
        return false;
    }

    bool
    process(const par::operation_node &node) override
    {
        return false;
    }

    bool
    process(const par::procedure_node &node) override
    {
        return false;
    }

    bool
    process(const par::register_node &node) override
    {
        return false;
    }

    bool
    process(const par::string_node &node) override
    {
        return false;
    }

    bool
    process(const par::subscript_node &node) override
    {
        return false;
    }

    void
    link(std::FILE *output);

  private:
    static ustring
    get_cname(const ustring &name);

    symbol &
    get_symbol(const ustring &name);

    bool
    set_symbol(const ustring    &name,
               symbol_type       type,
               zd4_known_section section,
               unsigned          offset);

    bool
    asm_mov(par::cpu_register dst, const ustring &src);

    bool
    asm_mov(par::cpu_register dst, unsigned src);

    bool
    asm_int(unsigned num);

    friend class zd4_builtins;
};

} // namespace gen

} // namespace zd
