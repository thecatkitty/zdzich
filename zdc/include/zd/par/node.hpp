#pragma once

#include <list>
#include <memory>

#include <zd/ustring.hpp>

namespace zd
{

namespace par
{

struct node;
using unique_node = std::unique_ptr<node>;
using node_list = std::list<unique_node>;

struct assignment_node;
struct call_node;
struct condition_node;
struct declaration_node;
struct end_node;
struct jump_node;
struct label_node;
struct number_node;
struct object_node;
struct operation_node;
struct procedure_node;
struct register_node;
struct string_node;

enum class object_type
{
    text,
    word,
};

enum class condition
{
    equal,
    less_than,
    greater_than,
    nonequal,
    less_or_equal,
    greater_or_equal,
};

enum class operation
{
    add,
    compare,
    subtract,
};

static const uint16_t cpu_register_lbyte = 0x0000;
static const uint16_t cpu_register_hbyte = 0x0100;
static const uint16_t cpu_register_word = 0x0200;

enum class cpu_register : uint16_t
{
    a,
    b,
    c,
    d,
    src,
    dst,
    al = a | cpu_register_lbyte,
    bl = b | cpu_register_lbyte,
    cl = c | cpu_register_lbyte,
    dl = d | cpu_register_lbyte,
    ah = a | cpu_register_hbyte,
    bh = b | cpu_register_hbyte,
    ch = c | cpu_register_hbyte,
    dh = d | cpu_register_hbyte,
    ax = a | cpu_register_word,
    bx = b | cpu_register_word,
    cx = c | cpu_register_word,
    dx = d | cpu_register_word,
    si = src | cpu_register_word,
    di = dst | cpu_register_word,
};

struct node
{
    virtual ustring
    to_string() = 0;

    template <typename T>
    inline bool
    is()
    {
#ifdef __ia16__
        T specimen{};
        return *reinterpret_cast<void ***>(&specimen) ==
               *reinterpret_cast<void ***>(this);
#else
        return !!dynamic_cast<T *>(this);
#endif
    }
};

struct assignment_node : public node
{
    const unique_node target;
    const unique_node source;

    assignment_node(unique_node target_, unique_node source_)
        : target{std::move(target_)}, source{std::move(source_)}
    {
    }

    virtual ustring
    to_string() override;
};

struct call_node : public node
{
    const ustring   callee;
    const node_list arguments;

    call_node(const ustring &callee_, node_list arguments_)
        : callee{callee_}, arguments{std::move(arguments_)}
    {
    }

    virtual ustring
    to_string() override;
};

struct condition_node : public node
{
    const condition   cond;
    const unique_node action;

    condition_node(condition cond_, unique_node action_)
        : cond{cond_}, action{std::move(action_)}
    {
    }

    virtual ustring
    to_string() override;
};

struct declaration_node : public node
{
    const unique_node target;

    declaration_node(unique_node target_) : target{std::move(target_)}
    {
    }

    virtual ustring
    to_string() override;
};

struct end_node : public node
{
    const ustring name;

    end_node() = default;

    end_node(const ustring &name_) : name{name_}
    {
    }

    virtual ustring
    to_string() override;
};

struct jump_node : public node
{
    const unique_node target;

    jump_node(unique_node target_) : target{std::move(target_)}
    {
    }

    virtual ustring
    to_string() override;
};

struct label_node : public node
{
    const ustring name;

    label_node(const ustring &name_) : name{name_}
    {
    }

    virtual ustring
    to_string() override;
};

struct number_node : public node
{
    const int value;

    number_node(int value_) : value{value_}
    {
    }

    virtual ustring
    to_string() override;
};

struct object_node : public node
{
    const ustring     name;
    const object_type type;

    object_node(const ustring &name_, object_type type_)
        : name{name_}, type{type_}
    {
    }

    virtual ustring
    to_string() override;
};

struct operation_node : public node
{
    const operation   op;
    const unique_node left;
    const unique_node right;

    operation_node(operation op_, unique_node left_, unique_node right_)
        : op{op_}, left{std::move(left_)}, right{std::move(right_)}
    {
    }

    virtual ustring
    to_string() override;
};

struct procedure_node : public node
{
    const ustring   name;
    const node_list body;

    procedure_node(const ustring &name_, node_list body_)
        : name{name_}, body{std::move(body_)}
    {
    }

    virtual ustring
    to_string() override;
};

struct register_node : public node
{
    const cpu_register reg;

    register_node(cpu_register reg_) : reg{reg_}
    {
    }

    virtual ustring
    to_string() override;
};

struct string_node : public node
{
    const ustring value;

    string_node(const ustring &value_) : value{value_}
    {
    }

    virtual ustring
    to_string() override;
};

} // namespace par

} // namespace zd
