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

template <typename Torig, typename Tord>
static constexpr uint16_t
make_id(Torig origin, Tord ordinal)
{
    return (static_cast<uint16_t>(origin) << 8) |
           static_cast<uint16_t>(ordinal);
}

#ifndef _WIN32
const struct _msg
{
    uint16_t    id;
    const char *fmt;
} MESSAGES[]{
    // Stream
    {make_id(error_origin::stream,
             lex::pl_istream::error_code::unexpected_byte),
     "unexpected byte %u in the encoding '%s'"},
    {make_id(error_origin::stream,
             lex::pl_istream::error_code::invalid_sequence),
     "invalid multi-byte sequence"},
    {make_id(error_origin::stream, lex::pl_istream::error_code::read_error),
     "read error in a multi-byte sequence"},

    // Lexer
    {make_id(error_origin::lexer, lex::lexer::error_code::invalid_newline),
     "invalid line break"},
    {make_id(error_origin::lexer, lex::lexer::error_code::unexpected_character),
     "unexpected character %d in '%s'"},

    // Parser
    {make_id(error_origin::parser, par::parser::error_code::eof),
     "end of file"},
    {make_id(error_origin::parser, par::parser::error_code::unexpected_token),
     "unexpected '%s' in '%s'"},
    {make_id(error_origin::parser, par::parser::error_code::unexpected_eof),
     "unexpected end of file while processing '%s'"},
    {make_id(error_origin::parser, par::parser::error_code::unknown_directive),
     "unknown directive"},
    {make_id(error_origin::parser, par::parser::error_code::out_of_range),
     "number %d is out of range"},
    {make_id(error_origin::parser, par::parser::error_code::name_expected),
     "name expected after '%s', got '%s'"},
    {make_id(error_origin::parser, par::parser::error_code::path_expected),
     "include path expected"},
    {make_id(error_origin::parser, par::parser::error_code::not_a_command),
     "unexpected '%s' at the beginning of a command"},
    {make_id(error_origin::parser, par::parser::error_code::cannot_emit),
     "unexpected character %u in the emit directive"},
};

static const char *
retrieve_fmt(uint8_t origin, uint8_t ordinal)
{
    auto id = make_id(origin, ordinal);
    auto it = std::lower_bound(std::begin(MESSAGES), std::end(MESSAGES), id,
                               [](const _msg &msg, uint16_t i) {
                                   return msg.id < i;
                               });
    return it == std::end(MESSAGES) ? nullptr
           : it->id != id           ? nullptr
                                    : it->fmt;
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
    if (auto fmt = retrieve_fmt(err.origin(), err.ordinal()))
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
