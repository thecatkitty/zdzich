#pragma once

#include <zd/gen/zd4_generator.hpp>
#include <zd/par/node.hpp>

namespace zd
{

namespace gen
{

class zd4_generator;

struct zd4_builtins
{
    static bool
    PiszL(zd4_generator *generator, const par::call_node &node);
};

} // namespace gen

} // namespace zd
