#pragma once

#include <list>

#include <zd/gen/generator.hpp>
#include <zd/gen/zd4_section.hpp>
#include <zd/par/node.hpp>

namespace zd
{

namespace gen
{

class zd4_generator : public generator
{
    zd4_section _code;
    zd4_section _data;

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
    process(const par::end_node &node) override;

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
    link();

  private:
    bool
    asm_mov(par::cpu_register dst, const ustring &src);

    bool
    asm_mov(par::cpu_register dst, unsigned src);

    bool
    asm_int(unsigned num);
};

} // namespace gen

} // namespace zd
