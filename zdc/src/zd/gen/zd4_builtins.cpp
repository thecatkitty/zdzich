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
zd4_builtins::Czekaj(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());
    REQUIRE(node.arguments.front()->is<number_node>());

    // INT 15,86 - Elapsed Time Wait (AT and PS/2)
    generator->_as.mov(cpu_register::ax, 0x8600);
    generator->_as.mov(cpu_register::cx,
                       node.arguments.front()->as<number_node>()->value);
    generator->_as.mov(cpu_register::dx, 0);
    generator->_as.intr(0x15);
    return true;
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
    generator->_as.mov(cpu_register::ax, 0x0600);
    generator->_as.mov(cpu_register::bh, attribute);
    generator->_as.mov(cpu_register::cx, 0);
    generator->_as.mov(cpu_register::dx, (24 << 8) | 79);
    generator->_as.intr(0x10);

    // INT 10,2 - Set Cursor Position
    generator->_as.mov(cpu_register::ah, 2);
    generator->_as.mov(cpu_register::bh, 0);
    generator->_as.mov(cpu_register::dx, 0);
    generator->_as.intr(0x10);

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
    generator->_as.mov(cpu_register::ah, 0xA);
    generator->_as.mov(cpu_register::dx, symbol);
    generator->_as.intr(0x21);

    generator->_as.mov(cpu_register::bx, symbol_ref{symbol, +1});
    generator->_as.mov(cpu_register::al, mreg{cpu_register::bx});
    generator->_as.mov(cpu_register::ah, 0);
    generator->_as.inc(cpu_register::bx);
    generator->_as.add(cpu_register::bx, cpu_register::ax);

    generator->_as.mov(cpu_register::cx, 0x2400); // NUL $
    generator->_as.mov(mreg{cpu_register::bx}, cpu_register::cx);

    return true;
}

bool
zd4_builtins::Klawisz(zd4_generator *generator, const par::call_node &node)
{
    // INT 21,7 - Direct Console Input Without Echo
    generator->_as.mov(cpu_register::ah, 7);
    generator->_as.intr(0x21);
    return true;
}

// Procedure $Losowa16
static const uint8_t Losowa16_impl[]{
    0x1E,             // push ds
    0x33, 0xC0,       // xor ax, ax
    0x8E, 0xD8,       // mov ds, ax
    0xA1, 0x6C, 0x04, // mov ax, [046Ch]
    0x35, 0xAA, 0xAA, // xor ax, AAAAh
    0x1F,             // pop ds
    0xC3,             // ret
};

bool
zd4_builtins::Losowa16(zd4_generator *generator, const call_node &node)
{
    REQUIRE(node.arguments.empty());

    auto procedure = get_procedure(generator, "$Losowa16", Losowa16_impl,
                                   sizeof(Losowa16_impl));
    REQUIRE(procedure);

    generator->_as.call(*procedure);

    return true;
}

// Procedure $Losowa8
static const uint8_t Losowa8_impl[]{
    0x1E,             // push ds
    0x33, 0xC0,       // xor ax, ax
    0x8E, 0xD8,       // mov ds, ax
    0xA0, 0x6C, 0x04, // mov al, [046Ch]
    0x34, 0xAA,       // xor al, AAh
    0x1F,             // pop ds
    0xC3,             // ret
};

bool
zd4_builtins::Losowa8(zd4_generator *generator, const call_node &node)
{
    REQUIRE(node.arguments.empty());

    auto procedure =
        get_procedure(generator, "Losowa8", Losowa8_impl, sizeof(Losowa8_impl));
    REQUIRE(procedure);

    generator->_as.call(*procedure);

    return true;
}

bool
zd4_builtins::Pisz(zd4_generator *generator)
{
    std::vector<char> data{'\r', '\n', '$'};

    // INT 21,9 - Print String
    generator->_as.mov(cpu_register::dx, data);
    generator->_as.mov(cpu_register::ah, 9);
    generator->_as.intr(0x21);
    return true;
}

bool
zd4_builtins::Pisz(zd4_generator *generator, const ustring &str)
{
    std::vector<char> data{};
    data.resize(str.size() + 1);

    auto ptr = str.encode(data.data(), text::encoding::ibm852);
    *ptr = '$';

    // INT 21,9 - Print String
    generator->_as.mov(cpu_register::dx, data);
    generator->_as.mov(cpu_register::ah, 9);
    generator->_as.intr(0x21);
    return true;
}

bool
zd4_builtins::Pisz(zd4_generator *generator, const par::object_node &obj)
{
    REQUIRE(object_type::text == obj.type);

    auto &symbol = generator->get_symbol(obj.name);

    // INT 21,9 - Print String
    generator->_as.mov(cpu_register::dx, symbol_ref{symbol, +2});
    generator->_as.mov(cpu_register::ah, 9);
    generator->_as.intr(0x21);

    return true;
}

bool
zd4_builtins::Pisz(zd4_generator *generator, const call_node &node)
{
    if (node.is_bare)
    {
        return Pisz(generator);
    }

    REQUIRE(node.arguments.size());
    if (node.arguments.front()->is<string_node>())
    {
        return Pisz(generator,
                    node.arguments.front()->as<string_node>()->value);
    }

    if (node.arguments.front()->is<object_node>())
    {
        return Pisz(generator, *node.arguments.front()->as<object_node>());
    }

    return false;
}

bool
zd4_builtins::PiszL(zd4_generator *generator, const call_node &node)
{
    if (node.is_bare)
    {
        return Pisz(generator);
    }

    if (node.arguments.size() && node.arguments.front()->is<string_node>())
    {
        ustring value{node.arguments.front()->as<string_node>()->value};
        value.append('\r');
        value.append('\n');
        return Pisz(generator, value);
    }

    return false;
}

// Procedure $Pisz8, input in CL
static const uint8_t Pisz8_impl[]{
    0xBB, 0xFF, 0xFF,       // hs:  mov bx, FFFFh
    0xFE, 0xC3,             //      inc bl
    0x80, 0xE9, 0x64,       //      sub cl, 100
    0x73, 0xF9,             //      jae hs
    0x80, 0xC1, 0x64,       // ts:  add cl, 100
    0xFE, 0xC7,             //      inc bh
    0x80, 0xE9, 0x0A,       //      sub cl, 10
    0x73, 0xF9,             //      jae ts
    0x80, 0xC1, 0x0A,       //      add cl, 10
    0x81, 0xC3, 0x30, 0x30, //      add bx, 3030h
    0x80, 0xC1, 0x30,       //      add cl, 30h
    0xB4, 0x02,             //      mov ah, 2 ; INT 21,2
    0x88, 0xDA,             //      mov dl, bl
    0xCD, 0x21,             //      int 21h
    0x88, 0xFA,             //      mov dl, bh
    0xCD, 0x21,             //      int 21h
    0x88, 0xCA,             //      mov dl, cl
    0xCD, 0x21,             //      int 21h
    0xC3,                   //      ret
};

bool
zd4_builtins::Pisz8(zd4_generator *generator, const call_node &node)
{
    REQUIRE(1 == node.arguments.size());

    auto procedure =
        get_procedure(generator, "$Pisz8", Pisz8_impl, sizeof(Pisz8_impl));
    REQUIRE(procedure);

    if (node.arguments.front()->is<register_node>())
    {
        auto reg = node.arguments.front()->as<register_node>();
        generator->_as.mov(cpu_register::cl, reg->reg);
    }
    else
    {
        REQUIRE(node.arguments.front()->is<object_node>());

        auto num = node.arguments.front()->as<object_node>();
        REQUIRE(object_type::word == num->type);

        auto &symbol = generator->get_symbol(num->name);
        generator->_as.mov(cpu_register::bx, symbol_ref{symbol});
        generator->_as.mov(cpu_register::cl, mreg{cpu_register::bx});
    }

    generator->_as.call(*procedure);

    return true;
}

bool
zd4_builtins::PiszZnak(zd4_generator *generator, const call_node &node)
{
    REQUIRE(!node.arguments.empty());
    REQUIRE(3 >= node.arguments.size());

    // Character code
    auto it = node.arguments.begin();
    REQUIRE((*it)->is<number_node>());
    uint8_t character = (*it)->as<number_node>()->value;

    // Display attribute (colors)
    uint8_t attribute{7};
    if (1 < node.arguments.size())
    {
        it++;
        REQUIRE((*it)->is<number_node>());
        attribute = (*it)->as<number_node>()->value;
    }

    // Number of repetitions
    uint16_t count{1};
    if (1 < node.arguments.size())
    {
        it++;
        REQUIRE((*it)->is<number_node>());
        count = (*it)->as<number_node>()->value;
    }

    // INT 10,9 - Write Character and Attribute at Cursor Position
    generator->_as.mov(cpu_register::ax, 0x0900 | character);
    generator->_as.mov(cpu_register::bx, attribute);
    generator->_as.mov(cpu_register::cx, count);
    generator->_as.intr(0x10);

    return true;
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
    generator->_as.mov(cpu_register::ah, 2);
    generator->_as.mov(cpu_register::bh, 0);
    generator->_as.mov(cpu_register::dx, (row << 8) | column);
    generator->_as.intr(0x10);

    return true;
}

bool
zd4_builtins::Punkt(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(3 == node.arguments.size());

    // Column
    auto it = node.arguments.begin();
    if ((*it)->is<object_node>())
    {
        auto arg_x = (*it)->as<object_node>();
        REQUIRE(object_type::word == arg_x->type);

        auto &sym_x = generator->get_symbol(arg_x->name);
        generator->_as.mov(cpu_register::si, symbol_ref{sym_x});
        generator->_as.mov(cpu_register::cx, mreg{cpu_register::si});
    }
    else
    {
        return false;
    }

    // Row
    it++;
    if ((*it)->is<object_node>())
    {
        auto arg_y = (*it)->as<object_node>();
        REQUIRE(object_type::word == arg_y->type);

        auto &sym_y = generator->get_symbol(arg_y->name);
        generator->_as.mov(cpu_register::si, symbol_ref{sym_y});
        generator->_as.mov(cpu_register::dx, mreg{cpu_register::si});
    }
    else
    {
        return false;
    }

    // Color value
    it++;
    if ((*it)->is<object_node>())
    {
        auto arg_c = (*it)->as<object_node>();
        REQUIRE(object_type::word == arg_c->type);

        auto &sym_c = generator->get_symbol(arg_c->name);
        generator->_as.mov(cpu_register::si, symbol_ref{sym_c});
        generator->_as.mov(cpu_register::al, mreg{cpu_register::si});
    }
    else
    {
        return false;
    }

    // INT 10,C - Write Graphics Pixel at Coordinate
    generator->_as.mov(cpu_register::ah, 0xC);
    generator->_as.mov(cpu_register::bx, 0);
    generator->_as.intr(0x10);

    return true;
}

bool
zd4_builtins::Tryb(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());
    REQUIRE(node.arguments.front()->is<number_node>());

    auto mode = node.arguments.front()->as<number_node>()->value;
    REQUIRE((0 <= mode) && (UINT8_MAX >= mode));

    // INT 10,0 - Set Video Mode
    generator->_as.mov(cpu_register::ax, mode);
    generator->_as.intr(0x10);

    return true;
}

bool
zd4_builtins::TworzKatalog(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());
    REQUIRE(node.arguments.front()->is<string_node>());

    auto &name = node.arguments.front()->as<string_node>()->value;

    std::vector<char> data{};
    data.resize(name.size() + 1);

    auto ptr = name.encode(data.data(), text::encoding::ibm852);
    *ptr = 0;

    // INT 21,39 - Create Subdirectory (mkdir)
    generator->_as.mov(cpu_register::ah, 0x39);
    generator->_as.mov(cpu_register::dx, data);
    generator->_as.intr(0x21);

    return true;
}

bool
zd::gen::zd4_builtins::ZmienKatalog(zd4_generator        *generator,
                                    const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());
    REQUIRE(node.arguments.front()->is<string_node>());

    auto &name = node.arguments.front()->as<string_node>()->value;

    std::vector<char> data{};
    data.resize(name.size() + 1);

    auto ptr = name.encode(data.data(), text::encoding::ibm852);
    *ptr = 0;

    // INT 21,3B - Change Current Directory (chdir)
    generator->_as.mov(cpu_register::ah, 0x3B);
    generator->_as.mov(cpu_register::dx, data);
    generator->_as.intr(0x21);

    return true;
}

symbol *
zd4_builtins::get_procedure(zd4_generator *generator,
                            const ustring &name,
                            const uint8_t *code,
                            unsigned       size)
{
    auto &procedure = generator->get_symbol(name);
    if (symbol_type::undefined == procedure.type)
    {
        zd4_generator::nesting_guard nested{*generator};

        if (!generator->set_symbol(
                name, symbol_type::procedure,
                static_cast<zd4_known_section>(generator->_curr_code->index),
                generator->_curr_code->emit(code, size)))
        {
            return nullptr;
        }
    }

    return &procedure;
}
