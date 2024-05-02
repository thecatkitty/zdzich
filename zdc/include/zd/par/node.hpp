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

} // namespace par

} // namespace zd
