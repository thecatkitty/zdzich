#include <cstdio>

#include <zd/par/node.hpp>

using namespace zd;

ustring
par::call_node::to_string()
{
    ustring str{"<call "};
    str.append(_callee);

    for (auto &argument : _arguments)
    {
        str.append(' ');
        str.append(argument->to_string());
    }

    str.append('>');
    return str;
}

ustring
par::declaration_node::to_string()
{
    ustring str{"<declaration "};
    str.append(_target->to_string());
    str.append('>');
    return str;
}

ustring
par::end_node::to_string()
{
    return "<end>";
}

ustring
par::number_node::to_string()
{
    char buff[32];
    std::snprintf(buff, 32, "<number %d>", _value);
    return buff;
}

ustring
par::object_node::to_string()
{
    ustring str{"<variable "};
    str.append(_name);

    str.append(':');
    switch (_type)
    {
    case object_type::text:
        str.append("text");
        break;
    }

    str.append('>');
    return str;
}

ustring
par::string_node::to_string()
{
    ustring str{"<string "};
    str.append(_value);
    str.append('>');
    return str;
}
