#pragma once

#include <tl/expected.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <utility>

namespace zd
{

class error
{
    uint8_t    _origin;
    uint8_t    _ordinal;
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
    error(uint8_t origin, uint8_t ordinal)
        : _origin{origin}, _ordinal{ordinal}, _argc{0}, _argv{nullptr}
    {
    }

    error(const error &) = delete;

    error(error &&that) noexcept
        : _origin{that._origin}, _ordinal{that._ordinal},
          _argc{std::exchange(that._argc, 0)},
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

    template <typename... Args>
    error(uint8_t origin, uint8_t ordinal, Args... args)
        : _origin{origin}, _ordinal{ordinal}, _argc{sizeof...(args)},
          _argv{new(std::nothrow) uintptr_t[sizeof...(args)]}
    {
        to_argv(_argv, args...);
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
