#include <cstdio>

#include <zd/gen/zd4_generator.hpp>
#include <zd/par/node.hpp>
#include <zd/text/pl_string.hpp>

#include "opcodes.hpp"

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define COM_BASE ((uint16_t)0x100)

bool
zd4_generator::process(const par::call_node &node)
{
    if (text::pl_streqai("Czyść", node.callee))
    {
        return zd4_builtins::Czysc(this, node);
    }

    if (text::pl_streqi("Pisz", node.callee))
    {
        return zd4_builtins::Pisz(this, node);
    }

    if (text::pl_streqi("PiszL", node.callee))
    {
        return zd4_builtins::PiszL(this, node);
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
    uint16_t bases[]{
        COM_BASE,                          // CODE
        uint16_t(COM_BASE + _code.size()), // DATA
    };

    _code.relocate(output, bases);
    _data.relocate(output, bases);
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
zd4_generator::asm_int(unsigned num)
{
    ASM_REQUIRE(UINT8_MAX >= num);

    uint8_t code[]{ASM_BYTE(0xCD), ASM_BYTE(num)};
    _code.emit(code, sizeof(code));

    return true;
}
