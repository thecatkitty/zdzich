#pragma once

#include <list>

#include <zd/gen/generator.hpp>
#include <zd/par/node.hpp>

namespace zd
{

namespace gen
{

class zd4_generator : public generator
{
    size_t            _code_offset{};
    size_t            _data_offset{};
    std::list<size_t> _data_relocs{};

  public:
    bool
    process(const par::assignment_node &node) override
    {
        return false;
    }

    bool
    process(const par::call_node &node) override;

    bool
    process(const par::condition_node &node) override
    {
        return false;
    }

    bool
    process(const par::declaration_node &node) override
    {
        return false;
    }

    bool
    process(const par::end_node &node) override
    {
        return false;
    }

    bool
    process(const par::emit_node &node) override
    {
        return false;
    }

    bool
    process(const par::include_node &node) override
    {
        return false;
    }

    bool
    process(const par::jump_node &node) override
    {
        return false;
    }

    bool
    process(const par::label_node &node) override
    {
        return false;
    }

    bool
    process(const par::number_node &node) override
    {
        return false;
    }

    bool
    process(const par::object_node &node) override
    {
        return false;
    }

    bool
    process(const par::operation_node &node) override
    {
        return false;
    }

    bool
    process(const par::procedure_node &node) override
    {
        return false;
    }

    bool
    process(const par::register_node &node) override
    {
        return false;
    }

    bool
    process(const par::string_node &node) override
    {
        return false;
    }

    bool
    process(const par::subscript_node &node) override
    {
        return false;
    }

    void
    list_relocations() const;

  private:
    bool
    emit_code(const uint8_t  *code,
              size_t          size,
              const uint16_t *refs = nullptr,
              size_t          ref_count = 0);

    uint16_t
    emit_data(const uint8_t *data, size_t size);

    inline uint16_t
    emit_data(const char *data, size_t size)
    {
        return emit_data(reinterpret_cast<const uint8_t *>(data), size);
    }

    bool
    asm_mov(par::cpu_register dst, const ustring &src);

    bool
    asm_mov(par::cpu_register dst, unsigned src);

    bool
    asm_int(unsigned num);
};

} // namespace gen

} // namespace zd
