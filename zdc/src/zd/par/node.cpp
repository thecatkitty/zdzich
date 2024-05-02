#include <zd/par/node.hpp>

using namespace zd;

ustring
par::string_node::to_string()
{
    ustring str{"<string "};
    str.append(_value);
    str.append('>');
    return str;
}

ustring
par::end_node::to_string()
{
    return "<end>";
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
