#pragma once

#include <tl/expected.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <utility>

#include <zd/ustring.hpp>

namespace zd
{

template <typename T> struct error_origin_traits
{
    static const auto origin_tag = T::error_origin_tag;

    using ordinal_type = typename T::error_code;

    static const ustring &
    path(const T &obj)
    {
        return obj.get_path();
    }

    static unsigned
    line(const T &obj)
    {
        return obj.get_line();
    }

    static unsigned
    column(const T &obj)
    {
        return obj.get_column();
    }
};

class error
{
    uint8_t    _origin;
    uint8_t    _ordinal;
    ustring    _file;
    unsigned   _line;
    unsigned   _column;
    size_t     _argc;
    uintptr_t *_argv;

    template <typename Head>
    inline static void
    to_argv(uintptr_t *argv, Head *head)
    {
        *argv = reinterpret_cast<uintptr_t>(head);
    }

    template <typename Head>
    inline static void
    to_argv(uintptr_t *argv, Head head)
    {
        *argv = static_cast<uintptr_t>(head);
    }

    inline static void
    to_argv(uintptr_t *argv, char head)
    {
        *argv = static_cast<uintptr_t>(head & 0xFF);
    }

    template <typename Head, typename... Tail>
    inline static void
    to_argv(uintptr_t *argv, Head head, Tail... tail)
    {
        to_argv(argv, head);
        to_argv(argv + 1, tail...);
    }

  public:
    template <typename Torigin, typename Traits = error_origin_traits<Torigin>>
    error(const Torigin &origin, typename Traits::ordinal_type code)
        : _origin{static_cast<uint8_t>(Traits::origin_tag)},
          _ordinal{static_cast<uint8_t>(code)}, _file{Traits::path(origin)},
          _line{Traits::line(origin)}, _column{Traits::column(origin)},
          _argc{0}, _argv{nullptr}
    {
    }

    template <typename Torigin,
              typename Traits = error_origin_traits<Torigin>,
              typename... Args>
    error(const Torigin                &origin,
          typename Traits::ordinal_type code,
          Args... args)
        : error{origin, code}
    {
        _argc = sizeof...(args);
        _argv = new (std::nothrow) uintptr_t[sizeof...(args)];

        to_argv(_argv, args...);
    }

    error(const error &) = delete;

    error(error &&that) noexcept
        : _origin{that._origin}, _ordinal{that._ordinal},
          _file{std::move(that._file)}, _line{that._line},
          _column{that._column}, _argc{std::exchange(that._argc, 0)},
          _argv{std::exchange(that._argv, nullptr)}
    {
    }

    ~error();

    error &
    operator=(error &&that) noexcept;

    uint8_t
    origin() const
    {
        return _origin;
    }

    uint8_t
    ordinal() const
    {
        return _ordinal;
    }

    const ustring &
    file() const
    {
        return _file;
    }

    unsigned
    line() const
    {
        return _line;
    }

    unsigned
    column() const
    {
        return _column;
    }

    size_t
    size() const
    {
        return _argc;
    }

    const uintptr_t
    operator[](size_t i) const;

    const uintptr_t *
    begin() const
    {
        return _argv;
    }

    const uintptr_t *
    end() const
    {
        return _argv + _argc;
    }

    template <typename Torigin, typename Traits = error_origin_traits<Torigin>>
    inline bool
    is(typename Traits::ordinal_type ord) const
    {
        return (static_cast<uint8_t>(Traits::origin_tag) == origin()) &&
               (static_cast<uint8_t>(ord) == ordinal());
    }
};

enum class error_origin : uint8_t
{
    stream = 1,
    lexer = 2,
    parser = 3,
};

template <typename T> using result = tl::expected<T, error>;

#define RETURN_IF_ERROR_VOID(expr)                                             \
    {                                                                          \
        auto __result = (expr);                                                \
        if (!__result)                                                         \
        {                                                                      \
            return tl::make_unexpected(std::move(__result.error()));           \
        }                                                                      \
    }

#define RETURN_IF_ERROR(out, expr)                                             \
    {                                                                          \
        auto __result = (expr);                                                \
        if (!__result)                                                         \
        {                                                                      \
            return tl::make_unexpected(std::move(__result.error()));           \
        }                                                                      \
        (out) = std::move(*__result);                                          \
    }

} // namespace zd
