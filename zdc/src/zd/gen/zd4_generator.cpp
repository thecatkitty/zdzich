#include <zd/gen/zd4_generator.hpp>
#include <zd/par/node.hpp>
#include <zd/text/pl_string.hpp>

#include "opcodes.hpp"

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define COM_BASE ((uint16_t)0x100)

#define CAST_NODE_OR_FAIL(out, node_ptr)                                       \
    {                                                                          \
        if (!(node_ptr)->is<std::remove_pointer<decltype(out)>::type>())       \
        {                                                                      \
            return false;                                                      \
        };                                                                     \
        out = (node_ptr)->as<std::remove_pointer<decltype(out)>::type>();      \
    }

uint8_t
mreg::encode() const
{
    switch (reg)
    {
    case cpu_register::si:
        return ModRM_SI;

    case cpu_register::di:
        return ModRM_DI;

    case cpu_register::bx:
        return ModRM_BX;
    }
    return 0xFF;
}

bool
zd4_generator::process(const par::call_node &node)
{
    if (text::pl_streqai("Czyść", node.callee))
    {
        return zd4_builtins::Czysc(this, node);
    }

    if (text::pl_streqi("Czytaj", node.callee))
    {
        return zd4_builtins::Czytaj(this, node);
    }

    if (text::pl_streqi("Klawisz", node.callee))
    {
        return zd4_builtins::Klawisz(this, node);
    }

    if (text::pl_streqi("Pisz", node.callee))
    {
        return zd4_builtins::Pisz(this, node);
    }

    if (text::pl_streqi("PiszL", node.callee))
    {
        return zd4_builtins::PiszL(this, node);
    }

    if (text::pl_streqi("Pozycja", node.callee))
    {
        return zd4_builtins::Pozycja(this, node);
    }

    return false;
}

bool
zd4_generator::process(const par::condition_node &node)
{
    jump_node *jump;
    CAST_NODE_OR_FAIL(jump, node.action);

    label_node *label;
    CAST_NODE_OR_FAIL(label, jump->target);

    auto &symbol = get_symbol(label->name);
    auto  rel = (int)symbol.offset - (int)_code.size();

    switch (node.cond)
    {
    case condition::nonequal: {
        return _as.jne(rel);
    }
    }

    return false;
}

bool
zd4_generator::process(const par::declaration_node &node)
{
    object_node *target;
    CAST_NODE_OR_FAIL(target, node.target);

    switch (target->type)
    {
    case object_type::text: {
        if (!set_symbol(target->name, symbol_type::var_text,
                        zd4_known_section::zd4_section_udat,
                        _udat.reserve(UINT8_MAX)))
        {
            return false;
        }

        auto &symbol = get_symbol(target->name);
        _as.mov(cpu_register::di, symbol);
        _as.mov(mreg{cpu_register::di}, 0x00FD);
        return true;
    }

    case object_type::word: {
        return set_symbol(target->name, symbol_type::var_word,
                          zd4_known_section::zd4_section_udat,
                          _udat.reserve(sizeof(uint16_t)));
    }

    default:
        return false;
    }

    return false;
}

bool
zd4_generator::process(const par::end_node &node)
{
    if (node.name.empty())
    {
        _as.mov(cpu_register::ah, 0x4C);
        _as.intr(0x21);
        return true;
    }

    return false;
}

bool
zd4_generator::process(const par::label_node &node)
{
    return set_symbol(node.name, symbol_type::label, zd4_section_code,
                      _code.size());
}

bool
zd4_generator::process(const par::operation_node &node)
{
    switch (node.op)
    {
    case operation::compare: {
        if (node.left->is<register_node>() && node.right->is<number_node>())
        {
            return _as.cmp(node.left->as<register_node>()->reg,
                           node.right->as<number_node>()->value);
        }

        return false;
    }
    }

    return false;
}

void
zd4_generator::link(std::FILE *output)
{
    uint16_t bases[3];
    bases[0] = COM_BASE;                // CODE
    bases[1] = bases[0] + _code.size(); // DATA
    bases[2] = bases[1] + _data.size(); // UDAT

    _code.relocate(output, bases);
    _data.relocate(output, bases);
}

ustring
zd4_generator::get_cname(const ustring &name)
{
    ustring cname{};
    std::for_each(name.begin(), name.end(), [&cname](int codepoint) {
        cname.append(std::toupper(text::pl_toascii(codepoint)));
    });
    return cname;
}

symbol &
zd4_generator::get_symbol(const ustring &name)
{
    auto cname = get_cname(name);
    auto it =
        std::find_if(_symbols.begin(), _symbols.end(), [&cname](symbol &sym) {
            return sym.name == cname;
        });

    if (it == _symbols.end())
    {
        // Add undefined
        _symbols.emplace_back(cname, symbol_type::undefined,
                              zd4_known_section::zd4_section_unkn,
                              _symbol_num++);
        it = std::prev(it);
    }

    return *it;
}

bool
zd4_generator::set_symbol(const ustring    &name,
                          symbol_type       type,
                          zd4_known_section section,
                          unsigned          offset)
{
    auto &symbol = get_symbol(name);
    if (zd4_known_section::zd4_section_unkn != symbol.section)
    {
        // Already defined
        return false;
    }

    symbol.type = type;
    symbol.section = section;
    symbol.offset = offset;
    return true;
}
