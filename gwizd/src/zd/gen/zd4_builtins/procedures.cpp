#include <zd/gen/zd4_builtins.hpp>
#include <zd/text/characters.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define ASM(x)                                                                 \
    if (!as.x)                                                                 \
    {                                                                          \
        return gen.make_assembler_error(*_node);                               \
    }

#define EMIT(...)                                                              \
    {                                                                          \
        static const uint8_t __code[] = __VA_ARGS__;                           \
        emit(__code, sizeof(__code));                                          \
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

error
zd4_builtins::Czekaj_w(const zd4_word &czas)
{
    // INT 15,86 - Elapsed Time Wait (AT and PS/2)
    LOAD(czas, cpu_register::cx);
    EMIT({
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
    EMIT({
        0xB8, 0x00, 0x06, // mov ax, 0600h
        0xB9, 0x00, 0x00, // mov cx, 0
        0xBA, 0x4F, 0x18, // mov dx, 184Fh
        0xCD, 0x10,       // int 10h
    });

    // INT 10,2 - Set Cursor Position
    EMIT({
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
    EMIT({
        0xB4, 0x0A, // mov ah, 0Ah
        0xCD, 0x21, // int 21h
    });

    // Append 'NUL $'
    ASM(mov(cpu_register::bx, symbol_ref{*wyjscie.sym, +1}));
    EMIT({
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
    EMIT({
        0xEE, // out dx, al
    });
    return {};
}

error
zd4_builtins::Klawisz()
{
    // INT 21,7 - Direct Console Input Without Echo
    EMIT({
        0xB4, 0x07, // mov ah, 7
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::Laduj()
{
    EMIT({
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
    EMIT({
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
    EMIT({
        0xB4, 0x3D, // mov ah, 3Dh
        0xCD, 0x21, // int 21h
    });

    // INT 21,46 - Force Duplicate File Handle
    LOAD(plik, cpu_register::cx);
    EMIT({
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

    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
        0xB4, 0x3C, // mov ah, 3Ch
        0xCD, 0x21, // int 21h
    });
    return {};
}

error
zd4_builtins::UkryjMysz()
{
    // INT 33,2 - Hide Mouse Cursor
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
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
    EMIT({
        0xEC, // in al, dx
    });
    return {};
}

std::pair<unsigned, unsigned>
#ifdef __ia16__
    __attribute__((noinline))
#endif
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
