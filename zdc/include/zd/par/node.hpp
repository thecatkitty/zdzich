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

struct call_node;
struct declaration_node;
struct end_node;
struct object_node;
struct string_node;

enum class object_type
{
    text,
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
};

class call_node : public node
{
    ustring   _callee;
    node_list _arguments;

  public:
    call_node(const ustring &callee, node_list arguments)
        : _callee{callee}, _arguments{std::move(arguments)}
    {
    }

    virtual ustring
    to_string() override;
};

class comparison_node : public node
{
    unique_node _left;
    unique_node _right;

  public:
    comparison_node(unique_node left, unique_node right)
        : _left{std::move(left)}, _right{std::move(right)}
    {
    }

    virtual ustring
    to_string() override;
};

class declaration_node : public node
{
    unique_node _target;

  public:
    declaration_node(unique_node target) : _target{std::move(target)}
    {
    }

    virtual ustring
    to_string() override;
};

class end_node : public node
{
  public:
    virtual ustring
    to_string() override;
};

class label_node : public node
{
    ustring _name;

  public:
    label_node(const ustring &name) : _name{name}
    {
    }

    virtual ustring
    to_string() override;
};

class number_node : public node
{
    int _value;

  public:
    number_node(int value) : _value{value}
    {
    }

    virtual ustring
    to_string() override;
};

class object_node : public node
{
    ustring     _name;
    object_type _type;

  public:
    object_node(const ustring &name, object_type type)
        : _name{name}, _type{type}
    {
    }

    virtual ustring
    to_string() override;
};

class string_node : public node
{
    ustring _value;

  public:
    string_node(const ustring &value) : _value{value}
    {
    }

    virtual ustring
    to_string() override;
};

} // namespace par

} // namespace zd
