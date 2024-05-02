#pragma once

#include <list>
#include <memory>

#include <zd/ustring.hpp>

namespace zd
{

namespace par
{

struct node
{
    virtual ustring
    to_string() = 0;
};

using unique_node = std::unique_ptr<node>;
using node_list = std::list<unique_node>;

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

class end_node : public node
{
  public:
    virtual ustring
    to_string() override;
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

} // namespace par

} // namespace zd
