#include <cstdio>

#include <zd/gen/zd4_generator.hpp>
#include <zd/par/node.hpp>
#include <zd/text/pl_string.hpp>

#include "opcodes.hpp"

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

bool
zd4_generator::process(const par::call_node &node)
{
    if (text::pl_streqi("PiszL", node.callee))
    {
        if (node.arguments.size() && node.arguments.front()->is<string_node>())
        {
            auto arg_string =
                reinterpret_cast<string_node *>(node.arguments.front().get());

            ustring value{arg_string->value};
            value.append('\r');
            value.append('\n');
            value.append('$');

            asm_mov(cpu_register::dx, value);
            asm_mov(cpu_register::ah, 9);
            asm_int(0x21);
            return true;
        }

        return false;
    }

    return false;
}

void
zd4_generator::list_relocations() const
{
    std::puts("Relocations:");
    for (auto &reloc : _data_relocs)
    {
        std::printf("\t%04zX\n", reloc);
    }
}

bool
zd4_generator::emit_code(const uint8_t  *code,
                         size_t          size,
                         const uint16_t *refs,
                         size_t          ref_count)
{
    std::printf("C: ");
    for (size_t i = 0; i < size; i++)
    {
        std::printf("%02X ", code[i]);
    }

    for (size_t i = 0; i < ref_count; i++)
    {
        _data_relocs.push_back(_code_offset + refs[i]);
    }

    _code_offset += size;

    std::puts("");
    return true;
}

uint16_t
zd4_generator::emit_data(const uint8_t *data, size_t size)
{
    std::printf("D: ");
    for (size_t i = 0; i < size; i++)
    {
        std::printf("%02X ", data[i]);
    }

    auto offset = _data_offset;
    _data_offset += size;

    std::puts("");
    return offset;
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

    auto     data_offset = emit_data(src.data(), src.size());
    uint8_t  code[]{ASM_BYTE(MOV_reg16_imm16 | _reg_encode(dst)),
                    ASM_WORD(data_offset)};
    uint16_t relocs[]{1};
    emit_code(code, sizeof(code), relocs, 1);

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
        emit_code(code, sizeof(code));

        return true;
    }

    if (sizeof(uint16_t) == _reg_size(dst))
    {
        ASM_REQUIRE(UINT16_MAX >= src);

        uint8_t code[]{ASM_BYTE(MOV_reg16_imm16 | _reg_encode(dst)),
                       ASM_WORD(src)};
        emit_code(code, sizeof(code));

        return true;
    }

    return false;
}

bool
zd4_generator::asm_int(unsigned num)
{
    ASM_REQUIRE(UINT8_MAX >= num);

    uint8_t code[]{ASM_BYTE(0xCD), ASM_BYTE(num)};
    emit_code(code, sizeof(code));

    return true;
}