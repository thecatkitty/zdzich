#pragma once

#include <zd/error.hpp>
#include <zd/lex/pl_istream.hpp>
#include <zd/lex/token.hpp>

namespace zd
{

namespace lex
{

class lexer
{
    pl_istream &_stream;
    unsigned    _column;
    token_type  _last_type;
    int         _ch;
    unsigned    _spaces;
    ustring     _head;

  public:
    lexer(pl_istream &stream)
        : _stream{stream}, _last_type{token_type::line_break}, _column{0},
          _ch{}, _spaces{}, _head{}
    {
    }

    const ustring &
    get_path() const
    {
        return _stream.get_path();
    }

    unsigned
    get_line() const
    {
        return _stream.get_line();
    }

    unsigned
    get_column() const
    {
        return _column;
    }

    result<token>
    get_token();

    unsigned
    get_spaces() const
    {
        return _spaces;
    }

    enum class error_code : uint8_t
    {
        invalid_newline = 0,
        unexpected_character = 1,
    };

  private:
    result<void>
    read();

    result<void>
    scan_while(ustring &out, bool (*predicate)(int));

    result<token>
    process_string();

    tl::unexpected<error>
    make_error(error_code code)
    {
        return tl::make_unexpected(error{
            static_cast<uint8_t>(error_origin::lexer),
            static_cast<uint8_t>(code), get_path(), get_line(), get_column()});
    }

    tl::unexpected<error>
    make_error(error_code code, int character, token_type ttype)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::lexer),
                  static_cast<uint8_t>(code), get_path(), get_line(),
                  get_column(), character, to_cstr(ttype)});
    }
};

} // namespace lex

} // namespace zd
