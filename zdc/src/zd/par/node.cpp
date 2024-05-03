#include <cstdio>

#include <zd/par/node.hpp>

using namespace zd;

ustring
par::assignment_node::to_string()
{
    ustring str{"<assignment "};
    str.append(target->to_string());
    str.append(' ');
    str.append(source->to_string());
    str.append('>');
    return str;
}

ustring
par::call_node::to_string()
{
    ustring str{"<call "};
    str.append(callee);

    for (auto &argument : arguments)
    {
        str.append(' ');
        str.append(argument->to_string());
    }

    str.append('>');
    return str;
}

ustring
par::operation_node::to_string()
{
    ustring str{"<operation "};
    switch (op)
    {
    case operation::add:
        str.append("add");
        break;

    case operation::compare:
        str.append("cmp");
        break;

    case operation::subtract:
        str.append("sub");
        break;
    }

    str.append(' ');
    str.append(left->to_string());
    str.append(' ');
    str.append(right->to_string());
    str.append('>');
    return str;
}

ustring
par::condition_node::to_string()
{
    ustring str{"<condition "};
    switch (cond)
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
    str.append(action->to_string());

    str.append('>');
    return str;
}

ustring
par::declaration_node::to_string()
{
    ustring str{"<declaration "};
    str.append(target->to_string());
    str.append('>');
    return str;
}

ustring
par::end_node::to_string()
{
    if (name.empty())
    {
        return "<end>";
    }

    ustring str{"<end "};
    str.append(name);
    str.append('>');
    return str;
}

ustring
par::jump_node::to_string()
{
    ustring str{"<jump "};
    str.append(target->to_string());
    str.append('>');
    return str;
}

ustring
par::label_node::to_string()
{
    ustring str{"<label "};
    str.append(name);
    str.append('>');
    return str;
}

ustring
par::number_node::to_string()
{
    char buff[32];
    std::snprintf(buff, 32, "<number %d>", value);
    return buff;
}

ustring
par::object_node::to_string()
{
    ustring str{"<variable "};
    str.append(name);

    str.append(':');
    switch (type)
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
par::procedure_node::to_string()
{
    ustring str{"<procedure "};
    str.append(name);

    for (auto &item : body)
    {
        str.append(' ');
        str.append(item->to_string());
    }

    str.append('>');
    return str;
}

ustring
par::register_node::to_string()
{
    ustring str{"<register "};

    str.append("ABCDSD"[static_cast<uint16_t>(reg) & 0xFF]);
    switch (static_cast<uint16_t>(reg) & 0xFF00)
    {
    case cpu_register_lbyte:
        str.append('L');
        break;

    case cpu_register_hbyte:
        str.append('H');
        break;

    case cpu_register_word:
        str.append(((cpu_register::si == reg) || (cpu_register::di == reg))
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
    str.append(value);
    str.append('>');
    return str;
}
