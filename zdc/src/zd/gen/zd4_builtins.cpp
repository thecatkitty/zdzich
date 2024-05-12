#include <zd/gen/zd4_builtins.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

bool
zd4_builtins::Pisz(zd4_generator *generator, const ustring &str)
{
    ustring data{str};
    data.append('$');
    // TODO: Convert to CP852

    generator->asm_mov(cpu_register::dx, data);
    generator->asm_mov(cpu_register::ah, 9);
    generator->asm_int(0x21);
    return true;
}

bool
zd4_builtins::Pisz(zd4_generator *generator, const call_node &node)
{
    if (node.arguments.size() && node.arguments.front()->is<string_node>())
    {
        auto arg_string =
            reinterpret_cast<string_node *>(node.arguments.front().get());
        return Pisz(generator, arg_string->value);
    }

    return false;
}

bool
zd4_builtins::PiszL(zd4_generator *generator, const call_node &node)
{
    if (node.arguments.size() && node.arguments.front()->is<string_node>())
    {
        auto arg_string =
            reinterpret_cast<string_node *>(node.arguments.front().get());

        ustring value{arg_string->value};
        value.append('\r');
        value.append('\n');
        return Pisz(generator, value);
    }

    return false;
}
