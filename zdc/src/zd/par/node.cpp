#include <zd/par/node.hpp>

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
