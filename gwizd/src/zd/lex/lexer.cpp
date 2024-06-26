#include <zd/lex/lexer.hpp>
#include <zd/text/characters.hpp>
#include <zd/text/pl_string.hpp>

using namespace zd;

#define RETURN_IF_STREQI(left, right, value)                                   \
    if (text::pl_streqi((left), (right)))                                      \
    {                                                                          \
        return value;                                                          \
    }

#define RETURN_IF_STREQAI(left, right, value)                                  \
    if (text::pl_streqai((left), (right)))                                     \
    {                                                                          \
        return value;                                                          \
    }

#define RETURN_IF_CHTOKEN(ch, value)                                           \
    if ((ch) == _ch)                                                           \
    {                                                                          \
        _ch = 0;                                                               \
        return value;                                                          \
    };

static lex::token_type
_match_keyword(const ustring &str)
{
    RETURN_IF_STREQI(str, "Koment", lex::token_type::comment);
    RETURN_IF_STREQI(str, "Komentarz", lex::token_type::comment);
    RETURN_IF_STREQAI(str, "Porównaj", lex::token_type::compare)
    RETURN_IF_STREQAI(str, "Stała", lex::token_type::constant)
    RETURN_IF_STREQI(str, "Zmniejsz", lex::token_type::decrement);
    RETURN_IF_STREQI(str, "Koniec", lex::token_type::end);
    RETURN_IF_STREQAI(str, "Zwiększ", lex::token_type::increment);
    RETURN_IF_STREQI(str, "Skok", lex::token_type::jump);
    RETURN_IF_STREQI(str, "Procedura", lex::token_type::procedure);
    RETURN_IF_STREQI(str, "Zmienna", lex::token_type::variable);

    return lex::token_type::name;
}

static bool
_is_register(const ustring &str)
{
    if (2 != str.size())
    {
        return false;
    }

    int first = str.data()[0], second = str.data()[1];
    if (!text::isalpha(first) || !text::isalpha(second))
    {
        return false;
    }

    first = tolower(first);
    second = tolower(second);

    if (('a' == first) || ('b' == first) || ('c' == first))
    {
        return ('l' == second) || ('h' == second) || ('x' == second);
    }

    if ('d' == first)
    {
        return ('l' == second) || ('h' == second) || ('x' == second) ||
               ('i' == second);
    }

    if ('s' == first)
    {
        return ('i' == second);
    }

    return false;
}

static bool
_isnotcrlf(int ch)
{
    return ('\r' != ch) && ('\n' != ch);
}

static unsigned
_chtou(int ch)
{
    if (text::isdigit(ch))
    {
        return ch - '0';
    }

    if ('a' <= ch)
    {
        return 10 + ch - 'a';
    }

    return 10 + ch - 'A';
}

static bool
_allows_name_after(lex::token_type type)
{
    switch (type)
    {
    case lex::token_type::name:
    case lex::token_type::comma:
    case lex::token_type::assign:
    case lex::token_type::rbracket:
        return false;
    }

    return true;
}

result<lex::token>
lex::lexer::get_token()
{
    if (!_head.empty())
    {
        return std::move(process_string());
    }

    _spaces = 0;

    if (!_ch)
    {
        RETURN_IF_ERROR_VOID(read());
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

    while (_stream && text::isspace(_ch))
    {
        _spaces++;
        RETURN_IF_ERROR_VOID(read());
    }

    // Single-character tokens
    RETURN_IF_CHTOKEN(',', {_last_type = token_type::comma});
    RETURN_IF_CHTOKEN(':', {_last_type = token_type::colon});
    RETURN_IF_CHTOKEN('(', {_last_type = token_type::lbracket});
    RETURN_IF_CHTOKEN(')', {_last_type = token_type::rbracket});
    RETURN_IF_CHTOKEN('%', {_last_type = token_type::byval});
    RETURN_IF_CHTOKEN('=', {_last_type = token_type::assign});

    if ((token_type::name == _last_type) && (('+' == _ch) || ('-' == _ch)))
    {
        // Plus sign, minus sign, or a string literal made of them
        _head.append(_ch);
        RETURN_IF_ERROR_VOID(read());

        if (('+' == _ch) || ('-' == _ch))
        {
            // A string literal (keep head)
            return std::move(process_string());
        }

        // A single character!
        token_type type =
            ('+' == *_head.data()) ? token_type::plus : token_type::minus;
        _head.clear();
        return {_last_type = type};
    }

    // Conditional prefixes
    if (token_type::line_break == _last_type)
    {
        RETURN_IF_CHTOKEN('>', {_last_type = token_type::cpref_gt});

        if ('<' == _ch)
        {
            RETURN_IF_ERROR_VOID(read());
            RETURN_IF_CHTOKEN('>', {_last_type = token_type::cpref_ne});
            return {_last_type = token_type::cpref_lt};
        }
    }

    if ('&' == _ch)
    {
        RETURN_IF_ERROR_VOID(read());
        RETURN_IF_CHTOKEN('<', {_last_type = token_type::cpref_le});
        RETURN_IF_CHTOKEN('>', {_last_type = token_type::cpref_ge});
        return {_last_type = token_type::ampersand};
    }

    if ((token_type::line_break == _last_type) && ('*' == _ch))
    {
        // Comment
        ustring comment{};
        RETURN_IF_ERROR_VOID(scan_while(comment, _isnotcrlf));
        return token{_last_type = token_type::comment, std::move(comment)};
    }

    int base{10};
    if ('$' == _ch)
    {
        // Either reference access or a hexadecimal literal
        RETURN_IF_ERROR_VOID(read());
        if (!isxdigit(_ch))
        {
            // Reference access
            return token{_last_type = token_type::byref};
        }

        base = 16;
        // Not sure yet, pass through
    }

    auto isbdigit = (16 == base) ? text::isxdigit : text::isdigit;
    if (isbdigit(_ch))
    {
        // Integer literal
        int number{0};

        while (isbdigit(_ch))
        {
            number *= base;
            number += _chtou(_ch);
            _head.append(_ch); // In case it's not a literal.
            RETURN_IF_ERROR_VOID(read());
        }

        if (text::is_name_continuation(_ch))
        {
            if (16 == base)
            {
                // It's all name, always have been (keep head)
                return token{_last_type = token_type::byref};
            }

            if (token_type::colon == _last_type)
            {
                // :3Opcja is a valid label in ZDZICH4
                return std::move(process_string());
            }

            return make_error(error_code::unexpected_character, _ch,
                              token_type::literal_int);
        }

        // It was an integer literal indeed (clear head)
        _head.clear();
        return token{_last_type = token_type::literal_int, number};
    }

    return std::move(process_string());
}

result<void>
lex::lexer::read()
{
    RETURN_IF_ERROR(_ch, _stream.read());
    _column = _isnotcrlf(_ch) ? (_column + 1) : 0;
    return {};
}

result<lex::token>
lex::lexer::process_string()
{
    ustring string{};
    if (!_head.empty())
    {
        string = _head;
        _head.clear();
    }

    if ('#' == _ch)
    {
        // Directive or subscript
        RETURN_IF_ERROR_VOID(read());
        if (!text::isalpha(_ch) && (',' != _ch))
        {
            // Subscript
            return token(token_type::subscript);
        }

        // Directive
        RETURN_IF_ERROR_VOID(scan_while(string, _isnotcrlf));
        return token{_last_type = token_type::directive, std::move(string)};
    }

    if (_allows_name_after(_last_type) &&
        ((token_type::colon == _last_type) ? text::is_name_continuation(_ch)
                                           : text::is_name_start(_ch)))
    {
        // Keyword, verb, or target
        RETURN_IF_ERROR_VOID(scan_while(string, text::is_name_continuation));
        _last_type = _match_keyword(string);
        if (token_type::comment == _last_type)
        {
            // Verbose comment
            RETURN_IF_ERROR_VOID(scan_while(string, _isnotcrlf));
        }

        return token{_last_type, std::move(string)};
    }

    // String literal or register name
    while (_stream && _isnotcrlf(_ch))
    {
        if (!text::isalnum(_ch) && _is_register(string))
        {
            // Register name (and more)
            return token{_last_type = token_type::name, std::move(string)};
        }

        string.append(_ch);

        RETURN_IF_ERROR_VOID(read());
    }

    // Ignore single trailing spaces 
    if (string.empty() && (2 > _spaces))
    {
        return token{_last_type = token_type::line_break};
    }

    if (_is_register(string))
    {
        // Register name (and nothing else)
        return token{_last_type = token_type::name, std::move(string)};
    }

    // Anything other - string literal
    return token{_last_type = token_type::literal_str, std::move(string)};
}

result<void>
lex::lexer::scan_while(ustring &out, bool (*predicate)(int))
{
    while (_stream && predicate(_ch))
    {
        out.append(_ch);

        RETURN_IF_ERROR_VOID(read());
    }

    return {};
}
