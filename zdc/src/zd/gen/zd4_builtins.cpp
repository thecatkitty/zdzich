#include <zd/gen/zd4_builtins.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

bool
zd4_builtins::PiszL(zd4_generator *generator, const call_node &node)
{
    if (node.arguments.size() && node.arguments.front()->is<string_node>())
    {
        auto arg_string =
            reinterpret_cast<string_node *>(node.arguments.front().get());

        ustring value{arg_string->value};
        value.append('\r');
        value.append('\n');
        value.append('$');

        generator->asm_mov(cpu_register::dx, value);
        generator->asm_mov(cpu_register::ah, 9);
        generator->asm_int(0x21);
        return true;
    }

    return false;
}
