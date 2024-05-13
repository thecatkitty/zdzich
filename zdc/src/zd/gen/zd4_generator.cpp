#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>

#include <zd/gen/zd4_generator.hpp>
#include <zd/par/node.hpp>
#include <zd/text/pl_string.hpp>

#include "opcodes.hpp"

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define COM_BASE ((uint16_t)0x100)

uint8_t
mreg::encode() const
{
    switch (reg)
    {
    case cpu_register::si:
        return ModRM_SI;

    case cpu_register::di:
        return ModRM_DI;

    case cpu_register::bx:
        return ModRM_BX;
    }
    return 0xFF;
}

bool
zd4_generator::process(const par::call_node &node)
{
    if (text::pl_streqai("Czyść", node.callee))
    {
        return zd4_builtins::Czysc(this, node);
    }

    if (text::pl_streqi("Czytaj", node.callee))
    {
        return zd4_builtins::Czytaj(this, node);
    }

    if (text::pl_streqi("Klawisz", node.callee))
    {
        return zd4_builtins::Klawisz(this, node);
    }

    if (text::pl_streqi("Pisz", node.callee))
    {
        return zd4_builtins::Pisz(this, node);
    }

    if (text::pl_streqi("PiszL", node.callee))
    {
        return zd4_builtins::PiszL(this, node);
    }

    if (text::pl_streqi("Pozycja", node.callee))
    {
        return zd4_builtins::Pozycja(this, node);
    }

    return false;
}

bool
zd4_generator::process(const par::declaration_node &node)
{
    if (!node.target->is<object_node>())
    {
        return false;
    }

    auto target = node.target->as<object_node>();
    switch (target->type)
    {
    case object_type::text: {
        if (!set_symbol(target->name, symbol_type::var_text,
                        zd4_known_section::zd4_section_udat,
                        _udat.reserve(UINT8_MAX)))
        {
            return false;
        }

        auto &symbol = get_symbol(target->name);
        asm_mov(cpu_register::di, symbol);
        asm_mov(mreg{cpu_register::di}, 0x00FD);
        return true;
    }

    default:
        return false;
    }

    return false;
}

bool
zd4_generator::process(const par::end_node &node)
{
    if (node.name.empty())
    {
        asm_mov(cpu_register::ah, 0x4C);
        asm_int(0x21);
        return true;
    }

    return false;
}

void
zd4_generator::link(std::FILE *output)
{
    uint16_t bases[3];
    bases[0] = COM_BASE;                // CODE
    bases[1] = bases[0] + _code.size(); // DATA
    bases[2] = bases[1] + _data.size(); // UDAT

    _code.relocate(output, bases);
    _data.relocate(output, bases);
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
                          unsigned          offset)
{
    auto &symbol = get_symbol(name);
    if (zd4_known_section::zd4_section_unkn != symbol.section)
    {
        // Already defined
        return false;
    }

    symbol.type = type;
    symbol.section = section;
    symbol.offset = offset;
    return true;
}

#define ASM_REQUIRE(expr)                                                      \
    if (!(expr))                                                               \
    {                                                                          \
        return false;                                                          \
    }

#define ASM_BYTE(byte) static_cast<uint8_t>((byte) & 0xFF)

#define ASM_WORD(word)                                                         \
    static_cast<uint8_t>((word) & 0xFF),                                       \
        static_cast<uint8_t>(((word) >> 8) & 0xFF)

static cpu_register
_reg_base(cpu_register reg)
{
    return static_cast<cpu_register>(static_cast<unsigned>(reg) & 0xFF);
}

static uint8_t
_reg_encode(cpu_register reg)
{
    uint8_t ret{0};

    switch (_reg_base(reg))
    {
    case cpu_register::a:
    case cpu_register::stack:
        ret = 0;
        break;

    case cpu_register::c:
    case cpu_register::base:
        ret = 1;
        break;

    case cpu_register::d:
    case cpu_register::src:
        ret = 2;
        break;

    case cpu_register::b:
    case cpu_register::dst:
        ret = 3;
        break;
    }

    if ((cpu_register_hbyte == (static_cast<unsigned>(reg) & 0xFF00)) ||
        _reg_base(reg) > cpu_register::d)
    {
        ret |= 4;
    }

    return ret;
}

static size_t
_reg_size(cpu_register reg)
{
    switch (static_cast<unsigned>(reg) & 0xFF00)
    {
    case cpu_register_lbyte:
    case cpu_register_hbyte:
        return sizeof(uint8_t);

    case cpu_register_word:
        return sizeof(uint16_t);
    }

    return 0;
}

bool
zd4_generator::asm_mov(par::cpu_register dst, const ustring &src)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    auto    data_offset = _data.emit(src.data(), src.size());
    uint8_t code[]{ASM_BYTE(MOV_reg16_imm16 | _reg_encode(dst)),
                   ASM_WORD(data_offset)};

    zd4_relocation ref{+1, zd4_section_data};
    _code.emit(code, sizeof(code), &ref, 1);

    return true;
}

bool
zd4_generator::asm_mov(par::cpu_register dst, const symbol_ref &src)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    uint8_t code[]{ASM_BYTE(MOV_reg16_imm16 | _reg_encode(dst)),
                   ASM_WORD(src.sym.offset + src.off)};

    zd4_relocation ref{+1, src.sym.section};
    _code.emit(code, sizeof(code), &ref, 1);

    return true;
}

bool
zd4_generator::asm_mov(mreg dst, uint16_t src)
{
    uint8_t code[]{ASM_BYTE(MOV_rm16_imm16), ASM_BYTE(dst.encode()),
                   ASM_WORD(src)};
    _code.emit(code, sizeof(code));

    return true;
}

bool
zd4_generator::asm_mov(par::cpu_register dst, mreg src)
{
    uint8_t code[]{ASM_BYTE(MOV_r8_rm8 | _reg_encode(dst)),
                   ASM_BYTE(src.encode())};
    _code.emit(code, sizeof(code));

    return true;
}

bool
zd4_generator::asm_mov(mreg dst, par::cpu_register src)
{
    uint8_t code[]{ASM_BYTE(MOV_rm16_r16),
                   ModRM(dst.encode(), _reg_encode(src))};
    _code.emit(code, sizeof(code));

    return true;
}

bool
zd4_generator::asm_mov(par::cpu_register dst, unsigned src)
{
    if (sizeof(uint8_t) == _reg_size(dst))
    {
        ASM_REQUIRE(UINT8_MAX >= src);

        uint8_t code[]{ASM_BYTE(MOV_reg8_imm8 | _reg_encode(dst)),
                       ASM_BYTE(src)};
        _code.emit(code, sizeof(code));

        return true;
    }

    if (sizeof(uint16_t) == _reg_size(dst))
    {
        ASM_REQUIRE(UINT16_MAX >= src);

        uint8_t code[]{ASM_BYTE(MOV_reg16_imm16 | _reg_encode(dst)),
                       ASM_WORD(src)};
        _code.emit(code, sizeof(code));

        return true;
    }

    return false;
}

bool
zd4_generator::asm_add(par::cpu_register dst, par::cpu_register src)
{
    ASM_REQUIRE(_reg_size(dst) == _reg_size(src));
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    uint8_t modrm{0xFF};
    switch (src)
    {
    case cpu_register::ax:
        modrm = ModRM_A;
        break;

    case cpu_register::bx:
        modrm = ModRM_B;
        break;

    case cpu_register::cx:
        modrm = ModRM_C;
        break;

    case cpu_register::dx:
        modrm = ModRM_D;
        break;

    case cpu_register::sp:
        modrm = ModRM_SPAH;
        break;

    case cpu_register::bp:
        modrm = ModRM_BPCH;
        break;

    case cpu_register::si:
        modrm = ModRM_SIDH;
        break;

    case cpu_register::di:
        modrm = ModRM_DIBH;
        break;
    }

    uint8_t code[]{ASM_BYTE(ADD_r16_rm16),
                   ASM_BYTE(ModRM(modrm, _reg_encode(dst)))};
    _code.emit(code, sizeof(code));

    return true;
}

bool
zd4_generator::asm_inc(par::cpu_register reg)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(reg));

    uint8_t code[]{ASM_BYTE(INC_r16 | _reg_encode(reg))};
    _code.emit(code, sizeof(code));

    return true;
}

bool
zd4_generator::asm_int(unsigned num)
{
    ASM_REQUIRE(UINT8_MAX >= num);

    uint8_t code[]{ASM_BYTE(0xCD), ASM_BYTE(num)};
    _code.emit(code, sizeof(code));

    return true;
}
