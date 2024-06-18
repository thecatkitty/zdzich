#include <zd/par/node.hpp>

using namespace zd;
using namespace zd::par;

#define RETURN_IF_IS(n, type, ret)                                             \
    if ((n).is<type>())                                                        \
    {                                                                          \
        return (ret);                                                          \
    }

const char *
zd::to_cstr(const par::node &node)
{
    RETURN_IF_IS(node, assignment_node, "assignemnt");
    RETURN_IF_IS(node, call_node, "call");
    RETURN_IF_IS(node, condition_node, "condition");
    RETURN_IF_IS(node, declaration_node, "declaration");
    RETURN_IF_IS(node, emit_node, "emit");
    RETURN_IF_IS(node, end_node, "end");
    RETURN_IF_IS(node, include_node, "include");
    RETURN_IF_IS(node, jump_node, "jump");
    RETURN_IF_IS(node, label_node, "label");
    RETURN_IF_IS(node, number_node, "number");
    RETURN_IF_IS(node, object_node, "object");
    RETURN_IF_IS(node, operation_node, "operation");
    RETURN_IF_IS(node, procedure_node, "procedure");
    RETURN_IF_IS(node, register_node, "register");
    RETURN_IF_IS(node, string_node, "string");
    RETURN_IF_IS(node, subscript_node, "subscript");
    return nullptr;
}

const char *
zd::to_cstr(zd::par::operation op)
{
    switch (op)
    {
    case par::operation::add:
        return "add";
    case par::operation::compare:
        return "compare";
    case par::operation::subtract:
        return "subtract";
    }

    return nullptr;
}
