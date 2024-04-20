#include <zd/characters.hpp>
#include <zd/lexer.hpp>
#include <zd/pl_string.hpp>

using namespace zd;

#define RETURN_IF_STREQI(left, right, value)                                   \
    if (pl_streqi((left), (right)))                                            \
    {                                                                          \
        return value;                                                          \
    }

#define RETURN_IF_STREQAI(left, right, value)                                  \
    if (pl_streqai((left), (right)))                                           \
    {                                                                          \
        return value;                                                          \
    }

static token_type
_match_keyword(const ustring &str)
{
    RETURN_IF_STREQI(str, "Koniec", token_type::end);

    return token_type::name;
}

token
lexer::get_token()
{
    auto ch = _last_ch ? std::exchange(_last_ch, 0) : _stream.read();
    if (!_stream)
    {
        return {_last_type = token_type::eof};
    }

    if (('\r' == ch) || ('\n' == ch))
    {
        if (('\r' == ch) && ('\n' != _stream.read()))
        {
            std::cerr << __FUNCTION__ << ": unexpected lone carriage return"
                      << std::endl;
            return {_last_type = token_type::unknown};
        }

        return {_last_type = token_type::line_break};
    }

    while (_stream && isspace(ch))
    {
        ch = _stream.read();
    }

    if (',' == ch)
    {
        return {_last_type = token_type::comma};
    }

    if ((token_type::line_break == _last_type) && isalpha(ch))
    {
        // Keyword, verb, or target
        ustring name{};

        while (_stream && isalnum(ch))
        {
            name.append(ch);

            ch = _stream.read();
        }

        _last_ch = ch;
        return {_last_type = _match_keyword(name), name};
    }

    // Integer literal
    if (isdigit(ch))
    {
        int number{0};

        while (isdigit(ch))
        {
            number *= 10;
            number += ch - '0';
            ch = _stream.read();
        }

        if (isalpha(ch))
        {
            std::cerr << __FUNCTION__ << ": unexpected character " << ch
                      << " in an integer literal" << std::endl;
            return {_last_type = token_type::unknown};
        }

        _last_ch = ch;
        return {_last_type = token_type::literal_int, number};
    }

    // String literal
    ustring str{};

    while (_stream && ('\r' != ch) && ('\n' != ch))
    {
        str.append(ch);

        ch = _stream.read();
    }

    _last_ch = ch;
    return {_last_type = token_type::literal_str, str};
}
