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

#define RETURN_IF_CHTOKEN(ch, value)                                           \
    if ((ch) == _ch)                                                           \
    {                                                                          \
        _ch = 0;                                                               \
        return value;                                                          \
    };

static token_type
_match_keyword(const ustring &str)
{
    RETURN_IF_STREQI(str, "Koment", token_type::comment);
    RETURN_IF_STREQI(str, "Komentarz", token_type::comment);
    RETURN_IF_STREQI(str, "Koniec", token_type::end);
    RETURN_IF_STREQI(str, "Procedura", token_type::procedure);

    return token_type::name;
}

static bool
_isnotcrlf(int ch)
{
    return ('\r' != ch) && ('\n' != ch);
}

result<token>
lexer::get_token()
{
    if (!_ch)
    {
        RETURN_IF_ERROR(_ch, _stream.read());
    }

    if (!_stream || (EOF == _ch))
    {
        return {_last_type = token_type::eof};
    }

    if (!_isnotcrlf(_ch))
    {
        if (('\r' == _ch) && ('\n' != _stream.read()))
        {
            return make_error(error_code::invalid_newline);
        }

        _ch = 0;
        return {_last_type = token_type::line_break};
    }

    while (_stream && isspace(_ch))
    {
        RETURN_IF_ERROR(_ch, _stream.read());
    }

    RETURN_IF_CHTOKEN(',', {_last_type = token_type::comma});
    RETURN_IF_CHTOKEN('(', {_last_type = token_type::lbracket});
    RETURN_IF_CHTOKEN(')', {_last_type = token_type::rbracket});

    if ((token_type::line_break == _last_type) && ('*' == _ch))
    {
        // Comment
        ustring comment{};
        RETURN_IF_ERROR_VOID(scan_while(comment, _isnotcrlf));
        return token{_last_type = token_type::comment, comment};
    }

    if (isdigit(_ch))
    {
        // Integer literal
        int number{0};

        while (isdigit(_ch))
        {
            number *= 10;
            number += _ch - '0';
            RETURN_IF_ERROR(_ch, _stream.read());
        }

        if (isalpha(_ch))
        {
            return make_error(error_code::unexpected_character, _ch,
                              token_type::literal_int);
        }

        return token{_last_type = token_type::literal_int, number};
    }

    ustring string{};
    if ((token_type::name != _last_type) && isalpha(_ch))
    {
        // Keyword, verb, or target
        RETURN_IF_ERROR_VOID(scan_while(string, isalnum));
        _last_type = _match_keyword(string);
        if (token_type::comment == _last_type)
        {
            // Verbose comment
            RETURN_IF_ERROR_VOID(scan_while(string, _isnotcrlf));
        }

        return token{_last_type, string};
    }

    // String literal
    RETURN_IF_ERROR_VOID(scan_while(string, _isnotcrlf));
    return token{_last_type = token_type::literal_str, string};
}

result<void>
lexer::scan_while(ustring &out, bool (*predicate)(int))
{
    while (_stream && predicate(_ch))
    {
        out.append(_ch);

        RETURN_IF_ERROR(_ch, _stream.read());
    }

    return {};
}
