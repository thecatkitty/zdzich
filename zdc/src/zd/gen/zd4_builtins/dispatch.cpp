#include <zd/gen/zd4_builtins.hpp>
#include <zd/text/pl_string.hpp>

using namespace zd;
using namespace zd::gen;
using namespace zd::par;

#define RETURN_IF_FALSE(expr)                                                  \
    if (!(expr))                                                               \
    {                                                                          \
        return false;                                                          \
    }

static bool
_matches_name(const char *signature, const ustring &str)
{
    ustring name{};
    for (auto ch : ustring{signature})
    {
        if ('_' == ch)
        {
            break;
        }

        name.append(ch);
    }

    return text::pl_streqai(name, str);
}

static bool
_matches_args(const char *signature, const node_list &args)
{
    auto arg_types = std::strchr(signature, '_');
    if (!arg_types)
    {
        return args.empty();
    }

    arg_types++;
    if (std::strlen(arg_types) != args.size())
    {
        return false;
    }

    for (auto &arg : args)
    {
        switch (*arg_types)
        {
        case 'b':
        case 'w':
            RETURN_IF_FALSE(
                arg->is<number_node>() || arg->is<register_node>() ||
                (arg->is<object_node>() &&
                 (object_type::text != arg->as<object_node>()->type)));
            break;

        case 'f':
            RETURN_IF_FALSE(
                arg->is<subscript_node>() &&
                arg->as<subscript_node>()->value->is<number_node>());
            break;

        case 't':
            RETURN_IF_FALSE(
                arg->is<string_node>() ||
                (arg->is<object_node>() &&
                 (object_type::text == arg->as<object_node>()->type)));
            break;

        case 'T':
            RETURN_IF_FALSE(
                arg->is<object_node>() &&
                (object_type::text == arg->as<object_node>()->type));
            break;

        default:
            assert(false && "unknown argument type");
        }

        arg_types++;
    }

    return true;
}

static bool
_matches(const char *signature, const ustring &name, const node_list &args)
{
    return _matches_name(signature, name) && _matches_args(signature, args);
}

template <typename T>
static T
get_arg(zd4_builtins *this_, node &arg) = delete;

template <>
zd4_byte
get_arg<zd4_byte>(zd4_builtins *this_, node &arg)
{
    if (arg.is<number_node>())
    {
        return zd4_byte(arg.as<number_node>()->value, &arg);
    }

    if (arg.is<register_node>())
    {
        return {arg.as<register_node>()->reg, &arg};
    }

    if (arg.is<object_node>())
    {
        return {&this_->gen.get_symbol(arg.as<object_node>()->name), &arg};
    }

    assert(false && "unexpected argument node type for a byte");
    return 0xFF;
}

template <>
zd4_file
get_arg<zd4_file>(zd4_builtins *this_, node &arg)
{
    return zd4_file(arg.as<subscript_node>()->value->as<number_node>()->value,
                    &arg);
}

template <>
zd4_text
get_arg<zd4_text>(zd4_builtins *this_, node &arg)
{
    if (arg.is<string_node>())
    {
        return {&arg.as<string_node>()->value, &arg};
    }

    if (arg.is<object_node>())
    {
        return {&this_->gen.get_symbol(arg.as<object_node>()->name), &arg};
    }

    assert(false && "unexpected argument node type for a text");
    return static_cast<symbol *>(nullptr);
}

template <>
zd4_word
get_arg<zd4_word>(zd4_builtins *this_, node &arg)
{
    if (arg.is<number_node>())
    {
        return zd4_word(arg.as<number_node>()->value, &arg);
    }

    if (arg.is<register_node>())
    {
        return {arg.as<register_node>()->reg, &arg};
    }

    if (arg.is<object_node>())
    {
        return {&this_->gen.get_symbol(arg.as<object_node>()->name), &arg};
    }

    assert(false && "unexpected argument node type for a word");
    return 0xFFFF;
}

static error
invoke(zd4_builtins *this_, error (zd4_builtins::*mfp)(), const node_list &args)
{
    return (this_->*mfp)();
}

template <typename Arg1>
static error
invoke(zd4_builtins    *this_,
       error            (zd4_builtins::*mfp)(Arg1),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **it);
    return (this_->*mfp)(arg1);
}

template <typename Arg1, typename Arg2>
static error
invoke(zd4_builtins    *this_,
       error            (zd4_builtins::*mfp)(Arg1, Arg2),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **(it++));
    auto arg2 = get_arg<std::decay_t<Arg2>>(this_, **it);
    return (this_->*mfp)(arg1, arg2);
}

template <typename Arg1, typename Arg2, typename Arg3>
static error
invoke(zd4_builtins    *this_,
       error            (zd4_builtins::*mfp)(Arg1, Arg2, Arg3),
       const node_list &args)
{
    auto it = args.begin();
    auto arg1 = get_arg<std::decay_t<Arg1>>(this_, **(it++));
    auto arg2 = get_arg<std::decay_t<Arg2>>(this_, **(it++));
    auto arg3 = get_arg<std::decay_t<Arg3>>(this_, **it);
    return (this_->*mfp)(arg1, arg2, arg3);
}

#define INVOKE_IF_MATCH(mf)                                                    \
    {                                                                          \
        auto __signature = #mf;                                                \
        if (_matches(__signature, node.callee, node.arguments))                \
        {                                                                      \
            return invoke(this, &zd4_builtins::mf, node.arguments);            \
        }                                                                      \
    }

error
zd4_builtins::dispatch(const call_node &node)
{
    _node = &node;

    INVOKE_IF_MATCH(Czekaj_w);
    INVOKE_IF_MATCH(Czysc);
    INVOKE_IF_MATCH(Czysc_b);
    INVOKE_IF_MATCH(Czytaj_T);
    INVOKE_IF_MATCH(DoPortu);
    INVOKE_IF_MATCH(Klawisz);
    INVOKE_IF_MATCH(Laduj);
    INVOKE_IF_MATCH(Losowa16);
    INVOKE_IF_MATCH(Losowa8);
    INVOKE_IF_MATCH(Nic);
    INVOKE_IF_MATCH(Otworz_ft);
    INVOKE_IF_MATCH(Otworz_ftb);
    INVOKE_IF_MATCH(Pisz);
    INVOKE_IF_MATCH(Pisz_t);
    INVOKE_IF_MATCH(Pisz_tt);
    INVOKE_IF_MATCH(Pisz_f);
    INVOKE_IF_MATCH(Pisz_ft);
    INVOKE_IF_MATCH(Pisz_ftt);
    INVOKE_IF_MATCH(Pisz_w);
    INVOKE_IF_MATCH(Pisz_fw);
    INVOKE_IF_MATCH(PiszL);
    INVOKE_IF_MATCH(PiszL_t);
    INVOKE_IF_MATCH(PiszL_tt);
    INVOKE_IF_MATCH(PiszL_f);
    INVOKE_IF_MATCH(PiszL_ft);
    INVOKE_IF_MATCH(PiszL_ftt);
    INVOKE_IF_MATCH(PiszL_w);
    INVOKE_IF_MATCH(PiszL_fw);
    INVOKE_IF_MATCH(Pisz8);
    INVOKE_IF_MATCH(Pisz8_b);
    INVOKE_IF_MATCH(Pisz8_t);
    INVOKE_IF_MATCH(PiszZnak_b);
    INVOKE_IF_MATCH(PiszZnak_bb);
    INVOKE_IF_MATCH(PiszZnak_bbw);
    INVOKE_IF_MATCH(Pozycja_bb);
    INVOKE_IF_MATCH(PokazMysz);
    INVOKE_IF_MATCH(Przerwanie_b);
    INVOKE_IF_MATCH(Punkt_wwb);
    INVOKE_IF_MATCH(StanPrzyciskow);
    INVOKE_IF_MATCH(StanPrzyciskow_t);
    INVOKE_IF_MATCH(Tryb_b);
    INVOKE_IF_MATCH(TworzKatalog_t);
    INVOKE_IF_MATCH(TworzPlik_t);
    INVOKE_IF_MATCH(UkryjMysz);
    INVOKE_IF_MATCH(UsunKatalog_t);
    INVOKE_IF_MATCH(UsunPlik_t);
    INVOKE_IF_MATCH(Zamknij_f);
    INVOKE_IF_MATCH(ZmienKatalog_t);
    INVOKE_IF_MATCH(ZmienNazwe_tt);
    INVOKE_IF_MATCH(ZmienNazwe_t);
    INVOKE_IF_MATCH(ZPortu);

    return error{gen, zd4_generator::error_code::not_a_builtin};
}
