#pragma once

#include <cstdio>

#include <zd/gen/generator.hpp>

namespace zd
{

namespace gen
{

class text_generator : public generator
{
    FILE *_out;

  public:
    text_generator(FILE *out) : _out{out}
    {
    }

    bool
    process(const par::assignment_node &node) override;

    bool
    process(const par::call_node &node) override;

    bool
    process(const par::condition_node &node) override;

    bool
    process(const par::declaration_node &node) override;

    bool
    process(const par::end_node &node) override;

    bool
    process(const par::jump_node &node) override;

    bool
    process(const par::label_node &node) override;

    bool
    process(const par::number_node &node) override;

    bool
    process(const par::object_node &node) override;

    bool
    process(const par::operation_node &node) override;

    bool
    process(const par::procedure_node &node) override;

    bool
    process(const par::register_node &node) override;

    bool
    process(const par::string_node &node) override;
};

} // namespace gen

} // namespace zd
