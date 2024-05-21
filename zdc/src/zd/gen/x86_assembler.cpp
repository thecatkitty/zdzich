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
_to_mode(cpu_register reg)
{
    switch (reg)
    {
    case cpu_register::al:
    case cpu_register::ax:
        return ModRM_A;

    case cpu_register::bl:
    case cpu_register::bx:
        return ModRM_B;

    case cpu_register::cl:
    case cpu_register::cx:
        return ModRM_C;

    case cpu_register::dl:
    case cpu_register::dx:
        return ModRM_D;

    case cpu_register::ah:
    case cpu_register::sp:
        return ModRM_SPAH;

    case cpu_register::ch:
    case cpu_register::bp:
        return ModRM_BPCH;

    case cpu_register::dh:
    case cpu_register::si:
        return ModRM_SIDH;

    case cpu_register::bh:
    case cpu_register::di:
        return ModRM_DIBH;
    }

    return 0xFF;
}

static uint8_t
_to_regopcode(cpu_register reg)
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
zd::gen::x86_assembler::dec(const symbol_ref &dst)
{
    _code->emit_byte(DEC_rm16);
    _code->emit_byte(ModRM(ModRM_disp16, 1));
    _code->emit_ref({dst.sym.address, dst.sym.section, dst.off});
    return true;
}

bool
zd::gen::x86_assembler::inb()
{
    _code->emit_byte(IN_AL_DX);
    return true;
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
x86_assembler::jb(const symbol_ref &target)
{
    _code->emit_word(JB_rel16);
    _code->emit_ref({target.sym.address, target.sym.section, -2, true});
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
zd::gen::x86_assembler::mov(par::cpu_register dst, par::cpu_register src)
{
    ASM_REQUIRE(_reg_size(src) == _reg_size(dst));

    _code->emit_byte((sizeof(uint8_t) == _reg_size(dst)) ? MOV_rm8_r8
                                                         : MOV_rm16_r16);
    _code->emit_byte(ModRM(_to_mode(dst), _to_regopcode(src)));
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, const std::vector<char> &src)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    _code->emit_byte(MOV_reg16_imm16 | _to_regopcode(dst));
    _code->emit_ref({_data.emit(src.data(), src.size()), zd4_section_data});
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, const symbol_ref &src)
{
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    _code->emit_byte(MOV_reg16_imm16 | _to_regopcode(dst));
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
        _code->emit_byte(ModRM(src.encode(), _to_regopcode(dst)));
        return true;
    }

    if (sizeof(uint16_t) == _reg_size(dst))
    {
        _code->emit_byte(MOV_r16_rm16);
        _code->emit_byte(ModRM(src.encode(), _to_regopcode(dst)));
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
    _code->emit_byte(ModRM(dst.encode(), _to_regopcode(src)));
    return true;
}

bool
x86_assembler::mov(par::cpu_register dst, unsigned src)
{
    if (sizeof(uint8_t) == _reg_size(dst))
    {
        ASM_REQUIRE(UINT8_MAX >= src);

        _code->emit_byte(MOV_reg8_imm8 | _to_regopcode(dst));
        _code->emit_byte(src);
        return true;
    }

    if (sizeof(uint16_t) == _reg_size(dst))
    {
        ASM_REQUIRE(UINT16_MAX >= src);

        _code->emit_byte(MOV_reg16_imm16 | _to_regopcode(dst));
        _code->emit_word(src);
        return true;
    }

    return false;
}

bool
zd::gen::x86_assembler::outb()
{
    _code->emit_byte(OUT_DX_AL);
    return true;
}

bool
x86_assembler::add(par::cpu_register dst, par::cpu_register src)
{
    ASM_REQUIRE(_reg_size(dst) == _reg_size(src));
    ASM_REQUIRE(sizeof(uint16_t) == _reg_size(dst));

    _code->emit_byte(ADD_r16_rm16);
    _code->emit_byte(ModRM(_to_mode(src), _to_regopcode(dst)));
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

    _code->emit_byte(INC_r16 | _to_regopcode(reg));
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

bool
zd::gen::x86_assembler::sub(const symbol_ref &dst, unsigned src)
{
    _code->emit_byte(SUB_rm16_imm16);
    _code->emit_byte(ModRM(ModRM_disp16, 5));
    _code->emit_ref({dst.sym.address, dst.sym.section, dst.off});
    _code->emit_word(src);
    return true;
}
