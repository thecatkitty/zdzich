#include <zd/gen/zd4_builtins.hpp>
#include <zd/text/characters.hpp>
#include <zd/text/pl_string.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define REQUIRE(expr)                                                          \
    if (!(expr))                                                               \
    {                                                                          \
        return false;                                                          \
    }

bool
zd4_byte::load(x86_assembler &as, cpu_register dst) const
{
    if (sym)
    {
        as.mov(cpu_register::si, *sym);
        as.mov(dst, mreg{cpu_register::si});
        return true;
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
    REQUIRE(cpu_register_word == ((unsigned)dst_buffer & 0xFF00));

    if (sym)
    {
        if (cpu_register::invalid != dst_size)
        {
            REQUIRE(as.mov(cpu_register::si, {*sym, +1}));
            REQUIRE(as.mov(dst_size, mreg{cpu_register::si}));
        }
        REQUIRE(as.mov(dst_buffer, {*sym, +2}));
        return true;
    }

    REQUIRE(val);
    std::vector<char> data{};
    data.resize(val->size() + 2);

    auto ptr = val->encode(data.data(), text::encoding::ibm852);
    *(ptr++) = 0;
    *(ptr++) = '$';

    REQUIRE(as.mov(dst_buffer, data));
    if (cpu_register::invalid != dst_size)
    {
        REQUIRE(as.mov(dst_size, ptr - data.data() - 2));
    }
    return true;
}

bool
zd4_text::loadd(x86_assembler &as, par::cpu_register dst) const
{
    REQUIRE(cpu_register_word == ((unsigned)dst & 0xFF00));
    REQUIRE(val);

    std::vector<char> data{};
    data.resize(val->size() + 1);

    auto ptr = val->encode(data.data(), text::encoding::ibm852);
    *ptr = '$';

    REQUIRE(as.mov(dst, data));
    return true;
}

bool
zd4_word::load(x86_assembler &as, cpu_register dst) const
{
    if (sym)
    {
        as.mov(cpu_register::si, *sym);
        as.mov(dst, mreg{cpu_register::si});
        return true;
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

bool
zd4_builtins::Czekaj_w(const zd4_word &czas)
{
    // INT 15,86 - Elapsed Time Wait (AT and PS/2)
    REQUIRE(czas.load(as, cpu_register::cx));
    as.mov(cpu_register::ax, 0x8600);
    as.mov(cpu_register::dx, 0);
    as.intr(0x15);
    return true;
}

bool
zd4_builtins::Czysc_b(const zd4_byte &atrybut)
{
    // INT 10,6 - Scroll Window Up
    REQUIRE(atrybut.load(as, cpu_register::bh));
    as.mov(cpu_register::ax, 0x0600);
    as.mov(cpu_register::cx, 0);
    as.mov(cpu_register::dx, (24 << 8) | 79);
    as.intr(0x10);

    // INT 10,2 - Set Cursor Position
    as.mov(cpu_register::ah, 2);
    as.mov(cpu_register::bh, 0);
    as.mov(cpu_register::dx, 0);
    as.intr(0x10);

    return true;
}

bool
zd4_builtins::Czytaj_T(zd4_text &wyjscie)
{
    REQUIRE(wyjscie.sym);
    REQUIRE(zd4_section_udat == wyjscie.sym->section);

    // INT 21,A - Buffered Keyboard Input
    as.mov(cpu_register::ah, 0xA);
    as.mov(cpu_register::dx, symbol_ref{*wyjscie.sym});
    as.intr(0x21);

    as.mov(cpu_register::bx, symbol_ref{*wyjscie.sym, +1});
    as.mov(cpu_register::al, mreg{cpu_register::bx});
    as.mov(cpu_register::ah, 0);
    as.inc(cpu_register::bx);
    as.add(cpu_register::bx, cpu_register::ax);

    as.mov(cpu_register::cx, 0x2400); // NUL $
    as.mov(mreg{cpu_register::bx}, cpu_register::cx);

    return true;
}

bool
zd4_builtins::DoPortu()
{
    return as.outb();
}

bool
zd4_builtins::Klawisz()
{
    // INT 21,7 - Direct Console Input Without Echo
    as.mov(cpu_register::ah, 7);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::Laduj()
{
    return as.lodsb();
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
zd4_builtins::Losowa16()
{
    auto procedure =
        get_procedure("$Losowa16", Losowa16_impl, sizeof(Losowa16_impl));
    REQUIRE(procedure);
    return as.call(*procedure);
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
zd4_builtins::Losowa8()
{
    auto procedure =
        get_procedure("$Losowa8", Losowa8_impl, sizeof(Losowa8_impl));
    REQUIRE(procedure);
    return as.call(*procedure);
}

bool
zd4_builtins::Nic()
{
    return as.nop();
}

bool
zd4_builtins::Otworz_ftb(const zd4_file &plik,
                         const zd4_text &nazwa,
                         const zd4_byte &tryb)
{
    // INT 21,3D - Open File Using Handle
    REQUIRE(nazwa.load(as, cpu_register::dx));
    REQUIRE(tryb.load(as, cpu_register::al));
    as.mov(cpu_register::ah, 0x3D);
    as.intr(0x21);

    // INT 21,46 - Force Duplicate File Handle
    REQUIRE(plik.load(as, cpu_register::cx));
    as.mov(cpu_register::bx, cpu_register::ax);
    as.mov(cpu_register::ah, 0x46);
    as.intr(0x21);

    return true;
}

bool
zd4_builtins::Pisz_t(const zd4_text &tekst)
{
    // INT 21,9 - Print String
    if (tekst.val)
    {
        REQUIRE(tekst.loadd(as, cpu_register::dx));
    }
    else
    {
        REQUIRE(tekst.load(as, cpu_register::dx));
    }
    as.mov(cpu_register::ah, 9);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::Pisz_tt(const zd4_text &tekst1, const zd4_text &tekst2)
{
    REQUIRE(Pisz_t(tekst1));
    REQUIRE(Pisz_t(tekst2));
    return true;
}

bool
zd4_builtins::Pisz_ft(const zd4_file &plik, const zd4_text &tekst)
{
    // INT 21,40 - Write To File or Device Using Handle
    REQUIRE(plik.load(as, cpu_register::bx));
    REQUIRE(tekst.load(as, cpu_register::dx, cpu_register::cl));
    as.mov(cpu_register::ch, 0);
    as.mov(cpu_register::ah, 0x40);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::Pisz_ftt(const zd4_file &plik,
                       const zd4_text &tekst1,
                       const zd4_text &tekst2)
{
    REQUIRE(Pisz_ft(plik, tekst1));
    REQUIRE(Pisz_ft(plik, tekst2));
    return true;
}

bool
zd4_builtins::Pisz_w(const zd4_word &liczba)
{
    REQUIRE(!liczba.sym);
    REQUIRE(cpu_register::invalid == liczba.reg);

    char buff[6];
    std::snprintf(buff, sizeof(buff), "%u", liczba.val);
    ustring str{buff};
    return Pisz_t(&str);
}

bool
zd4_builtins::Pisz_fw(const zd4_file &plik, const zd4_word &liczba)
{
    REQUIRE(!liczba.sym);
    REQUIRE(cpu_register::invalid == liczba.reg);

    char buff[6];
    std::snprintf(buff, sizeof(buff), "%u", liczba.val);
    ustring str{buff};
    return Pisz_ft(plik, &str);
}

bool
zd4_builtins::PiszL_t(const zd4_text &tekst)
{
    REQUIRE(Pisz_t(tekst));
    REQUIRE(Pisz());
    return true;
}

bool
zd4_builtins::PiszL_tt(const zd4_text &tekst1, const zd4_text &tekst2)
{
    REQUIRE(Pisz_t(tekst1));
    REQUIRE(PiszL_t(tekst2));
    return true;
}

bool
zd4_builtins::PiszL_ft(const zd4_file &plik, const zd4_text &tekst)
{
    REQUIRE(Pisz_ft(plik, tekst));
    REQUIRE(Pisz_f(plik));
    return true;
}

bool
zd4_builtins::PiszL_ftt(const zd4_file &plik,
                        const zd4_text &tekst1,
                        const zd4_text &tekst2)
{
    REQUIRE(Pisz_ft(plik, tekst1));
    REQUIRE(PiszL_ft(plik, tekst2));
    return true;
}

bool
zd4_builtins::PiszL_w(const zd4_word &liczba)
{
    REQUIRE(!liczba.sym);
    REQUIRE(cpu_register::invalid == liczba.reg);

    char buff[8];
    std::snprintf(buff, sizeof(buff), "%u\r\n", liczba.val);
    ustring str{buff};
    return Pisz_t(&str);
}

bool
zd4_builtins::PiszL_fw(const zd4_file &plik, const zd4_word &liczba)
{
    REQUIRE(!liczba.sym);
    REQUIRE(cpu_register::invalid == liczba.reg);

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

bool
zd4_builtins::Pisz8_b(const zd4_byte &liczba)
{
    REQUIRE(liczba.load(as, cpu_register::cl));
    auto procedure = get_procedure("$Pisz8", Pisz8_impl, sizeof(Pisz8_impl));
    REQUIRE(procedure);
    return as.call(*procedure);
}

bool
zd4_builtins::Pisz8_t(const zd4_text &tekst)
{
    REQUIRE(tekst.load(as, cpu_register::si));
    as.mov(cpu_register::cl, mreg{cpu_register::si});
    return Pisz8_b(cpu_register::cl);
}

bool
zd4_builtins::PiszZnak_bbw(const zd4_byte &znak,
                           const zd4_byte &atrybut,
                           const zd4_word &powtorzenia)
{
    // INT 10,9 - Write Character and Attribute at Cursor Position
    REQUIRE(znak.load(as, cpu_register::al));
    REQUIRE(atrybut.load(as, cpu_register::bl));
    REQUIRE(powtorzenia.load(as, cpu_register::cx));
    as.mov(cpu_register::ah, 0x09);
    as.mov(cpu_register::bh, 0);
    as.intr(0x10);
    return true;
}

bool
zd4_builtins::Pozycja_bb(const zd4_byte &kolumna, const zd4_byte &wiersz)
{
    // INT 10,2 - Set Cursor Position
    REQUIRE(kolumna.load(as, cpu_register::dl));
    REQUIRE(wiersz.load(as, cpu_register::dh));
    as.mov(cpu_register::ah, 2);
    as.mov(cpu_register::bh, 0);
    as.intr(0x10);
    return true;
}

bool
zd4_builtins::PokazMysz()
{
    // INT 33,1 - Show Mouse Cursor
    as.mov(cpu_register::ax, 1);
    as.intr(0x33);
    return true;
}

bool
zd4_builtins::Przerwanie_b(const zd4_byte &numer)
{
    REQUIRE(cpu_register::invalid == numer.reg);
    REQUIRE(!numer.sym);
    return as.intr(numer.val);
}

bool
zd4_builtins::Punkt_wwb(const zd4_word &kolumna,
                        const zd4_word &wiersz,
                        const zd4_byte &kolor)
{
    // INT 10,C - Write Graphics Pixel at Coordinate
    REQUIRE(kolumna.load(as, cpu_register::cx));
    REQUIRE(wiersz.load(as, cpu_register::dx));
    REQUIRE(kolor.load(as, cpu_register::al));
    as.mov(cpu_register::ah, 0xC);
    as.mov(cpu_register::bx, 0);
    as.intr(0x10);
    return true;
}

bool
zd4_builtins::StanPrzyciskow()
{
    // INT 33,3 - Get Mouse Position and Button Status
    as.mov(cpu_register::ax, 3);
    as.intr(0x33);
    return true;
}

bool
zd4_builtins::StanPrzyciskow_t(const zd4_text &tekst)
{
    REQUIRE(!tekst.sym);
    REQUIRE(tekst.val);
    REQUIRE('!' == *tekst.val->data());

    REQUIRE(StanPrzyciskow());
    as.mov(cpu_register::ax, cpu_register::cx);
    as.mov(cpu_register::cl, 3);
    as.shr(cpu_register::ax, cpu_register::cl);
    as.shr(cpu_register::dx, cpu_register::cl);
    as.mov(cpu_register::cx, cpu_register::ax);
    return true;
}

bool
zd4_builtins::Tryb_b(const zd4_byte &tryb)
{
    // INT 10,0 - Set Video Mode
    REQUIRE(tryb.load(as, cpu_register::al));
    as.mov(cpu_register::ah, 0);
    as.intr(0x10);
    return true;
}

bool
zd4_builtins::TworzKatalog_t(const zd4_text &tekst)
{
    // INT 21,39 - Create Subdirectory (mkdir)
    REQUIRE(tekst.load(as, cpu_register::dx));
    as.mov(cpu_register::ah, 0x39);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::TworzPlik_t(const zd4_text &tekst)
{
    // INT 21,3C - Create File Using Handle
    REQUIRE(tekst.load(as, cpu_register::dx));
    as.mov(cpu_register::ah, 0x3C);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::UkryjMysz()
{
    // INT 33,2 - Hide Mouse Cursor
    as.mov(cpu_register::ax, 2);
    as.intr(0x33);
    return true;
}

bool
zd4_builtins::UsunKatalog_t(const zd4_text &tekst)
{
    // INT 21,3A - Remove Subdirectory (rmdir)
    REQUIRE(tekst.load(as, cpu_register::dx));
    as.mov(cpu_register::ah, 0x3A);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::UsunPlik_t(const zd4_text &tekst)
{
    // INT 21,41 - Delete File
    REQUIRE(tekst.load(as, cpu_register::dx));
    as.mov(cpu_register::ah, 0x41);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::Zamknij_f(const zd4_file &plik)
{
    // INT 21,3E - Close File Using Handle
    REQUIRE(plik.load(as, cpu_register::bx));
    as.mov(cpu_register::ah, 0x3E);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::ZmienKatalog_t(const zd4_text &tekst)
{
    // INT 21,3B - Change Current Directory (chdir)
    REQUIRE(tekst.load(as, cpu_register::dx));
    as.mov(cpu_register::ah, 0x3B);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::ZmienNazwe_tt(const zd4_text &stara, const zd4_text &nowa)
{
    // INT 21,56 - Rename File
    REQUIRE(stara.load(as, cpu_register::dx));
    REQUIRE(nowa.load(as, cpu_register::di));
    as.push(x86_segment::ds);
    as.pop(x86_segment::es);
    as.mov(cpu_register::ah, 0x56);
    as.intr(0x21);
    return true;
}

bool
zd4_builtins::ZmienNazwe_t(const zd4_text &stara_nowa)
{
    REQUIRE(!stara_nowa.sym);
    REQUIRE(stara_nowa.val);

    // Irregular string literal pair
    auto comma = std::find(stara_nowa.val->begin(), stara_nowa.val->end(), ',');
    REQUIRE(stara_nowa.val->end() != comma);

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

bool
zd4_builtins::ZPortu()
{
    return as.inb();
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

        if (!gen.set_symbol(
                name, symbol_type::procedure,
                static_cast<zd4_known_section>(gen._curr_code->index),
                gen._curr_code->emit(code, size)))
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
            REQUIRE(arg->is<number_node>() || arg->is<register_node>() ||
                    (arg->is<object_node>() &&
                     (object_type::text != arg->as<object_node>()->type)));
            break;

        case 'f':
            REQUIRE(arg->is<subscript_node>() &&
                    arg->as<subscript_node>()->value->is<number_node>());
            break;

        case 't':
            REQUIRE(arg->is<string_node>() ||
                    (arg->is<object_node>() &&
                     (object_type::text == arg->as<object_node>()->type)));
            break;

        case 'T':
            REQUIRE(arg->is<object_node>() &&
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
        return arg.as<number_node>()->value;
    }

    if (arg.is<register_node>())
    {
        return arg.as<register_node>()->reg;
    }

    if (arg.is<object_node>())
    {
        return &this_->gen.get_symbol(arg.as<object_node>()->name);
    }

    assert(false && "unexpected argument node type for a byte");
}

template <>
zd4_file
get_arg<zd4_file>(zd4_builtins *this_, node &arg)
{
    return arg.as<subscript_node>()->value->as<number_node>()->value;
}

template <>
zd4_text
get_arg<zd4_text>(zd4_builtins *this_, node &arg)
{
    if (arg.is<string_node>())
    {
        return &arg.as<string_node>()->value;
    }

    if (arg.is<object_node>())
    {
        return &this_->gen.get_symbol(arg.as<object_node>()->name);
    }

    assert(false && "unexpected argument node type for a text");
}

template <>
zd4_word
get_arg<zd4_word>(zd4_builtins *this_, node &arg)
{
    if (arg.is<number_node>())
    {
        return arg.as<number_node>()->value;
    }

    if (arg.is<register_node>())
    {
        return arg.as<register_node>()->reg;
    }

    if (arg.is<object_node>())
    {
        return &this_->gen.get_symbol(arg.as<object_node>()->name);
    }

    assert(false && "unexpected argument node type for a word");
}

bool
invoke(zd4_builtins *this_, bool (zd4_builtins::*mfp)(), const node_list &args)
{
    return (this_->*mfp)();
}

template <typename Arg1>
bool
invoke(zd4_builtins *this_,
       bool (zd4_builtins::*mfp)(Arg1),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **it);
    return (this_->*mfp)(arg1);
}

template <typename Arg1, typename Arg2>
bool
invoke(zd4_builtins *this_,
       bool (zd4_builtins::*mfp)(Arg1, Arg2),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **(it++));
    auto arg2 = get_arg<std::decay_t<Arg2>>(this_, **it);
    return (this_->*mfp)(arg1, arg2);
}

template <typename Arg1, typename Arg2, typename Arg3>
bool
invoke(zd4_builtins *this_,
       bool (zd4_builtins::*mfp)(Arg1, Arg2, Arg3),
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
        if (_matches_name(__signature, name) &&                                \
            _matches_args(__signature, args))                                  \
        {                                                                      \
            return invoke(this, &zd4_builtins::mf, args);                      \
        }                                                                      \
    }

bool
zd4_builtins::dispatch(const ustring &name, const node_list &args)
{
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
    return false;
}
