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
    Pisz(zd4_generator *generator, const par::call_node &node);

    static bool
    PiszL(zd4_generator *generator, const par::call_node &node);

private:
    static bool
    Pisz(zd4_generator *generator, const ustring &str);
};

} // namespace gen

} // namespace zd
