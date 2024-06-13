#include <zd/gen/zd4_builtins.hpp>
#include <zd/text/characters.hpp>
#include <zd/text/pl_string.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define ASM(x)                                                                 \
    if (!as.x)                                                                 \
    {                                                                          \
        return gen.make_assembler_error(*_node);                               \
    }

#define LOAD(arg, dst)                                                         \
    if (!(arg).load(as, (dst)))                                                \
    {                                                                          \
        return gen.make_assembler_error(*(arg).node);                          \
    }

#define LOAD2(arg, dst1, dst2)                                                 \
    if (!(arg).load(as, (dst1), (dst2)))                                       \
    {                                                                          \
        return gen.make_assembler_error(*(arg).node);                          \
    }

#define REQUIRE_ARG(arg, x)                                                    \
    if (!(arg.x))                                                              \
    {                                                                          \
        return gen.make_invalid_argument(*(arg).node);                         \
    }

#define REQUIRE_SUCCESS(x)                                                     \
    {                                                                          \
        error __status = (x);                                                  \
        if (!__status)                                                         \
        {                                                                      \
            return __status;                                                   \
        }                                                                      \
    }

#define RETURN_IF_FALSE(expr)                                                  \
    if (!(expr))                                                               \
    {                                                                          \
        return false;                                                          \
    }

bool
zd4_byte::load(x86_assembler &as, cpu_register dst) const
{
    if (sym)
    {
        return as.mov(cpu_register::si, *sym) &&
               as.mov(dst, mreg{cpu_register::si});
    }

    if (cpu_register::invalid != reg)
    {
        if (reg == dst)
        {
            return true;
        }

        if (cpu_register_word == ((unsigned)reg & 0xFF00))
        {
            // TODO: warning
            return as.mov(dst,
                          static_cast<cpu_register>(((unsigned)reg & 0xFF) |
                                                    cpu_register_lbyte));
        }

        return as.mov(dst, reg);
    }

    return as.mov(dst, val);
}

bool
zd4_file::load(x86_assembler &as, cpu_register dst) const
{
    return as.mov(dst, val);
}

bool
zd4_text::load(x86_assembler    &as,
               par::cpu_register dst_buffer,
               par::cpu_register dst_size) const
{
    RETURN_IF_FALSE(cpu_register_word == ((unsigned)dst_buffer & 0xFF00));

    if (sym)
    {
        if (cpu_register::invalid != dst_size)
        {
            RETURN_IF_FALSE(as.mov(cpu_register::si, {*sym, +1}));
            RETURN_IF_FALSE(as.mov(dst_size, mreg{cpu_register::si}));
        }
        return as.mov(dst_buffer, {*sym, +2});
    }

    RETURN_IF_FALSE(val);
    std::vector<char> data{};
    data.resize(val->size() + 2);

    auto ptr = val->encode(data.data(), text::encoding::ibm852);
    *(ptr++) = 0;
    *(ptr++) = '$';

    RETURN_IF_FALSE(as.mov(dst_buffer, data));
    if (cpu_register::invalid != dst_size)
    {
        RETURN_IF_FALSE(as.mov(dst_size, ptr - data.data() - 2));
    }
    return true;
}

bool
zd4_text::loadd(x86_assembler &as, par::cpu_register dst) const
{
    RETURN_IF_FALSE(cpu_register_word == ((unsigned)dst & 0xFF00));
    RETURN_IF_FALSE(val);

    std::vector<char> data{};
    data.resize(val->size() + 1);

    auto ptr = val->encode(data.data(), text::encoding::ibm852);
    *ptr = '$';

    return as.mov(dst, data);
}

bool
zd4_word::load(x86_assembler &as, cpu_register dst) const
{
    if (sym)
    {
        return as.mov(cpu_register::si, *sym) &&
               as.mov(dst, mreg{cpu_register::si});
    }

    if (cpu_register::invalid != reg)
    {
        if (reg == dst)
        {
            return true;
        }

        return as.mov(dst, reg);
    }

    return as.mov(dst, val);
}

error
zd4_builtins::Czekaj_w(const zd4_word &czas)
{
    // INT 15,86 - Elapsed Time Wait (AT and PS/2)
    LOAD(czas, cpu_register::cx);
    emit({
        0xB8, 0x00, 0x86, // mov ax, 8600h
        0xBA, 0x00, 0x00, // mov dx, 0
        0xCD, 0x15,       // int 15h
    });
    return {};
}

error
zd4_builtins::Czysc_b(const zd4_byte &atrybut)
{
    // INT 10,6 - Scroll Window Up
    LOAD(atrybut, cpu_register::bh);
    emit({
        0xB8, 0x00, 0x06, // mov ax, 0600h
        0xB9, 0x00, 0x00, // mov cx, 0
        0xBA, 0x4F, 0x18, // mov dx, 184Fh
        0xCD, 0x10,       // int 10h
    });

    // INT 10,2 - Set Cursor Position
    emit({
        0xB4, 0x02,       // mov ah, 2
        0xB7, 0x00,       // mov bh, 0
        0xBA, 0x00, 0x00, // mov dx, 0
        0xCD, 0x10,       // int 10h
    });
    return {};
}

error
zd4_builtins::Czytaj_T(zd4_text &wyjscie)
{
    REQUIRE_ARG(wyjscie, sym);
    REQUIRE_ARG(wyjscie, sym->section == zd4_section_udat);

    // INT 21,A - Buffered Keyboard Input
    ASM(mov(cpu_register::dx, symbol_ref{*wyjscie.sym}));
    emit({
        0xB4, 0x0A, // mov ah, 0Ah
        0xCD, 0x21, // int 21h
    });

    // Append 'NUL $'
    ASM(mov(cpu_register::bx, symbol_ref{*wyjscie.sym, +1}));
    emit({
        0x8A, 0x07,       // mov al, byte [bx]
        0xB4, 0x00,       // mov ah, 0
        0x43,             // inc bx
        0x01, 0xC3,       // add bx, ax
        0xB9, 0x00, 0x24, // mov cx, 2400h
        0x89, 0x0F,       // mov word [bx], cx
    });
    return {};
}

error
zd4_builtins::DoPortu()
{
    emit({
        0xEE, // out dx, al
    });
    return {};
}

error
zd4_builtins::Klawisz()
{
    // INT 21,7 - Direct Console Input Without Echo
    emit({
        0xB4, 0x07, // mov ah, 7
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::Laduj()
{
    emit({
        0xAC, // lodsb
    });
    return {};
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

error
zd4_builtins::Losowa16()
{
    auto procedure =
        get_procedure("$Losowa16", Losowa16_impl, sizeof(Losowa16_impl));
    assert(procedure && "cannot locate a built-in procedure");
    ASM(call(*procedure));
    return {};
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

error
zd4_builtins::Losowa8()
{
    auto procedure =
        get_procedure("$Losowa8", Losowa8_impl, sizeof(Losowa8_impl));
    assert(procedure && "cannot locate a built-in procedure");
    ASM(call(*procedure));
    return {};
}

error
zd4_builtins::Nic()
{
    emit({
        0x90, // nop
    });
    return {};
}

error
zd4_builtins::Otworz_ftb(const zd4_file &plik,
                         const zd4_text &nazwa,
                         const zd4_byte &tryb)
{
    // INT 21,3D - Open File Using Handle
    LOAD(nazwa, cpu_register::dx);
    LOAD(tryb, cpu_register::al);
    emit({
        0xB4, 0x3D, // mov ah, 3Dh
        0xCD, 0x21, // int 21h
    });

    // INT 21,46 - Force Duplicate File Handle
    LOAD(plik, cpu_register::cx);
    emit({
        0x89, 0xC3, // mov bx, ax
        0xB4, 0x46, // mov ah, 46h
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::Pisz_t(const zd4_text &tekst)
{
    // INT 21,9 - Print String
    if (tekst.val)
    {
        if (!tekst.loadd(as, cpu_register::dx))
        {
            return gen.make_assembler_error(*tekst.node);
        }
    }
    else
    {
        LOAD(tekst, cpu_register::dx);
    }

    emit({
        0xB4, 0x09, // mov ah, 9
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::Pisz_tt(const zd4_text &tekst1, const zd4_text &tekst2)
{
    REQUIRE_SUCCESS(Pisz_t(tekst1));
    REQUIRE_SUCCESS(Pisz_t(tekst2));
    return {};
}

error
zd4_builtins::Pisz_ft(const zd4_file &plik, const zd4_text &tekst)
{
    // INT 21,40 - Write To File or Device Using Handle
    LOAD(plik, cpu_register::bx);
    LOAD2(tekst, cpu_register::dx, cpu_register::cl);
    emit({
        0xB5, 0x00, // mov ch, 0
        0xB4, 0x40, // mov ah, 40h
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::Pisz_ftt(const zd4_file &plik,
                       const zd4_text &tekst1,
                       const zd4_text &tekst2)
{
    REQUIRE_SUCCESS(Pisz_ft(plik, tekst1));
    REQUIRE_SUCCESS(Pisz_ft(plik, tekst2));
    return {};
}

error
zd4_builtins::Pisz_w(const zd4_word &liczba)
{
    REQUIRE_ARG(liczba, sym == nullptr);
    REQUIRE_ARG(liczba, reg == cpu_register::invalid);

    char buff[6];
    std::snprintf(buff, sizeof(buff), "%u", liczba.val);
    ustring str{buff};
    return Pisz_t(&str);
}

error
zd4_builtins::Pisz_fw(const zd4_file &plik, const zd4_word &liczba)
{
    REQUIRE_ARG(liczba, sym == nullptr);
    REQUIRE_ARG(liczba, reg == cpu_register::invalid);

    char buff[6];
    std::snprintf(buff, sizeof(buff), "%u", liczba.val);
    ustring str{buff};
    return Pisz_ft(plik, &str);
}

error
zd4_builtins::PiszL_t(const zd4_text &tekst)
{
    REQUIRE_SUCCESS(Pisz_t(tekst));
    REQUIRE_SUCCESS(Pisz());
    return {};
}

error
zd4_builtins::PiszL_tt(const zd4_text &tekst1, const zd4_text &tekst2)
{
    REQUIRE_SUCCESS(Pisz_t(tekst1));
    REQUIRE_SUCCESS(PiszL_t(tekst2));
    return {};
}

error
zd4_builtins::PiszL_ft(const zd4_file &plik, const zd4_text &tekst)
{
    REQUIRE_SUCCESS(Pisz_ft(plik, tekst));
    REQUIRE_SUCCESS(Pisz_f(plik));
    return {};
}

error
zd4_builtins::PiszL_ftt(const zd4_file &plik,
                        const zd4_text &tekst1,
                        const zd4_text &tekst2)
{
    REQUIRE_SUCCESS(Pisz_ft(plik, tekst1));
    REQUIRE_SUCCESS(PiszL_ft(plik, tekst2));
    return {};
}

error
zd4_builtins::PiszL_w(const zd4_word &liczba)
{
    REQUIRE_ARG(liczba, sym == nullptr);
    REQUIRE_ARG(liczba, reg == cpu_register::invalid);

    char buff[8];
    std::snprintf(buff, sizeof(buff), "%u\r\n", liczba.val);
    ustring str{buff};
    return Pisz_t(&str);
}

error
zd4_builtins::PiszL_fw(const zd4_file &plik, const zd4_word &liczba)
{
    REQUIRE_ARG(liczba, sym == nullptr);
    REQUIRE_ARG(liczba, reg == cpu_register::invalid);

    char buff[8];
    std::snprintf(buff, sizeof(buff), "%u\r\n", liczba.val);
    ustring str{buff};
    return Pisz_ft(plik, &str);
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

error
zd4_builtins::Pisz8_b(const zd4_byte &liczba)
{
    LOAD(liczba, cpu_register::cl);
    auto procedure = get_procedure("$Pisz8", Pisz8_impl, sizeof(Pisz8_impl));
    assert(procedure && "cannot locate a built-in procedure");
    ASM(call(*procedure));
    return {};
}

error
zd4_builtins::Pisz8_t(const zd4_text &tekst)
{
    LOAD(tekst, cpu_register::si);
    emit({
        0x8A, 0x0C, // mov cl, byte [si]
    });
    return Pisz8_b(cpu_register::cl);
}

error
zd4_builtins::PiszZnak_bbw(const zd4_byte &znak,
                           const zd4_byte &atrybut,
                           const zd4_word &powtorzenia)
{
    // INT 10,9 - Write Character and Attribute at Cursor Position
    LOAD(znak, cpu_register::al);
    LOAD(atrybut, cpu_register::bl);
    LOAD(powtorzenia, cpu_register::cx);
    emit({
        0xB4, 0x09, // mov ah, 9
        0xB7, 0x00, // mov bh, 0
        0xCD, 0x10, // int 10h
    });
    return {};
}

error
zd4_builtins::Pozycja_bb(const zd4_byte &kolumna, const zd4_byte &wiersz)
{
    // INT 10,2 - Set Cursor Position
    LOAD(kolumna, cpu_register::dl);
    LOAD(wiersz, cpu_register::dh);
    emit({
        0xB4, 0x09, // mov ah, 2
        0xB7, 0x00, // mov bh, 0
        0xCD, 0x10, // int 10h
    });
    return {};
}

error
zd4_builtins::PokazMysz()
{
    // INT 33,1 - Show Mouse Cursor
    emit({
        0xB8, 0x01, 0x00, // mov ax, 1
        0xCD, 0x33,       // int 33h
    });
    return {};
}

error
zd4_builtins::Przerwanie_b(const zd4_byte &numer)
{
    REQUIRE_ARG(numer, reg == cpu_register::invalid);
    REQUIRE_ARG(numer, sym == nullptr);
    ASM(intr(numer.val));
    return {};
}

error
zd4_builtins::Punkt_wwb(const zd4_word &kolumna,
                        const zd4_word &wiersz,
                        const zd4_byte &kolor)
{
    // INT 10,C - Write Graphics Pixel at Coordinate
    LOAD(kolumna, cpu_register::cx);
    LOAD(wiersz, cpu_register::dx);
    LOAD(kolor, cpu_register::al);
    emit({
        0xB4, 0x0C,       // mov ah, 0Ch
        0xBB, 0x00, 0x00, // mov bx, 0
        0xCD, 0x10,       // int 10h
    });
    return {};
}

error
zd4_builtins::StanPrzyciskow()
{
    // INT 33,3 - Get Mouse Position and Button Status
    emit({
        0xB8, 0x30, 0x00, // mov ax, 3
        0xCD, 0x33,       // int 22h
    });
    return {};
}

error
zd4_builtins::StanPrzyciskow_t(const zd4_text &tekst)
{
    REQUIRE_ARG(tekst, sym == nullptr);
    REQUIRE_ARG(tekst, val);
    REQUIRE_ARG(tekst, val->data()[0] == '!');

    REQUIRE_SUCCESS(StanPrzyciskow());
    emit({
        0x89, 0xC8, // mov ax, cx
        0xB1, 0x03, // mov cl, 3
        0xD3, 0xE8, // shr ax, cl
        0xD3, 0xEA, // shr dx, cl
        0x89, 0xC1, // mov cx, ax
    });
    return {};
}

error
zd4_builtins::Tryb_b(const zd4_byte &tryb)
{
    // INT 10,0 - Set Video Mode
    LOAD(tryb, cpu_register::al);
    emit({
        0xB4, 0x00, // mov ah, 0
        0xCD, 0x10, // int 10h
    });
    return {};
}

error
zd4_builtins::TworzKatalog_t(const zd4_text &tekst)
{
    // INT 21,39 - Create Subdirectory (mkdir)
    LOAD(tekst, cpu_register::dx);
    emit({
        0xB4, 0x39, // mov ah, 39h
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::TworzPlik_t(const zd4_text &tekst)
{
    // INT 21,3C - Create File Using Handle
    LOAD(tekst, cpu_register::dx);
    emit({
        0xB4, 0x3C, // mov ah, 3Ch
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::UkryjMysz()
{
    // INT 33,2 - Hide Mouse Cursor
    emit({
        0xB8, 0x02, 0x00, // mov ax, 2
        0xCD, 0x33,       // int 33h
    });
    return {};
}

error
zd4_builtins::UsunKatalog_t(const zd4_text &tekst)
{
    // INT 21,3A - Remove Subdirectory (rmdir)
    LOAD(tekst, cpu_register::dx);
    emit({
        0xB4, 0x3A, // mov ah, 3Ah
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::UsunPlik_t(const zd4_text &tekst)
{
    // INT 21,41 - Delete File
    LOAD(tekst, cpu_register::dx);
    emit({
        0xB4, 0x41, // mov ah, 41h
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::Zamknij_f(const zd4_file &plik)
{
    // INT 21,3E - Close File Using Handle
    LOAD(plik, cpu_register::bx);
    emit({
        0xB4, 0x3E, // mov ah, 3Eh
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::ZmienKatalog_t(const zd4_text &tekst)
{
    // INT 21,3B - Change Current Directory (chdir)
    LOAD(tekst, cpu_register::dx);
    emit({
        0xB4, 0x3B, // mov ah, 3Bh
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::ZmienNazwe_tt(const zd4_text &stara, const zd4_text &nowa)
{
    // INT 21,56 - Rename File
    LOAD(stara, cpu_register::dx);
    LOAD(nowa, cpu_register::di);
    emit({
        0x1E,       // push ds
        0x07,       // pop es
        0xB4, 0x56, // mov ah, 56h
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::ZmienNazwe_t(const zd4_text &stara_nowa)
{
    REQUIRE_ARG(stara_nowa, sym == nullptr);
    REQUIRE_ARG(stara_nowa, val);

    // Irregular string literal pair
    auto comma = std::find(stara_nowa.val->begin(), stara_nowa.val->end(), ',');
    REQUIRE_ARG(stara_nowa, val->end() != comma);

    auto str_it = stara_nowa.val->begin();

    ustring stara{};
    while (comma != str_it)
    {
        stara.append(*(str_it++));
    }

    str_it++;
    while (text::isspace(*str_it))
    {
        str_it++;
    }

    ustring nowa{};
    while (stara_nowa.val->end() != str_it)
    {
        nowa.append(*(str_it++));
    }

    return ZmienNazwe_tt(&stara, &nowa);
}

error
zd4_builtins::ZPortu()
{
    emit({
        0xEC, // in al, dx
    });
    return {};
}

std::pair<unsigned, unsigned>
zd4_builtins::emit(const uint8_t *code, unsigned size)
{
    return {gen._curr_code->index, gen._curr_code->emit(code, size)};
}

symbol *
zd4_builtins::get_procedure(const ustring &name,
                            const uint8_t *code,
                            unsigned       size)
{
    auto &procedure = gen.get_symbol(name);
    if (symbol_type::undefined == procedure.type)
    {
        zd4_generator::nesting_guard nested{gen};

        auto address = emit(code, size);
        if (!gen.set_symbol(name, symbol_type::procedure,
                            static_cast<zd4_known_section>(address.first),
                            address.second))
        {
            return nullptr;
        }
    }

    return &procedure;
}

static bool
_matches_name(const char *signature, const ustring &str)
{
    ustring name{};
    for (auto ch : ustring{signature})
    {
        if ('_' == ch)
        {
            break;
        }

        name.append(ch);
    }

    return text::pl_streqai(name, str);
}

static bool
_matches_args(const char *signature, const node_list &args)
{
    auto arg_types = std::strchr(signature, '_');
    if (!arg_types)
    {
        return args.empty();
    }

    arg_types++;
    if (std::strlen(arg_types) != args.size())
    {
        return false;
    }

    for (auto &arg : args)
    {
        switch (*arg_types)
        {
        case 'b':
        case 'w':
            RETURN_IF_FALSE(
                arg->is<number_node>() || arg->is<register_node>() ||
                (arg->is<object_node>() &&
                 (object_type::text != arg->as<object_node>()->type)));
            break;

        case 'f':
            RETURN_IF_FALSE(
                arg->is<subscript_node>() &&
                arg->as<subscript_node>()->value->is<number_node>());
            break;

        case 't':
            RETURN_IF_FALSE(
                arg->is<string_node>() ||
                (arg->is<object_node>() &&
                 (object_type::text == arg->as<object_node>()->type)));
            break;

        case 'T':
            RETURN_IF_FALSE(
                arg->is<object_node>() &&
                (object_type::text == arg->as<object_node>()->type));
            break;

        default:
            assert(false && "unknown argument type");
        }

        arg_types++;
    }

    return true;
}

template <typename T>
T
get_arg(zd4_builtins *this_, node &arg) = delete;

template <>
zd4_byte
get_arg<zd4_byte>(zd4_builtins *this_, node &arg)
{
    if (arg.is<number_node>())
    {
        return zd4_byte(arg.as<number_node>()->value, &arg);
    }

    if (arg.is<register_node>())
    {
        return {arg.as<register_node>()->reg, &arg};
    }

    if (arg.is<object_node>())
    {
        return {&this_->gen.get_symbol(arg.as<object_node>()->name), &arg};
    }

    assert(false && "unexpected argument node type for a byte");
    return 0xFF;
}

template <>
zd4_file
get_arg<zd4_file>(zd4_builtins *this_, node &arg)
{
    return zd4_file(arg.as<subscript_node>()->value->as<number_node>()->value,
                    &arg);
}

template <>
zd4_text
get_arg<zd4_text>(zd4_builtins *this_, node &arg)
{
    if (arg.is<string_node>())
    {
        return {&arg.as<string_node>()->value, &arg};
    }

    if (arg.is<object_node>())
    {
        return {&this_->gen.get_symbol(arg.as<object_node>()->name), &arg};
    }

    assert(false && "unexpected argument node type for a text");
    return static_cast<symbol *>(nullptr);
}

template <>
zd4_word
get_arg<zd4_word>(zd4_builtins *this_, node &arg)
{
    if (arg.is<number_node>())
    {
        return zd4_word(arg.as<number_node>()->value, &arg);
    }

    if (arg.is<register_node>())
    {
        return {arg.as<register_node>()->reg, &arg};
    }

    if (arg.is<object_node>())
    {
        return {&this_->gen.get_symbol(arg.as<object_node>()->name), &arg};
    }

    assert(false && "unexpected argument node type for a word");
    return 0xFFFF;
}

error
invoke(zd4_builtins *this_, error (zd4_builtins::*mfp)(), const node_list &args)
{
    return (this_->*mfp)();
}

template <typename Arg1>
error
invoke(zd4_builtins *this_,
       error (zd4_builtins::*mfp)(Arg1),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **it);
    return (this_->*mfp)(arg1);
}

template <typename Arg1, typename Arg2>
error
invoke(zd4_builtins *this_,
       error (zd4_builtins::*mfp)(Arg1, Arg2),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **(it++));
    auto arg2 = get_arg<std::decay_t<Arg2>>(this_, **it);
    return (this_->*mfp)(arg1, arg2);
}

template <typename Arg1, typename Arg2, typename Arg3>
error
invoke(zd4_builtins *this_,
       error (zd4_builtins::*mfp)(Arg1, Arg2, Arg3),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **(it++));
    auto arg2 = get_arg<std::decay_t<Arg2>>(this_, **(it++));
    auto arg3 = get_arg<std::decay_t<Arg3>>(this_, **it);
    return (this_->*mfp)(arg1, arg2, arg3);
}

#define INVOKE_IF_MATCH(mf)                                                    \
    {                                                                          \
        auto __signature = #mf;                                                \
        if (_matches_name(__signature, node.callee) &&                         \
            _matches_args(__signature, node.arguments))                        \
        {                                                                      \
            return invoke(this, &zd4_builtins::mf, node.arguments);            \
        }                                                                      \
    }

error
zd4_builtins::dispatch(const call_node &node)
{
    _node = &node;

    INVOKE_IF_MATCH(Czekaj_w);
    INVOKE_IF_MATCH(Czysc);
    INVOKE_IF_MATCH(Czysc_b);
    INVOKE_IF_MATCH(Czytaj_T);
    INVOKE_IF_MATCH(DoPortu);
    INVOKE_IF_MATCH(Klawisz);
    INVOKE_IF_MATCH(Laduj);
    INVOKE_IF_MATCH(Losowa16);
    INVOKE_IF_MATCH(Losowa8);
    INVOKE_IF_MATCH(Nic);
    INVOKE_IF_MATCH(Otworz_ft);
    INVOKE_IF_MATCH(Otworz_ftb);
    INVOKE_IF_MATCH(Pisz);
    INVOKE_IF_MATCH(Pisz_t);
    INVOKE_IF_MATCH(Pisz_tt);
    INVOKE_IF_MATCH(Pisz_f);
    INVOKE_IF_MATCH(Pisz_ft);
    INVOKE_IF_MATCH(Pisz_ftt);
    INVOKE_IF_MATCH(Pisz_w);
    INVOKE_IF_MATCH(Pisz_fw);
    INVOKE_IF_MATCH(PiszL);
    INVOKE_IF_MATCH(PiszL_t);
    INVOKE_IF_MATCH(PiszL_tt);
    INVOKE_IF_MATCH(PiszL_f);
    INVOKE_IF_MATCH(PiszL_ft);
    INVOKE_IF_MATCH(PiszL_ftt);
    INVOKE_IF_MATCH(PiszL_w);
    INVOKE_IF_MATCH(PiszL_fw);
    INVOKE_IF_MATCH(Pisz8);
    INVOKE_IF_MATCH(Pisz8_b);
    INVOKE_IF_MATCH(Pisz8_t);
    INVOKE_IF_MATCH(PiszZnak_b);
    INVOKE_IF_MATCH(PiszZnak_bb);
    INVOKE_IF_MATCH(PiszZnak_bbw);
    INVOKE_IF_MATCH(Pozycja_bb);
    INVOKE_IF_MATCH(PokazMysz);
    INVOKE_IF_MATCH(Przerwanie_b);
    INVOKE_IF_MATCH(Punkt_wwb);
    INVOKE_IF_MATCH(StanPrzyciskow);
    INVOKE_IF_MATCH(StanPrzyciskow_t);
    INVOKE_IF_MATCH(Tryb_b);
    INVOKE_IF_MATCH(TworzKatalog_t);
    INVOKE_IF_MATCH(TworzPlik_t);
    INVOKE_IF_MATCH(UkryjMysz);
    INVOKE_IF_MATCH(UsunKatalog_t);
    INVOKE_IF_MATCH(UsunPlik_t);
    INVOKE_IF_MATCH(Zamknij_f);
    INVOKE_IF_MATCH(ZmienKatalog_t);
    INVOKE_IF_MATCH(ZmienNazwe_tt);
    INVOKE_IF_MATCH(ZmienNazwe_t);
    INVOKE_IF_MATCH(ZPortu);

    return error{gen, zd4_generator::error_code::not_a_builtin};
}
