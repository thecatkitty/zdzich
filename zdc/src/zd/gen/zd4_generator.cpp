#include <zd/gen/zd4_builtins.hpp>
#include <zd/gen/zd4_generator.hpp>
#include <zd/par/node.hpp>
#include <zd/text/pl_string.hpp>

#include "opcodes.hpp"

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define COM_BASE ((uint16_t)0x100)

#define ASM(x, n)                                                              \
    if (!_as.x)                                                                \
    {                                                                          \
        return make_assembler_error((n));                                      \
    }

#define CAST_NODE_OR_FAIL(out, node_ptr)                                       \
    {                                                                          \
        if (!(node_ptr)->is<std::remove_pointer<decltype(out)>::type>())       \
        {                                                                      \
            return make_unexpected_node(*(node_ptr));                          \
        };                                                                     \
        out = (node_ptr)->as<std::remove_pointer<decltype(out)>::type>();      \
    }

error
zd4_generator::process(const par::assignment_node &node)
{
    if (node.target->is<object_node>())
    {
        auto  target = node.target->as<object_node>();
        auto &dst = get_symbol(target->name);

        if (node.source->is<number_node>())
        {
            ASM(mov(dst, (uint16_t)node.source->as<number_node>()->value),
                node);
            return {};
        }

        if (node.source->is<object_node>())
        {
            ASM(push(get_symbol(node.source->as<object_node>()->name)), node);
            ASM(pop(get_symbol(node.target->as<object_node>()->name)), node);
            return {};
        }

        if (node.source->is<register_node>())
        {
            auto src_reg = node.source->as<register_node>()->reg;
            ASM(mov(cpu_register::di, dst), node);
            if ((symbol_type::var_byte != dst.type) &&
                (cpu_register_word != ((unsigned)src_reg & 0xFF00)))
            {
                ASM(mov(mreg{cpu_register::di}, 0), node);
            }
            ASM(mov(mreg{cpu_register::di}, src_reg), node);
            return {};
        }

        return make_invalid_assignment(node);
    }

    if (node.target->is<register_node>())
    {
        auto target = node.target->as<register_node>();
        auto dst = target->reg;

        // Flag register
        auto value = node.source->as<number_node>()->value;
        if (cpu_register_flag == ((unsigned)dst & 0xFF00))
        {
            switch (dst)
            {
            case cpu_register::flag_c: {
                if (value)
                {
                    ASM(stc(), node);
                    return {};
                }

                ASM(clc(), node);
                return {};
            }

            case cpu_register::flag_d: {
                if (value)
                {
                    ASM(std(), node);
                    return {};
                }

                ASM(cld(), node);
                return {};
            }

            case cpu_register::flag_i: {
                if (value)
                {
                    ASM(sti(), node);
                    return {};
                }

                ASM(cli(), node);
                return {};
            }
            }
        }
        else if (node.source->is<number_node>())
        {
            ASM(mov(dst, (uint16_t)node.source->as<number_node>()->value),
                node);
            return {};
        }

        return make_invalid_assignment(node);
    }

    assert(false && "unhandled assignment target");
}

error
zd4_generator::process(const par::call_node &node)
{
    zd4_builtins builtins{*this};
    if (builtins.dispatch(node.callee, node.arguments))
    {
        return {};
    }

    if (node.arguments.empty())
    {
        auto &symbol = get_symbol(node.callee);
        ASM(call(symbol), node);
        return {};
    }

    return make_unexpected_arguments(node);
}

error
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
        ASM(je(symbol), node);
    }
        return {};

    case condition::less_than: {
        ASM(jb(symbol), node);
        return {};
    }

    case condition::nonequal: {
        ASM(jne(symbol), node);
        return {};
    }
    }

    assert(false && "unhandled condition");
    return {};
}

error
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
        CAST_NODE_OR_FAIL(assignment, node.target);
        CAST_NODE_OR_FAIL(target, assignment->target);

        if (!node.is_const)
        {
            return make_nonconst_assignment(*assignment);
        }

        section = zd4_section_data;
    }

    // Text or number?
    symbol_type type{};
    switch (target->type)
    {
    case object_type::byte:
        type = symbol_type::var_byte;
        break;

    case object_type::text:
        type = symbol_type::var_text;
        break;

    case object_type::word:
        type = symbol_type::var_word;
        break;

    default:
        assert(false && "unhandled object type");
    }

    // Place data in proper sections
    unsigned address{};
    switch (type)
    {
    case symbol_type::var_byte: {
        if (zd4_section_udat == section)
        {
            address = _udat.reserve(sizeof(uint8_t));
            break;
        }

        // Integer constant
        number_node *num;
        CAST_NODE_OR_FAIL(num, assignment->source);
        address = _data.emit_byte(num->value);
        break;
    }

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
            return make_string_too_long(*str);
        }

        std::vector<char> data{};
        data.resize(str->value.size() + 4);

        auto ptr = data.data();
        *(ptr++) = str->value.size() + 2;
        *(ptr++) = str->value.size() + 2;
        ptr = str->value.encode(ptr, text::encoding::ibm852);
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
        return make_symbol_redefinition(*target);
    }

    // Add string variable initialization
    if (!node.is_const && (symbol_type::var_text == type))
    {
        ASM(mov(get_symbol(target->name), (uint16_t)0x00FD), node);
    }

    return {};
}

error
zd4_generator::process(const par::end_node &node)
{
    if (node.name.empty())
    {
        ASM(mov(cpu_register::ah, 0x4C), node);
        ASM(intr(0x21), node);
        return {};
    }

    return make_unexpected_arguments(node);
}

error
zd4_generator::process(const par::emit_node &node)
{
    _curr_code->emit(node.bytes.data(), node.bytes.size());
    return {};
}

error
zd4_generator::process(const par::jump_node &node)
{
    label_node *label;
    CAST_NODE_OR_FAIL(label, node.target);

    auto &symbol = get_symbol(label->name);
    ASM(jmp(symbol), node);

    return {};
}

error
zd4_generator::process(const par::label_node &node)
{
    if (!set_symbol(node.name, symbol_type::label,
                    static_cast<zd4_known_section>(
                        std::distance(_codes.begin(), _curr_code)),
                    _curr_code->size()))
    {
        return make_symbol_redefinition(node);
    }

    return {};
}

error
zd4_generator::process(const par::operation_node &node)
{
    switch (node.op)
    {
    case operation::add: {
        if (node.left->is<register_node>() && node.right->is<number_node>())
        {
            auto left_reg = node.left->as<register_node>();
            auto right_num = node.right->as<number_node>();
            if (1 == right_num->value)
            {
                ASM(inc(left_reg->reg), node);
                return {};
            }

            ASM(add(left_reg->reg, right_num->value), node);
            return {};
        }

        if (node.left->is<object_node>() && node.right->is<number_node>())
        {
            auto  left_obj = node.left->as<object_node>();
            auto &left_sym = get_symbol(left_obj->name);
            auto  right_num = node.right->as<number_node>();
            if (1 == right_num->value)
            {
                ASM(inc(symbol_ref{left_sym}), node);
                return {};
            }

            ASM(add(symbol_ref{left_sym}, right_num->value), node);
            return {};
        }

        return make_invalid_operands(node);
    }

    case operation::compare: {
        if (node.left->is<register_node>() && node.right->is<number_node>())
        {
            ASM(cmp(node.left->as<register_node>()->reg,
                    node.right->as<number_node>()->value),
                node);
            return {};
        }

        if (node.left->is<object_node>() && node.right->is<number_node>())
        {
            auto  left_obj = node.left->as<object_node>();
            auto &left_sym = get_symbol(left_obj->name);
            ASM(cmp(left_sym, (uint16_t)node.right->as<number_node>()->value),
                node);
            return {};
        }

        if (node.left->is<object_node>() && node.right->is<object_node>())
        {
            auto  left_obj = node.left->as<object_node>();
            auto  right_obj = node.left->as<object_node>();
            auto &left_sym = get_symbol(left_obj->name);
            auto &right_sym = get_symbol(right_obj->name);
            ASM(mov(cpu_register::bx, left_sym), node);
            ASM(mov(cpu_register::bx, mreg{cpu_register::bx}), node);
            ASM(cmp(cpu_register::bx, right_sym), node);
            return {};
        }

        return make_invalid_operands(node);
    }

    case operation::subtract: {
        if (node.left->is<object_node>() && node.right->is<number_node>())
        {
            auto left_obj = node.left->as<object_node>();
            if (object_type::word != left_obj->type)
            {
                return make_invalid_operands(node);
            }

            auto &left_sym = get_symbol(left_obj->name);
            auto  right_num = node.right->as<number_node>();
            if (1 == right_num->value)
            {
                ASM(dec(symbol_ref{left_sym}), node);
                return {};
            }

            ASM(sub(symbol_ref{left_sym}, right_num->value), node);
            return {};
        }

        return make_invalid_operands(node);
    }
    }

    assert(false && "unhandled operation");
    return {};
}

error
zd4_generator::process(const par::procedure_node &node)
{
    nesting_guard nested{*this};

    if (!set_symbol(node.name, symbol_type::procedure,
                    static_cast<zd4_known_section>(_curr_code->index),
                    _curr_code->size()))
    {
        position pos{*node.code_path, node.code_line, node.code_column};
        return make_symbol_redefinition(
            object_node{pos, node.name, object_type::procedure});
    }

    for (auto &child : node.body)
    {
        error status = std::move(child->generate(this));
        if (!status)
        {
            return status;
        }
    }

    ASM(ret(), node);
    return {};
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
        assert(false && "symbol index out of range");
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

void
zd4_generator::set_position(const par::node &node)
{
    _path = *node.code_path;
    _line = node.code_line;
    _column = node.code_column;
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

error
zd4_generator::make_unexpected_node(const par::node &node)
{
    set_position(node);
    return error{*this, error_code::unexpected_node};
}

error
zd4_generator::make_invalid_operands(const par::operation_node &node)
{
    set_position(node);
    return error{*this, error_code::invalid_operands, to_cstr(node.op)};
}

error
zd4_generator::make_invalid_assignment(const par::assignment_node &node)
{
    set_position(node);
    return error{*this, error_code::invalid_assignment};
}

error
zd4_generator::make_nonconst_assignment(const par::assignment_node &node)
{
    set_position(node);
    return error{*this, error_code::nonconst_assignment,
                 node.target->as<object_node>()->name.data()};
}

error
zd4_generator::make_unexpected_arguments(const par::node &node)
{
    set_position(node);
    return error{*this, error_code::unexpected_arguments,
                 node.is<call_node>() ? "procedure" : "command"};
}

error
zd4_generator::make_symbol_redefinition(const par::node &node)
{
    set_position(node);
    return error{*this, error_code::symbol_redefinition};
}

error
zd4_generator::make_string_too_long(const par::string_node &node)
{
    set_position(node);
    return error{*this, error_code::string_too_long};
}

error
zd4_generator::make_assembler_error(const par::node &node)
{
    set_position(node);
    return error{*this, error_code::assembler_error};
}
