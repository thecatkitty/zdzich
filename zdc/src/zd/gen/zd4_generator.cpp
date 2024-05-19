#include <zd/gen/zd4_builtins.hpp>
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
zd4_generator::process(const par::assignment_node &node)
{
    object_node *target;
    CAST_NODE_OR_FAIL(target, node.target);

    auto &dst = get_symbol(target->name);
    if (node.source->is<number_node>())
    {
        _as.mov(dst, (uint16_t)node.source->as<number_node>()->value);
        return true;
    }

    return false;
}

bool
zd4_generator::process(const par::call_node &node)
{
    if (text::pl_streqi("Czekaj", node.callee))
    {
        return zd4_builtins::Czekaj(this, node);
    }

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

    if (text::pl_streqi("PiszZnak", node.callee))
    {
        return zd4_builtins::PiszZnak(this, node);
    }

    if (text::pl_streqi("Pozycja", node.callee))
    {
        return zd4_builtins::Pozycja(this, node);
    }

    if (text::pl_streqi("Punkt", node.callee))
    {
        return zd4_builtins::Punkt(this, node);
    }

    if (text::pl_streqi("Tryb", node.callee))
    {
        return zd4_builtins::Tryb(this, node);
    }

    if (node.arguments.empty())
    {
        auto &symbol = get_symbol(node.callee);
        return _as.call(symbol);
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
    switch (node.cond)
    {
    case condition::equal: {
        return _as.je(symbol);
    }

    case condition::nonequal: {
        return _as.jne(symbol);
    }
    }

    return false;
}

bool
zd4_generator::process(const par::declaration_node &node)
{
    object_node     *target{nullptr};
    assignment_node *assignment{nullptr};

    // Variable definition or constant assigment?
    zd4_known_section section{zd4_section_udat};
    if (node.target->is<object_node>())
    {
        target = node.target->as<object_node>();
        section = zd4_section_udat;
    }
    else
    {
        if (!node.is_const)
        {
            return false;
        }

        CAST_NODE_OR_FAIL(assignment, node.target);
        CAST_NODE_OR_FAIL(target, assignment->target);
        section = zd4_section_data;
    }

    // Text or number?
    symbol_type type{};
    switch (target->type)
    {
    case object_type::text:
        type = symbol_type::var_text;
        break;

    case object_type::word:
        type = symbol_type::var_word;
        break;

    default:
        return false;
    }

    // Place data in proper sections
    unsigned address{};
    switch (type)
    {
    case symbol_type::var_text: {
        if (zd4_section_udat == section)
        {
            address = _udat.reserve(UINT8_MAX);
            break;
        }

        // Text constant
        string_node *str;
        CAST_NODE_OR_FAIL(str, assignment->source);
        if ((UINT8_MAX - 4) < str->value.size())
        {
            return false;
        }

        std::vector<char> data{};
        data.resize(str->value.size() + 4);

        auto ptr = data.data();
        *(ptr++) = str->value.size() + 2;
        *(ptr++) = str->value.size() + 2;

        for (auto cp : str->value)
        {
            text::encoding::ibm852->encode(ptr, cp);
            ptr++;
        }
        *(ptr++) = 0;
        *ptr = '$';

        address = _data.emit(data.data(), data.size());
        break;
    }

    case symbol_type::var_word: {
        if (zd4_section_udat == section)
        {
            address = _udat.reserve(sizeof(uint16_t));
            break;
        }

        // Integer constant
        number_node *num;
        CAST_NODE_OR_FAIL(num, assignment->source);
        address = _data.emit_word(num->value);
        break;
    }
    }

    // Define a new symbol
    if (!set_symbol(target->name, type, section, address))
    {
        return false;
    }

    // Add string variable initialization
    if (!node.is_const && (symbol_type::var_text == type))
    {
        _as.mov(get_symbol(target->name), (uint16_t)0x00FD);
    }

    return true;
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
zd4_generator::process(const par::jump_node &node)
{
    label_node *label;
    CAST_NODE_OR_FAIL(label, node.target);

    auto &symbol = get_symbol(label->name);
    _as.jmp(symbol);

    return true;
}

bool
zd4_generator::process(const par::label_node &node)
{
    return set_symbol(node.name, symbol_type::label,
                      static_cast<zd4_known_section>(
                          std::distance(_codes.begin(), _curr_code)),
                      _curr_code->size());
}

bool
zd4_generator::process(const par::operation_node &node)
{
    switch (node.op)
    {
    case operation::add: {
        if (node.left->is<object_node>() && node.right->is<number_node>())
        {
            auto left_obj = node.left->as<object_node>();
            if (object_type::word != left_obj->type)
            {
                return false;
            }

            auto &left_sym = get_symbol(left_obj->name);
            auto  right_num = node.right->as<number_node>();
            if (1 == right_num->value)
            {
                _as.inc(symbol_ref{left_sym});
                return true;
            }

            return false;
        }

        return false;
    }

    case operation::compare: {
        if (node.left->is<register_node>() && node.right->is<number_node>())
        {
            return _as.cmp(node.left->as<register_node>()->reg,
                           node.right->as<number_node>()->value);
        }

        if (node.left->is<object_node>() && node.right->is<number_node>())
        {
            auto left_obj = node.left->as<object_node>();
            if (object_type::word != left_obj->type)
            {
                return false;
            }

            auto &left_sym = get_symbol(left_obj->name);
            _as.cmp(left_sym, (uint16_t)node.right->as<number_node>()->value);
            return true;
        }

        return false;
    }
    }

    return false;
}

bool
zd4_generator::process(const par::procedure_node &node)
{
    nesting_guard nested{*this};

    if (!set_symbol(node.name, symbol_type::procedure,
                    static_cast<zd4_known_section>(_curr_code->index),
                    _curr_code->size()))
    {
        return false;
    }

    for (auto &child : node.body)
    {
        if (!child->generate(this))
        {
            return false;
        }
    }

    return _as.ret();
}

void
zd4_generator::link(std::FILE *output)
{
    // Calculate section base addresses
    uint16_t bases[zd4_section_unkn]{COM_BASE};

    int i = 1;
    for (auto &section : _codes)
    {
        bases[i] = section.size();
        i++;
    }

    bases[zd4_section_udat] = _data.size();

    for (int i = 1; i < zd4_section_unkn; i++)
    {
        bases[i] += bases[i - 1];
    }

    // Relocate all section into one output file
    for (auto &section : _codes)
    {
        section.relocate(output, bases, this);
    }

    _data.relocate(output, bases, this);
}

bool
zd::gen::zd4_generator::get_symbol_address(unsigned  index,
                                           unsigned &section,
                                           unsigned &address) const
{
    if (_symbol_num <= index)
    {
        return false;
    }

    auto it = _symbols.begin();
    std::advance(it, index);

    section = it->section;
    address = it->address;

    if (zd4_section_unkn == section)
    {
        std::fprintf(stderr, "error: unresolved symbol %s\n", it->name.data());
        return false;
    }

    return true;
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
                          unsigned          address)
{
    auto &symbol = get_symbol(name);
    if (zd4_known_section::zd4_section_unkn != symbol.section)
    {
        // Already defined
        return false;
    }

    symbol.type = type;
    symbol.section = section;
    symbol.address = address;
    return true;
}
