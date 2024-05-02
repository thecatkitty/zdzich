#include <cstdio>

#include <zd/par/node.hpp>

using namespace zd;

ustring
par::assignment_node::to_string()
{
    ustring str{"<assignment "};
    str.append(_target->to_string());
    str.append(' ');
    str.append(_source->to_string());
    str.append('>');
    return str;
}

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
par::comparison_node::to_string()
{
    ustring str{"<comparison "};
    str.append(_left->to_string());
    str.append(' ');
    str.append(_right->to_string());
    str.append('>');
    return str;
}

ustring
par::condition_node::to_string()
{
    ustring str{"<condition "};
    switch (_condition)
    {
    case condition::equal:
        str.append("eq");
        break;

    case condition::less_than:
        str.append("lt");
        break;

    case condition::greater_than:
        str.append("gt");
        break;

    case condition::nonequal:
        str.append("ne");
        break;

    case condition::less_or_equal:
        str.append("le");
        break;

    case condition::greater_or_equal:
        str.append("ge");
        break;
    }

    str.append(' ');
    str.append(_action->to_string());

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
par::jump_node::to_string()
{
    ustring str{"<jump "};
    str.append(_target->to_string());
    str.append('>');
    return str;
}

ustring
par::label_node::to_string()
{
    ustring str{"<label "};
    str.append(_name);
    str.append('>');
    return str;
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

    case object_type::word:
        str.append("word");
        break;
    }

    str.append('>');
    return str;
}

ustring
par::register_node::to_string()
{
    ustring str{"<register "};

    str.append("ABCDSD"[static_cast<uint16_t>(_reg) & 0xFF]);
    switch (static_cast<uint16_t>(_reg) & 0xFF00)
    {
    case cpu_register_lbyte:
        str.append('L');
        break;

    case cpu_register_hbyte:
        str.append('H');
        break;

    case cpu_register_word:
        str.append(((cpu_register::si == _reg) || (cpu_register::di == _reg))
                       ? 'I'
                       : 'X');
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
