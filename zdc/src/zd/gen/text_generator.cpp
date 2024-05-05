#include <cstdio>

#include <zd/gen/text_generator.hpp>
#include <zd/par/node.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

bool
text_generator::process(const assignment_node &node)
{
    std::fputs("<assignment ", _out);
    node.target->generate(this);
    std::fputc(' ', _out);
    node.source->generate(this);
    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const call_node &node)
{
    std::fputs("<call ", _out);
    std::fputs(node.callee.data(), _out);

    if (node.is_bare)
    {
        std::fputs(" bare", _out);
    }

    for (auto &argument : node.arguments)
    {
        std::fputc(' ', _out);
        argument->generate(this);
    }

    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const operation_node &node)
{
    std::fputs("<operation ", _out);
    switch (node.op)
    {
    case operation::add:
        std::fputs("add", _out);
        break;

    case operation::compare:
        std::fputs("cmp", _out);
        break;

    case operation::subtract:
        std::fputs("sub", _out);
        break;
    }

    std::fputc(' ', _out);
    node.left->generate(this);
    std::fputc(' ', _out);
    node.right->generate(this);
    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const condition_node &node)
{
    std::fputs("<condition ", _out);
    switch (node.cond)
    {
    case condition::equal:
        std::fputs("eq", _out);
        break;

    case condition::less_than:
        std::fputs("lt", _out);
        break;

    case condition::greater_than:
        std::fputs("gt", _out);
        break;

    case condition::nonequal:
        std::fputs("ne", _out);
        break;

    case condition::less_or_equal:
        std::fputs("le", _out);
        break;

    case condition::greater_or_equal:
        std::fputs("ge", _out);
        break;
    }

    std::fputc(' ', _out);
    node.action->generate(this);

    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const declaration_node &node)
{
    std::fputs("<declaration ", _out);
    node.target->generate(this);
    std::fputc('>', _out);
    return true;
}
bool
text_generator::process(const emit_node &node)
{
    std::fputs("<emit", _out);

    for (auto byte : node.bytes)
    {
        std::fprintf(_out, " %02X", (unsigned)byte);
    }

    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const end_node &node)
{
    if (node.name.empty())
    {
        std::fputs("<end>", _out);
        return true;
    }

    std::fprintf(_out, "<end %s>", node.name.data());
    return true;
}

bool
text_generator::process(const include_node &node)
{
    if (node.is_binary)
    {
        std::fprintf(_out, "<include binary %s>", node.name.data());
        return true;
    }

    std::fprintf(_out, "<include %s>", node.name.data());
    return true;
}

bool
text_generator::process(const jump_node &node)
{
    std::fputs("<jump ", _out);
    node.target->generate(this);
    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const label_node &node)
{
    std::fprintf(_out, "<label %s>", node.name.data());
    return true;
}

bool
text_generator::process(const number_node &node)
{
    std::fprintf(_out, "<number %d>", node.value);
    return true;
}

bool
text_generator::process(const object_node &node)
{
    std::fprintf(_out, "<variable %s:", node.name.data());

    switch (node.type)
    {
    case object_type::text:
        std::fputs("text", _out);
        break;

    case object_type::byte:
        std::fputs("byte", _out);
        break;

    case object_type::word:
        std::fputs("word", _out);
        break;
    }

    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const procedure_node &node)
{
    std::fprintf(_out, "<procedure %s", node.name.data());

    for (auto &item : node.body)
    {
        std::fputc(' ', _out);
        item->generate(this);
    }

    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const register_node &node)
{
    std::fputs("<register ", _out);

    std::fputc("ABCDSD"[(static_cast<uint16_t>(node.reg) & 0xFF) - 1], _out);
    switch (static_cast<uint16_t>(node.reg) & 0xFF00)
    {
    case cpu_register_lbyte:
        std::fputc('L', _out);
        break;

    case cpu_register_hbyte:
        std::fputc('H', _out);
        break;

    case cpu_register_word:
        std::fputc(
            ((cpu_register::si == node.reg) || (cpu_register::di == node.reg))
                ? 'I'
                : 'X',
            _out);
        break;
    }

    std::fputc('>', _out);
    return true;
}

bool
text_generator::process(const string_node &node)
{
    std::fprintf(_out, "<string %s>", node.value.data());
    return true;
}

bool
text_generator::process(const subscript_node &node)
{
    std::fputs("<subscript ", _out);
    node.value->generate(this);
    std::fputc('>', _out);
    return true;
}
