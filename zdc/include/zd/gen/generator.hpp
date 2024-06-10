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
    error, // TODO: define error codes
};

} // namespace gen

} // namespace zd
