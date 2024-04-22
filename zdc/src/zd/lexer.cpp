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
    RETURN_IF_STREQI(str, "Koment", token_type::comment);
    RETURN_IF_STREQI(str, "Komentarz", token_type::comment);

    return token_type::name;
}

token
lexer::get_token()
{
    auto ch = _last_ch ? std::exchange(_last_ch, 0) : _stream.read();
    if (!_stream || (EOF == ch))
    {
        return {_last_type = token_type::eof};
    }

    if (('\r' == ch) || ('\n' == ch))
    {
        if (('\r' == ch) && ('\n' != _stream.read()))
        {
            std::fprintf(stderr, "%s: unexpected lone carriage return\n",
                         __FUNCTION__);
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

    if ((token_type::line_break == _last_type) && ('*' == ch))
    {
        // Comment
        ustring comment{};

        while (_stream && ('\r' != ch) && ('\n' != ch))
        {
            comment.append(ch);

            ch = _stream.read();
        }

        _last_ch = ch;
        return {_last_type = token_type::comment, comment};
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

        auto keyword = _match_keyword(name);
        if (token_type::comment == keyword)
        {
            while (_stream && ('\r' != ch) && ('\n' != ch))
            {
                name.append(ch);

                ch = _stream.read();
            }
        }

        _last_ch = ch;
        return {_last_type = keyword, name};
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
            std::fprintf(stderr,
                         "%s: unexpected character %d in an integer literal\n",
                         __FUNCTION__, ch);
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
