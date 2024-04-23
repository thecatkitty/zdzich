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

static int
_read_stream(pl_istream &str)
{
    auto che = str.read();
    if (che)
    {
        return *che;
    }

    auto err = std::move(che.error());
    std::fprintf(stderr, "error %02X%02X", err.origin(), err.ordinal());
    for (auto arg : err)
    {
        std::fprintf(stderr, " %p", reinterpret_cast<void *>(arg));
    }
    std::fputs("\n", stderr);
    return EOF;
}

token
lexer::get_token()
{
    if (!_ch)
    {
        _ch = _read_stream(_stream);
    }

    if (!_stream || (EOF == _ch))
    {
        return {_last_type = token_type::eof};
    }

    if (!_isnotcrlf(_ch))
    {
        if (('\r' == _ch) && ('\n' != _stream.read()))
        {
            std::fprintf(stderr, "%s: unexpected lone carriage return\n",
                         __FUNCTION__);
            return {_last_type = token_type::unknown};
        }

        _ch = 0;
        return {_last_type = token_type::line_break};
    }

    while (_stream && isspace(_ch))
    {
        _ch = _read_stream(_stream);
    }

    RETURN_IF_CHTOKEN(',', {_last_type = token_type::comma});
    RETURN_IF_CHTOKEN('(', {_last_type = token_type::lbracket});
    RETURN_IF_CHTOKEN(')', {_last_type = token_type::rbracket});

    if ((token_type::line_break == _last_type) && ('*' == _ch))
    {
        // Comment
        ustring comment{};
        scan_while(comment, _isnotcrlf);
        return {_last_type = token_type::comment, comment};
    }

    if (isdigit(_ch))
    {
        // Integer literal
        int number{0};

        while (isdigit(_ch))
        {
            number *= 10;
            number += _ch - '0';
            _ch = _read_stream(_stream);
        }

        if (isalpha(_ch))
        {
            std::fprintf(stderr,
                         "%s: unexpected character %d in an integer literal\n",
                         __FUNCTION__, _ch);
            return {_last_type = token_type::unknown};
        }

        return {_last_type = token_type::literal_int, number};
    }

    ustring string{};
    if ((token_type::name != _last_type) && isalpha(_ch))
    {
        // Keyword, verb, or target
        scan_while(string, isalnum);
        _last_type = _match_keyword(string);
        if (token_type::comment == _last_type)
        {
            // Verbose comment
            scan_while(string, _isnotcrlf);
        }

        return {_last_type, string};
    }

    // String literal
    scan_while(string, _isnotcrlf);
    return {_last_type = token_type::literal_str, string};
}

bool
lexer::scan_while(ustring &out, bool (*predicate)(int))
{
    while (_stream && predicate(_ch))
    {
        out.append(_ch);

        _ch = _read_stream(_stream);
    }

    return !!_stream;
}
