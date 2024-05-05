#include <algorithm>
#include <cstdint>
#include <cstdio>

#include <zd/error.hpp>
#include <zd/lex/lexer.hpp>
#include <zd/lex/pl_istream.hpp>
#include <zd/par/parser.hpp>

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
     "unexpected character %d in a token '%s'"},

    // Parser
    {make_id(error_origin::parser, par::parser::error_code::eof),
     "end of file"},
    {make_id(error_origin::parser, par::parser::error_code::unexpected_token),
     "unexpected token '%s'"},
    {make_id(error_origin::parser, par::parser::error_code::unexpected_eof),
     "unexpected end of file while processing '%s'"},
    {make_id(error_origin::parser, par::parser::error_code::unknown_directive),
     "unknown directive"},
    {make_id(error_origin::parser, par::parser::error_code::out_of_range),
     "number %d is out of range"},
    {make_id(error_origin::parser, par::parser::error_code::name_expected),
     "name expected after '%s', got '%s'"},
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

void
print_error(const error &err)
{
    if (auto fmt = retrieve_fmt(err.origin(), err.ordinal()))
    {
        switch (err.size())
        {
            CASE_PRINT_ARGS(1, fmt, err);
            CASE_PRINT_ARGS(2, fmt, err);
        default:
            std::fputs(fmt, stderr);
        }

        std::fputs("\n", stderr);
    }
}
