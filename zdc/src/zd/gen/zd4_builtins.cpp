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
zd::gen::zd4_builtins::DoPortu(zd4_generator        *generator,
                               const par::call_node &node)
{
    REQUIRE(node.is_bare);

    generator->_as.outb();
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
zd::gen::zd4_builtins::Otworz(zd4_generator        *generator,
                              const par::call_node &node)
{
    REQUIRE(3 >= node.arguments.size());

    auto it = node.arguments.begin();
    REQUIRE(node.arguments.end() != it);
    REQUIRE((*it)->is<subscript_node>());
    auto subscript = (*it)->as<subscript_node>();
    REQUIRE(subscript->value->is<number_node>());
    auto fileno = subscript->value->as<number_node>()->value;

    it++;
    REQUIRE(node.arguments.end() != it);
    auto &name = *it;

    // INT 21,3D - Open File Using Handle
    it++;
    if (node.arguments.end() != it)
    {
        REQUIRE((*it)->is<number_node>());
        auto access = (*it)->as<number_node>()->value;
        generator->_as.mov(cpu_register::al, access);
    }
    else
    {
        // Default to read/write access
        generator->_as.mov(cpu_register::al, 2);
    }

    generator->_as.mov(cpu_register::ah, 0x3D);
    REQUIRE(file_operation(generator, *name));

    // INT 21,46 - Force Duplicate File Handle
    generator->_as.mov(cpu_register::bx, cpu_register::ax);
    generator->_as.mov(cpu_register::ah, 0x46);
    generator->_as.mov(cpu_register::cx, fileno);
    generator->_as.intr(0x21);

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
zd::gen::zd4_builtins::Pisz(zd4_generator *generator,
                            unsigned       fileno,
                            const ustring &str)
{
    std::vector<char> data{};
    data.resize(str.size());
    str.encode(data.data(), text::encoding::ibm852);

    // INT 21,40 - Write To File or Device Using Handle
    generator->_as.mov(cpu_register::ah, 0x40);
    generator->_as.mov(cpu_register::bx, fileno);
    generator->_as.mov(cpu_register::cx, data.size());
    generator->_as.mov(cpu_register::dx, data);
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

    REQUIRE(!node.arguments.empty());
    auto it = node.arguments.begin();
    if ((*it)->is<subscript_node>())
    {
        REQUIRE((*it)->is<subscript_node>());
        auto subscript = (*it)->as<subscript_node>();
        REQUIRE(subscript->value->is<number_node>());
        auto fileno = subscript->value->as<number_node>()->value;

        it++;
        if ((*it)->is<string_node>())
        {
            return Pisz(generator, fileno, (*it)->as<string_node>()->value);
        }

        return false;
    }

    if ((*it)->is<string_node>())
    {
        return Pisz(generator, (*it)->as<string_node>()->value);
    }

    if (node.arguments.front()->is<object_node>())
    {
        return Pisz(generator, *(*it)->as<object_node>());
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

    REQUIRE(node.arguments.size());

    auto it = node.arguments.begin();
    if ((*it)->is<subscript_node>())
    {
        auto subscript = (*it)->as<subscript_node>();
        REQUIRE(subscript->value->is<number_node>());
        auto fileno = subscript->value->as<number_node>()->value;

        it++;
        REQUIRE((*it)->is<string_node>());
        ustring value{(*it)->as<string_node>()->value};
        value.append('\r');
        value.append('\n');
        return Pisz(generator, fileno, value);
    }

    if ((*it)->is<string_node>())
    {
        ustring value{(*it)->as<string_node>()->value};
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
    if (2 < node.arguments.size())
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
zd::gen::zd4_builtins::PokazMysz(zd4_generator        *generator,
                                 const par::call_node &node)
{
    // INT 33,1 - Show Mouse Cursor
    generator->_as.mov(cpu_register::ax, 1);
    generator->_as.intr(0x33);
    return true;
}

bool
zd::gen::zd4_builtins::Przerwanie(zd4_generator        *generator,
                                  const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());
    REQUIRE(node.arguments.front()->is<number_node>());

    generator->_as.intr(node.arguments.front()->as<number_node>()->value);

    return true;
}

bool
zd4_builtins::Punkt(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(3 == node.arguments.size());

    // Column
    auto it = node.arguments.begin();
    REQUIRE(load_numeric_argument(generator, cpu_register::cx, **it));

    // Row
    it++;
    REQUIRE(load_numeric_argument(generator, cpu_register::dx, **it));

    // Color value
    it++;
    REQUIRE(load_numeric_argument(generator, cpu_register::al, **it));

    // INT 10,C - Write Graphics Pixel at Coordinate
    generator->_as.mov(cpu_register::ah, 0xC);
    generator->_as.mov(cpu_register::bx, 0);
    generator->_as.intr(0x10);

    return true;
}

bool
zd::gen::zd4_builtins::StanPrzyciskow(zd4_generator        *generator,
                                      const par::call_node &node)
{
    REQUIRE(1 >= node.arguments.size());

    // INT 33,3 - Get Mouse Position and Button Status
    generator->_as.mov(cpu_register::ax, 3);
    generator->_as.intr(0x33);

    if (1 == node.arguments.size())
    {
        REQUIRE(node.arguments.front()->is<string_node>());
        auto &str = node.arguments.front()->as<string_node>()->value;
        REQUIRE(1 == str.size());
        REQUIRE('!' == *str.data());

        generator->_as.mov(cpu_register::ax, cpu_register::cx);
        generator->_as.mov(cpu_register::cl, 3);
        generator->_as.shr(cpu_register::ax, cpu_register::cl);
        generator->_as.shr(cpu_register::dx, cpu_register::cl);
        generator->_as.mov(cpu_register::cx, cpu_register::ax);
    }

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

    // INT 21,39 - Create Subdirectory (mkdir)
    generator->_as.mov(cpu_register::ah, 0x39);
    return file_operation(generator, *node.arguments.front());
}

bool
zd4_builtins::TworzPlik(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());

    // INT 21,3C - Create File Using Handle
    generator->_as.mov(cpu_register::ah, 0x3C);
    generator->_as.mov(cpu_register::cx, 0);
    return file_operation(generator, *node.arguments.front());
}

bool
zd::gen::zd4_builtins::UkryjMysz(zd4_generator        *generator,
                                 const par::call_node &node)
{
    // INT 33,2 - Hide Mouse Cursor
    generator->_as.mov(cpu_register::ax, 2);
    generator->_as.intr(0x33);
    return true;
}

bool
zd4_builtins::UsunKatalog(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());

    // INT 21,3A - Remove Subdirectory (rmdir)
    generator->_as.mov(cpu_register::ah, 0x3A);
    return file_operation(generator, *node.arguments.front());
}

bool
zd4_builtins::UsunPlik(zd4_generator *generator, const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());

    // INT 21,41 - Delete File
    generator->_as.mov(cpu_register::ah, 0x41);
    return file_operation(generator, *node.arguments.front());
}

bool
zd::gen::zd4_builtins::Zamknij(zd4_generator        *generator,
                               const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());

    REQUIRE(node.arguments.front()->is<subscript_node>());
    auto subscript = node.arguments.front()->as<subscript_node>();
    REQUIRE(subscript->value->is<number_node>());
    auto fileno = subscript->value->as<number_node>()->value;

    // INT 21,3E - Close File Using Handle
    generator->_as.mov(cpu_register::ax, 0x3E);
    generator->_as.mov(cpu_register::bx, fileno);
    generator->_as.intr(0x21);

    return true;
}

bool
zd::gen::zd4_builtins::ZmienKatalog(zd4_generator        *generator,
                                    const par::call_node &node)
{
    REQUIRE(1 == node.arguments.size());

    // INT 21,3B - Change Current Directory (chdir)
    generator->_as.mov(cpu_register::ah, 0x3B);
    return file_operation(generator, *node.arguments.front());
}

bool
zd::gen::zd4_builtins::ZPortu(zd4_generator        *generator,
                              const par::call_node &node)
{
    REQUIRE(node.is_bare);

    generator->_as.inb();
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

bool
zd::gen::zd4_builtins::file_operation(zd4_generator *generator, par::node &arg)
{
    REQUIRE(load_textual_argument(generator, cpu_register::dx, arg));
    generator->_as.intr(0x21);

    return true;
}

bool
zd4_builtins::load_numeric_argument(zd4_generator    *generator,
                                    par::cpu_register reg,
                                    par::node        &arg)
{
    if (arg.is<number_node>())
    {
        auto num = arg.as<number_node>();
        generator->_as.mov(reg, num->value);

        return true;
    }

    if (arg.is<object_node>())
    {
        auto obj = arg.as<object_node>();
        auto &symbol = generator->get_symbol(obj->name);
        generator->_as.mov(cpu_register::si, symbol_ref{symbol});
        generator->_as.mov(reg, mreg{cpu_register::si});

        return true;
    }

    if (arg.is<register_node>())
    {
        auto src_reg = arg.as<register_node>();

        if ((unsigned)reg & 0xFF)
        {
            // The same register or parts
            if (reg == src_reg->reg)
            {
                // The same exact register
                return true;
            }

            return false;
        }

        return generator->_as.mov(reg, src_reg->reg);
    }

    return false;
}

bool
zd::gen::zd4_builtins::load_textual_argument(zd4_generator    *generator,
                                             par::cpu_register reg,
                                             par::node        &arg)
{
    REQUIRE(cpu_register_word == ((unsigned)reg & 0xFF00));

    if (arg.is<object_node>())
    {
        auto  obj = arg.as<object_node>();
        auto &symbol = generator->get_symbol(obj->name);
        generator->_as.mov(reg, symbol_ref{symbol, +2});

        return true;
    }

    if (arg.is<string_node>())
    {
        auto &str = arg.as<string_node>()->value;

        std::vector<char> data{};
        data.resize(str.size() + 1);

        auto ptr = str.encode(data.data(), text::encoding::ibm852);
        *ptr = 0;

        generator->_as.mov(reg, data);

        return true;
    }

    return false;
}
