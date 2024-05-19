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
x86_assembler::call(const symbol_ref &target)
{
    _code->emit_byte(CALL_rel16);
    _code->emit_ref({target.sym.address, target.sym.section, -2, true});
    return true;
}

bool
x86_assembler::cmp(par::cpu_register left, unsigned right)
{
    if (sizeof(uint8_t) == _reg_size(left))
    {
        ASM_REQUIRE(UINT8_MAX >= right);

        if (cpu_register::al == left)
        {
            _code->emit_byte(CMP_AL_imm8);
            _code->emit_byte(right);
            return true;
        }

        return false;
    }

    if (sizeof(uint16_t) == _reg_size(left))
    {
        ASM_REQUIRE(UINT16_MAX >= right);

        if (cpu_register::ax == left)
        {
            _code->emit_byte(CMP_AX_imm16);
            _code->emit_word(right);
            return true;
        }

        return false;
    }

    return false;
}

bool
x86_assembler::cmp(mreg left, unsigned right)
{
    if (sizeof(uint16_t) == _reg_size(left.reg))
    {
        ASM_REQUIRE(UINT16_MAX >= right);

        _code->emit_byte(CMP_rm16_imm16);
        _code->emit_byte(ModRM(left.encode(), 7));
        _code->emit_word(right);
        return true;
    }

    return false;
}

bool
x86_assembler::cmp(const symbol_ref &left, uint16_t right)
{
    _code->emit_byte(CMP_rm16_imm16);
    _code->emit_byte(ModRM(ModRM_disp16, 7));
    _code->emit_ref({left.sym.address, left.sym.section, left.off});
    _code->emit_word(right);
    return true;
}

bool
x86_assembler::je(const symbol_ref &target)
{
    _code->emit_word(JE_rel16);
    _code->emit_ref({target.sym.address, target.sym.section, -2, true});
    return true;
}

bool
x86_assembler::jmp(const symbol_ref &target)
{
    _code->emit_byte(JMP_rel16);
    _code->emit_ref({target.sym.address, target.sym.section, -2, true});
    return true;
}

bool
x86_assembler::jne(const symbol_ref &target)
{
    _code->emit_word(JNE_rel16);
    _code->emit_ref({target.sym.address, target.sym.section, -2, true});
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, const std::vector<char> &src)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    _code->emit_byte(MOV_reg16_imm16 | _reg_encode(dst));
    _code->emit_ref({_data.emit(src.data(), src.size()), zd4_section_data});
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, const symbol_ref &src)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    _code->emit_byte(MOV_reg16_imm16 | _reg_encode(dst));
    _code->emit_ref({src.sym.address, src.sym.section, src.off});
    return true;
}

bool
x86_assembler::mov(mreg dst, uint16_t src)
{
    _code->emit_byte(MOV_rm16_imm16);
    _code->emit_byte(dst.encode());
    _code->emit_word(src);
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, mreg src)
{
    if (sizeof(uint8_t) == _reg_size(dst))
    {
        _code->emit_byte(MOV_r8_rm8);
        _code->emit_byte(ModRM(src.encode(), _reg_encode(dst)));
        return true;
    }

    if (sizeof(uint16_t) == _reg_size(dst))
    {
        _code->emit_byte(MOV_r16_rm16);
        _code->emit_byte(ModRM(src.encode(), _reg_encode(dst)));
        return true;
    }

    return false;
}

bool
x86_assembler::mov(const symbol_ref &dst, uint16_t src)
{
    _code->emit_byte(MOV_rm16_imm16);
    _code->emit_byte(ModRM(ModRM_disp16, 0));
    _code->emit_ref({dst.sym.address, dst.sym.section, dst.off});
    _code->emit_word(src);
    return true;
}

bool
x86_assembler::mov(mreg dst, par::cpu_register src)
{
    _code->emit_byte(MOV_rm16_r16);
    _code->emit_byte(ModRM(dst.encode(), _reg_encode(src)));
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, unsigned src)
{
    if (sizeof(uint8_t) == _reg_size(dst))
    {
        ASM_REQUIRE(UINT8_MAX >= src);

        _code->emit_byte(MOV_reg8_imm8 | _reg_encode(dst));
        _code->emit_byte(src);
        return true;
    }

    if (sizeof(uint16_t) == _reg_size(dst))
    {
        ASM_REQUIRE(UINT16_MAX >= src);

        _code->emit_byte(MOV_reg16_imm16 | _reg_encode(dst));
        _code->emit_word(src);
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

    _code->emit_byte(ADD_r16_rm16);
    _code->emit_byte(ModRM(modrm, _reg_encode(dst)));
    return true;
}

bool
x86_assembler::add(const symbol_ref &dst, unsigned src)
{
    _code->emit_byte(ADD_rm16_imm16);
    _code->emit_byte(ModRM(ModRM_disp16, 0));
    _code->emit_ref({dst.sym.address, dst.sym.section, dst.off});
    _code->emit_word(src);
    return true;
}

bool
x86_assembler::inc(par::cpu_register reg)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(reg));

    _code->emit_byte(INC_r16 | _reg_encode(reg));
    return true;
}

bool
x86_assembler::inc(mreg reg)
{
    _code->emit_byte(INC_rm16);
    _code->emit_byte(ModRM(reg.encode(), 0));
    return true;
}

bool
x86_assembler::inc(const symbol_ref &dst)
{
    _code->emit_byte(INC_rm16);
    _code->emit_byte(ModRM(ModRM_disp16, 0));
    _code->emit_ref({dst.sym.address, dst.sym.section, dst.off});
    return true;
}

bool
x86_assembler::intr(unsigned num)
{
    ASM_REQUIRE(UINT8_MAX >= num);

    _code->emit_byte(INT_imm8);
    _code->emit_byte(num);
    return true;
}

bool
x86_assembler::ret()
{
    _code->emit_byte(RET_near);
    return true;
}
