#include <zd/gen/zd4_builtins.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

bool
zd4_builtins::Czysc(zd4_generator *generator, const par::call_node &node)
{
    uint8_t attribute{0x07};
    if (!node.arguments.empty() && node.arguments.front()->is<number_node>())
    {
        auto arg_integer =
            reinterpret_cast<number_node *>(node.arguments.front().get());
        attribute = arg_integer->value;
    }

    // INT 10,6 - Scroll Window Up
    generator->asm_mov(cpu_register::ax, 0x0600);
    generator->asm_mov(cpu_register::bh, attribute);
    generator->asm_mov(cpu_register::cx, 0);
    generator->asm_mov(cpu_register::dx, (24 << 8) | 79);
    generator->asm_int(0x10);

    // INT 10,2 - Set Cursor Position
    generator->asm_mov(cpu_register::ah, 2);
    generator->asm_mov(cpu_register::bh, 0);
    generator->asm_mov(cpu_register::dx, 0);
    generator->asm_int(0x10);

    return true;
}

bool
zd4_builtins::Klawisz(zd4_generator *generator, const par::call_node &node)
{
    // INT 21,7 - Direct Console Input Without Echo
    generator->asm_mov(cpu_register::ah, 7);
    generator->asm_int(0x21);
    return true;
}

bool
zd4_builtins::Pisz(zd4_generator *generator, const ustring &str)
{
    ustring data{str};
    data.append('$');
    // TODO: Convert to CP852

    // INT 21,9 - Print String
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
