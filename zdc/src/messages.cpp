#include <algorithm>
#include <cstdint>
#include <cstdio>

#include <zd/error.hpp>
#include <zd/lex/lexer.hpp>
#include <zd/lex/pl_istream.hpp>
#include <zd/par/parser.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

using namespace zd;

static constexpr uint16_t
make_id(uint8_t origin, uint8_t ordinal)
{
    return (static_cast<uint16_t>(origin) << 8) |
           static_cast<uint16_t>(ordinal);
}

#ifndef _WIN32
extern "C" const struct
{
    unsigned    id;
    const char *msg;
} MESSAGES_0409[];

static const char *
retrieve_fmt(uint16_t id)
{
    for (auto it = MESSAGES_0409; it->id; it++)
    {
        if (it->id == id)
        {
            return it->msg;
        }
    }

    return nullptr;
}

#define ARGV1(argv) (argv)[0]
#define ARGV2(argv) ARGV1(argv), (argv)[1]

#define CASE_PRINT_ARGS(num, fmt, argv)                                        \
    case num:                                                                  \
        std::fprintf(stderr, (fmt), ARGV##num(argv));                          \
        break;
#endif

void
print_error(const error &err)
{
    auto path = err.file().empty() ? "input" : err.file().data();
    std::fprintf(stderr, "%s:%u:%u: error: ", path, err.line(), err.column());

#ifdef _WIN32
    auto args =
        reinterpret_cast<DWORD_PTR *>(const_cast<uintptr_t *>(err.begin()));
    LPWSTR buffer{nullptr};
    DWORD  length = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY |
            FORMAT_MESSAGE_FROM_HMODULE,
        nullptr, make_id(err.origin(), err.ordinal()),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&buffer), 0,
        reinterpret_cast<va_list *>(args));
    ::WriteConsoleW(::GetStdHandle(STD_ERROR_HANDLE), buffer, length, NULL,
                    NULL);
    ::LocalFree(buffer);

#else
    if (auto fmt = retrieve_fmt(make_id(err.origin(), err.ordinal())))
    {
        switch (err.size())
        {
            CASE_PRINT_ARGS(1, fmt, err);
            CASE_PRINT_ARGS(2, fmt, err);
        default:
            std::fputs(fmt, stderr);
        }
    }
#endif

    std::fputs("\n", stderr);
}
