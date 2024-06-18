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

struct _builtin
{
    const char *name;
    const char *args;
    bool        has_diacritics;
    error (zd4_builtins::*handler)();

    bool
    matches_name(const ustring &name) const;

    bool
    matches_args(const node_list &args) const;

    bool
    matches(const ustring &name, const node_list &args) const
    {
        return matches_name(name) && matches_args(args);
    }
};

bool
_builtin::matches_name(const ustring &name) const
{
    if (has_diacritics)
    {
        RETURN_IF_FALSE(text::pl_streqai(this->name, name.data()));
    }
    else
    {
        RETURN_IF_FALSE(text::pl_streqi(this->name, name.data()));
    }

    return true;
}

bool
_builtin::matches_args(const node_list &args) const
{
    if (nullptr == this->args)
    {
        return args.empty();
    }

    RETURN_IF_FALSE(std::strlen(this->args) == args.size());
    auto arg_types = this->args;

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

static zd4_byte
_get_byte(zd4_builtins *this_, node &arg)
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

static zd4_file
_get_file(zd4_builtins *this_, node &arg)
{
    return zd4_file(arg.as<subscript_node>()->value->as<number_node>()->value,
                    &arg);
}

static zd4_text
_get_text(zd4_builtins *this_, node &arg)
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

static zd4_word
_get_word(zd4_builtins *this_, node &arg)
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

struct _arg
{
    char buff[std::max({sizeof(zd4_byte), sizeof(zd4_file), sizeof(zd4_text),
                        sizeof(zd4_word)})];

    _arg(char type, zd4_builtins *bs, node &n)
    {
        switch (type)
        {
        case 'b':
            *this = _get_byte(bs, n);
            break;
        case 'f':
            *this = _get_file(bs, n);
            break;
        case 't':
        case 'T':
            *this = _get_text(bs, n);
            break;
        case 'w':
            *this = _get_word(bs, n);
            break;
        default:
            assert(false && "unhandled argument type");
        }
    }

    template <typename T>
    inline T &
    operator=(const T &that)
    {
        return *reinterpret_cast<T *>(buff) = that;
    }

    inline zd4_arg &
    operator*()
    {
        return *reinterpret_cast<zd4_arg *>(buff);
    }
};

static error
_invoke(zd4_builtins *this_,
        const char   *types,
        error (zd4_builtins::*mfp)(zd4_arg &, zd4_arg &, zd4_arg &),
        const node_list &args)
{
    assert(3 == args.size());

    auto it = args.begin();
    _arg arg1{types[0], this_, **(it++)};
    _arg arg2{types[1], this_, **(it++)};
    _arg arg3{types[2], this_, **it};
    return (this_->*mfp)(*arg1, *arg2, *arg3);
}

static error
_invoke(zd4_builtins *this_,
        const char   *types,
        error (zd4_builtins::*mfp)(zd4_arg &, zd4_arg &),
        const node_list &args)
{
    if (2 < args.size())
    {
        return _invoke(this_, types,
                       reinterpret_cast<error (zd4_builtins::*)(
                           zd4_arg &, zd4_arg &, zd4_arg &)>(mfp),
                       args);
    }

    auto it = args.begin();
    _arg arg1{types[0], this_, **(it++)};
    _arg arg2{types[1], this_, **it};
    return (this_->*mfp)(*arg1, *arg2);
}

static error
_invoke(zd4_builtins *this_,
        const char   *types,
        error (zd4_builtins::*mfp)(zd4_arg &),
        const node_list &args)
{
    if (1 < args.size())
    {
        return _invoke(
            this_, types,
            reinterpret_cast<error (zd4_builtins::*)(zd4_arg &, zd4_arg &)>(
                mfp),
            args);
    }

    auto it = args.begin();
    _arg arg1{types[0], this_, **it};
    return (this_->*mfp)(*arg1);
}

static error
_invoke(zd4_builtins *this_,
        const char   *types,
        error (zd4_builtins::*mfp)(),
        const node_list &args)
{
    if (0 < args.size())
    {
        return _invoke(
            this_, types,
            reinterpret_cast<error (zd4_builtins::*)(zd4_arg &)>(mfp), args);
    }

    return (this_->*mfp)();
}

#define SIGNATURE(name, args, hasd)                                            \
    {                                                                          \
        #name, #args, hasd,                                                    \
            reinterpret_cast<decltype(_builtin::handler)>(                     \
                &zd4_builtins::name##_##args)                                  \
    }

#define SIGNATURE_NA(name, hasd)                                               \
    {                                                                          \
        #name, nullptr, hasd, &zd4_builtins::name                              \
    }

error
zd4_builtins::dispatch(const call_node &node)
{
    _node = &node;

    // clang-format off
    const static _builtin builtins[]{
        SIGNATURE(Czekaj, w, false),
        SIGNATURE_NA(Czysc, true),
        SIGNATURE(Czysc, b, true),
        SIGNATURE(Czytaj, T, false),
        SIGNATURE_NA(DoPortu, false),
        SIGNATURE_NA(Klawisz, false),
        SIGNATURE_NA(Laduj, true),
        SIGNATURE_NA(Losowa16, false),
        SIGNATURE_NA(Losowa8, false),
        SIGNATURE_NA(Nic, false),
        SIGNATURE(Otworz, ft, true),
        SIGNATURE(Otworz, ftb, true),
        SIGNATURE_NA(Pisz, false),
        SIGNATURE(Pisz, t, false),
        SIGNATURE(Pisz, tt, false),
        SIGNATURE(Pisz, f, false),
        SIGNATURE(Pisz, ft, false),
        SIGNATURE(Pisz, ftt, false),
        SIGNATURE(Pisz, w, false),
        SIGNATURE(Pisz, fw, false),
        SIGNATURE_NA(PiszL, false),
        SIGNATURE(PiszL, t, false),
        SIGNATURE(PiszL, tt, false),
        SIGNATURE(PiszL, f, false),
        SIGNATURE(PiszL, ft, false),
        SIGNATURE(PiszL, ftt, false),
        SIGNATURE(PiszL, w, false),
        SIGNATURE(PiszL, fw, false),
        SIGNATURE_NA(Pisz8, false),
        SIGNATURE(Pisz8, b, false),
        SIGNATURE(Pisz8, t, false),
        SIGNATURE(PiszZnak, b, false),
        SIGNATURE(PiszZnak, bb, false),
        SIGNATURE(PiszZnak, bbw, false),
        SIGNATURE(Pozycja, bb, false),
        SIGNATURE_NA(PokazMysz, true),
        SIGNATURE(Przerwanie, b, false),
        SIGNATURE(Punkt, wwb, false),
        SIGNATURE_NA(StanPrzyciskow, true),
        SIGNATURE(StanPrzyciskow, t, true),
        SIGNATURE(Tryb, b, false),
        SIGNATURE(TworzKatalog, t, true),
        SIGNATURE(TworzPlik, t, true),
        SIGNATURE_NA(UkryjMysz, false),
        SIGNATURE(UsunKatalog, t, true),
        SIGNATURE(UsunPlik, t, true),
        SIGNATURE(Zamknij, f, false),
        SIGNATURE(ZmienKatalog, t, true),
        SIGNATURE(ZmienNazwe, tt, true),
        SIGNATURE(ZmienNazwe, t, true),
        SIGNATURE_NA(ZPortu, false),
    };
    // clang-format on

    for (auto &signature : builtins)
    {
        if (signature.matches(node.callee, node.arguments))
        {
            return _invoke(this, signature.args, signature.handler,
                           node.arguments);
        }
    }

    return error{gen, zd4_generator::error_code::not_a_builtin};
}
