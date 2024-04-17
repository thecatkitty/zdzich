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

static token
_match_keyword(const std::string &str)
{
    RETURN_IF_STREQI(str, "Koniec", token::end);

    return token::name;
}

token
lexer::get_token()
{
    auto ch = _saved_ch ? std::exchange(_saved_ch, 0) : _stream.read();
    if (!_stream)
    {
        return _last = token::eof;
    }

    if (('\r' == ch) || ('\n' == ch))
    {
        if (('\r' == ch) && ('\n' != _stream.read()))
        {
            std::cerr << __FUNCTION__ << ": unexpected lone carriage return"
                      << std::endl;
            return _last = token::unknown;
        }

        return _last = token::line_break;
    }

    while (_stream && isspace(ch))
    {
        ch = _stream.read();
    }

    _integer = 0;
    _string.clear();

    if (',' == ch)
    {
        return _last = token::comma;
    }

    if ((token::line_break == _last) && isalpha(ch))
    {
        // Keyword, verb, or target
        while (_stream && isalnum(ch))
        {
            char buff[5]{};
            if (!encoding::utf_8->encode(buff, ch))
            {
                return _last = token::eof;
            }
            _string.append(buff);

            ch = _stream.read();
        }

        _saved_ch = ch;
        return _last = _match_keyword(_string);
    }

    // Integer literal
    if (isdigit(ch))
    {
        while (isdigit(ch))
        {
            _integer *= 10;
            _integer += ch - '0';
            ch = _stream.read();
        }

        if (isalpha(ch))
        {
            std::cerr << __FUNCTION__ << ": unexpected character " << ch
                      << " in an integer literal" << std::endl;
            return _last = token::unknown;
        }

        _saved_ch = ch;
        return _last = token::literal_int;
    }

    // String literal
    while (_stream && ('\r' != ch) && ('\n' != ch))
    {
        char buff[5]{};
        if (!encoding::utf_8->encode(buff, ch))
        {
            return _last = token::eof;
        }
        _string.append(buff);

        ch = _stream.read();
    }

    _saved_ch = ch;
    return _last = token::literal_str;
}
