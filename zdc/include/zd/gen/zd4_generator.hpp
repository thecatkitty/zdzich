#pragma once

#include <zd/gen/generator.hpp>

namespace zd
{

namespace gen
{

class zd4_generator : public generator
{
  public:
    bool
    process(const par::assignment_node &node) override
    {
        return false;
    }

    bool
    process(const par::call_node &node) override
    {
        return false;
    }

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
};

} // namespace gen

} // namespace zd
