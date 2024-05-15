#include <zd/gen/x86_assembler.hpp>

#include "opcodes.hpp"

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

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
x86_assembler::cmp(par::cpu_register left, unsigned right)
{
    if (sizeof(uint8_t) == _reg_size(left))
    {
        ASM_REQUIRE(UINT8_MAX >= right);

        if (cpu_register::al == left)
        {
            uint8_t code[]{ASM_BYTE(CMP_AL_imm8), ASM_BYTE(right)};
            _code.emit(code, sizeof(code));
            return true;
        }

        return false;
    }

    if (sizeof(uint16_t) == _reg_size(left))
    {
        ASM_REQUIRE(UINT16_MAX >= right);

        if (cpu_register::ax == left)
        {
            uint8_t code[]{ASM_BYTE(CMP_AX_imm16), ASM_WORD(right)};
            _code.emit(code, sizeof(code));
            return true;
        }

        return false;
    }

    return false;
}

bool
x86_assembler::jne(int rel)
{
    rel -= 2;
    if ((INT8_MAX < rel) || (INT8_MIN > rel))
    {
        rel -= 2;
        uint8_t code[]{ASM_WORD(JNE_rel16), ASM_WORD(rel)};
        _code.emit(code, sizeof(code));
        return true;
    }

    uint8_t code[]{ASM_BYTE(JNE_rel8), ASM_BYTE(rel)};
    _code.emit(code, sizeof(code));
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, const ustring &src)
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
x86_assembler::mov(par::cpu_register dst, const symbol_ref &src)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    uint8_t code[]{ASM_BYTE(MOV_reg16_imm16 | _reg_encode(dst)),
                   ASM_WORD(src.sym.offset + src.off)};

    zd4_relocation ref{+1, src.sym.section};
    _code.emit(code, sizeof(code), &ref, 1);

    return true;
}

bool
x86_assembler::mov(mreg dst, uint16_t src)
{
    uint8_t code[]{ASM_BYTE(MOV_rm16_imm16), ASM_BYTE(dst.encode()),
                   ASM_WORD(src)};
    _code.emit(code, sizeof(code));

    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, mreg src)
{
    if (sizeof(uint8_t) == _reg_size(dst))
    {
        uint8_t code[]{ASM_BYTE(MOV_r8_rm8),
                       ASM_BYTE(ModRM(src.encode(), _reg_encode(dst)))};
        _code.emit(code, sizeof(code));

        return true;
    }

    if (sizeof(uint16_t) == _reg_size(dst))
    {
        uint8_t code[]{ASM_BYTE(MOV_r16_rm16),
                       ASM_BYTE(ModRM(src.encode(), _reg_encode(dst)))};
        _code.emit(code, sizeof(code));

        return true;
    }

    return false;
}

bool
x86_assembler::mov(mreg dst, par::cpu_register src)
{
    uint8_t code[]{ASM_BYTE(MOV_rm16_r16),
                   ModRM(dst.encode(), _reg_encode(src))};
    _code.emit(code, sizeof(code));

    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, unsigned src)
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
x86_assembler::add(par::cpu_register dst, par::cpu_register src)
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
x86_assembler::inc(par::cpu_register reg)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(reg));

    uint8_t code[]{ASM_BYTE(INC_r16 | _reg_encode(reg))};
    _code.emit(code, sizeof(code));

    return true;
}

bool
x86_assembler::inc(mreg reg)
{
    uint8_t code[]{ASM_BYTE(INC_rm16), ASM_BYTE(ModRM(reg.encode(), 0))};
    _code.emit(code, sizeof(code));

    return true;
}

bool
x86_assembler::intr(unsigned num)
{
    ASM_REQUIRE(UINT8_MAX >= num);

    uint8_t code[]{ASM_BYTE(INT_imm8), ASM_BYTE(num)};
    _code.emit(code, sizeof(code));

    return true;
}
