#pragma once

#include <zd.hpp>

namespace zd
{

namespace par
{

struct assignment_node;
struct call_node;
struct condition_node;
struct declaration_node;
struct end_node;
struct emit_node;
struct include_node;
struct jump_node;
struct label_node;
struct number_node;
struct object_node;
struct operation_node;
struct procedure_node;
struct register_node;
struct string_node;
struct subscript_node;

} // namespace par

namespace gen
{

// Output generator interface
struct generator
{
    virtual error
    process(const par::assignment_node &node) = 0;

    virtual error
    process(const par::call_node &node) = 0;

    virtual error
    process(const par::condition_node &node) = 0;

    virtual error
    process(const par::declaration_node &node) = 0;

    virtual error
    process(const par::end_node &node) = 0;

    virtual error
    process(const par::emit_node &node) = 0;

    virtual error
    process(const par::include_node &node) = 0;

    virtual error
    process(const par::jump_node &node) = 0;

    virtual error
    process(const par::label_node &node) = 0;

    virtual error
    process(const par::number_node &node) = 0;

    virtual error
    process(const par::object_node &node) = 0;

    virtual error
    process(const par::operation_node &node) = 0;

    virtual error
    process(const par::procedure_node &node) = 0;

    virtual error
    process(const par::register_node &node) = 0;

    virtual error
    process(const par::string_node &node) = 0;

    virtual error
    process(const par::subscript_node &node) = 0;
};

enum class generator_error
{
    unexpected_node = 0x01,      // unexpected X
    invalid_operands = 0x02,     // cannot X Y and Z
    invalid_assignment = 0x03,   // cannot assign X to Y
    nonconst_assignment = 0x04,  // cannot assign variable at declaration
    unexpected_arguments = 0x05, // cannot invoke X with arguments
    symbol_redefinition = 0x06,  // cannot redefine X
    string_too_long = 0x07,      // string constant is too long
    invalid_argument = 0x08,     // invalid argument
    assembler_error = 0x80,
    not_a_builtin = 0x81,
    undefined_name = 0x82,
};

} // namespace gen

} // namespace zd
