#include <zd/gen/zd4_builtins.hpp>
#include <zd/gen/zd4_generator.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define REQUIRE(expr)                                                          \
    if (!(expr))                                                               \
    {                                                                          \
        return false;                                                          \
    }

bool
zd4_builtins::Czysc(zd4_generator *generator, const par::call_node &node)
{
    uint8_t attribute{0x07};
    if (!node.arguments.empty() && node.arguments.front()->is<number_node>())
    {
        attribute = node.arguments.front()->as<number_node>()->value;
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
zd4_builtins::Czytaj(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());
    REQUIRE(node.arguments.front()->is<object_node>());

    auto argument = node.arguments.front()->as<object_node>();
    REQUIRE(object_type::text == argument->type);

    auto &symbol = generator->get_symbol(argument->name);

    // INT 21,A - Buffered Keyboard Input
    generator->asm_mov(cpu_register::ah, 0xA);
    generator->asm_mov(cpu_register::dx, symbol);
    generator->asm_int(0x21);

    generator->asm_mov(cpu_register::bx, symbol_ref{symbol, +1});
    generator->asm_mov(cpu_register::al, mreg{cpu_register::bx});
    generator->asm_mov(cpu_register::ah, 0);
    generator->asm_inc(cpu_register::bx);
    generator->asm_add(cpu_register::bx, cpu_register::ax);

    generator->asm_mov(cpu_register::cx, 0x2400); // NUL $
    generator->asm_mov(mreg{cpu_register::bx}, cpu_register::cx);

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
zd4_builtins::Pisz(zd4_generator *generator)
{
    ustring data{"\r\n$"};

    // INT 21,9 - Print String
    generator->asm_mov(cpu_register::dx, data);
    generator->asm_mov(cpu_register::ah, 9);
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
    if (node.is_bare)
    {
        return Pisz(generator);
    }

    if (node.arguments.size() && node.arguments.front()->is<string_node>())
    {
        return Pisz(generator,
                    node.arguments.front()->as<string_node>()->value);
    }

    return false;
}

bool
zd4_builtins::PiszL(zd4_generator *generator, const call_node &node)
{
    if (node.arguments.size() && node.arguments.front()->is<string_node>())
    {
        ustring value{node.arguments.front()->as<string_node>()->value};
        value.append('\r');
        value.append('\n');
        return Pisz(generator, value);
    }

    return false;
}

bool
zd4_builtins::Pozycja(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(2 == node.arguments.size());

    auto it = node.arguments.begin();
    REQUIRE((*it)->is<number_node>());
    uint8_t column = it->get()->as<number_node>()->value;

    it++;
    REQUIRE((*it)->is<number_node>());
    uint8_t row = it->get()->as<number_node>()->value;

    // INT 10,2 - Set Cursor Position
    generator->asm_mov(cpu_register::ah, 2);
    generator->asm_mov(cpu_register::bh, 0);
    generator->asm_mov(cpu_register::dx, (row << 8) | column);
    generator->asm_int(0x10);

    return true;
}
