#pragma once

#include <list>
#include <vector>

#include <zd/gen/generator.hpp>

namespace zd
{

namespace par
{

struct node;
struct position;
using unique_node = std::unique_ptr<node>;
using node_list = std::list<unique_node>;

enum class object_type
{
    text,
    byte,
    word,
};

enum class condition
{
    equal,
    less_than,
    greater_than,
    nonequal,
    less_or_equal,
    greater_or_equal,
};

enum class operation
{
    add,
    compare,
    subtract,
};

static const uint16_t cpu_register_lbyte = 0x0000;
static const uint16_t cpu_register_hbyte = 0x0100;
static const uint16_t cpu_register_word = 0x0200;
static const uint16_t cpu_register_flag = 0xFF00;

enum class cpu_register : uint16_t
{
    invalid = 0,
    a,
    b,
    c,
    d,
    stack,
    base,
    src,
    dst,
    flag_c = cpu_register_flag,
    flag_d,
    flag_i,
    al = a | cpu_register_lbyte,
    bl = b | cpu_register_lbyte,
    cl = c | cpu_register_lbyte,
    dl = d | cpu_register_lbyte,
    ah = a | cpu_register_hbyte,
    bh = b | cpu_register_hbyte,
    ch = c | cpu_register_hbyte,
    dh = d | cpu_register_hbyte,
    ax = a | cpu_register_word,
    bx = b | cpu_register_word,
    cx = c | cpu_register_word,
    dx = d | cpu_register_word,
    sp = stack | cpu_register_word,
    bp = base | cpu_register_word,
    si = src | cpu_register_word,
    di = dst | cpu_register_word,
};

struct position
{
    const ustring &path;
    const unsigned line;
    const unsigned column;
};

#define IMPLEMENT_GENERATOR_ACCESS                                             \
    virtual bool generate(gen::generator *generator) override                  \
    {                                                                          \
        return generator->process(*this);                                      \
    }

struct node
{
    const ustring *code_path;
    const unsigned code_line;
    const unsigned code_column;

    virtual bool
    generate(gen::generator *generator) = 0;

    virtual ~node() = default;

    template <typename T>
    inline T *
    as()
    {
#ifdef __ia16__
        return reinterpret_cast<T *>(this);
#else
        return dynamic_cast<T *>(this);
#endif
    }

    template <typename T>
    inline bool
    is()
    {
#ifdef __ia16__
        T specimen{};
        return *reinterpret_cast<void ***>(&specimen) ==
               *reinterpret_cast<void ***>(this);
#else
        return !!dynamic_cast<T *>(this);
#endif
    }

  protected:
    node() : code_path{nullptr}, code_line{0}, code_column{0}
    {
    }

    node(const position &pos)
        : code_path{&pos.path}, code_line{pos.line}, code_column{pos.column}
    {
    }
};

struct assignment_node : public node
{
    const unique_node target;
    const unique_node source;

    assignment_node() = default;

    assignment_node(const position &pos,
                    unique_node     target_,
                    unique_node     source_)
        : node{pos}, target{std::move(target_)}, source{std::move(source_)}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct call_node : public node
{
    const ustring   callee;
    const node_list arguments;
    const bool      is_bare;

    call_node(const position &pos,
              const ustring  &callee_,
              node_list       arguments_,
              bool            is_bare_)
        : node{pos}, callee{callee_}, arguments{std::move(arguments_)},
          is_bare{is_bare_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct condition_node : public node
{
    const condition   cond;
    const unique_node action;

    condition_node(const position &pos, condition cond_, unique_node action_)
        : node{pos}, cond{cond_}, action{std::move(action_)}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct declaration_node : public node
{
    const unique_node target;
    const bool        is_const;

    declaration_node(const position &pos, unique_node target_, bool is_const_)
        : node{pos}, target{std::move(target_)}, is_const{is_const_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct emit_node : public node
{
    const std::vector<uint8_t> bytes;

    emit_node(const position &pos, std::vector<uint8_t> bytes_)
        : node{pos}, bytes{std::move(bytes_)}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct end_node : public node
{
    const ustring name;

    end_node() = default;

    end_node(const position &pos) : node{pos}, name{}
    {
    }

    end_node(const position &pos, const ustring &name_) : node{pos}, name{name_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct include_node : public node
{
    const ustring name;
    const bool    is_binary;

    include_node() : name{}, is_binary{false}
    {
    }

    include_node(const position &pos,
                 const ustring  &name_,
                 bool            is_binary_ = false)
        : node{pos}, name{name_}, is_binary{is_binary_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct jump_node : public node
{
    const unique_node target;

    jump_node() = default;

    jump_node(const position &pos, unique_node target_)
        : node{pos}, target{std::move(target_)}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct label_node : public node
{
    const ustring name;

    label_node() = default;

    label_node(const position &pos, const ustring &name_)
        : node{pos}, name{name_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct number_node : public node
{
    const int value;

    number_node() : value{0}
    {
    }

    number_node(const position &pos, int value_) : node{pos}, value{value_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct object_node : public node
{
    const ustring     name;
    const object_type type;

    object_node() : name{}, type{object_type::text}
    {
    }

    object_node(const position &pos, const ustring &name_, object_type type_)
        : node{pos}, name{name_}, type{type_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct operation_node : public node
{
    const operation   op;
    const unique_node left;
    const unique_node right;

    operation_node(const position &pos,
                   operation       op_,
                   unique_node     left_,
                   unique_node     right_)
        : node{pos}, op{op_}, left{std::move(left_)}, right{std::move(right_)}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct procedure_node : public node
{
    const ustring   name;
    const node_list body;

    procedure_node(const position &pos, const ustring &name_, node_list body_)
        : node{pos}, name{name_}, body{std::move(body_)}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct register_node : public node
{
    const cpu_register reg;

    register_node() : reg{cpu_register::invalid}
    {
    }

    register_node(const position &pos, cpu_register reg_) : node{pos}, reg{reg_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct string_node : public node
{
    const ustring value;

    string_node() = default;

    string_node(const position &pos, const ustring &value_)
        : node{pos}, value{value_}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

struct subscript_node : public node
{
    const unique_node value;

    subscript_node() = default;

    subscript_node(const position &pos, unique_node value_)
        : node{pos}, value{std::move(value_)}
    {
    }

    IMPLEMENT_GENERATOR_ACCESS;
};

#undef IMPLEMENT_GENERATOR_ACCESS

} // namespace par

} // namespace zd
