#include <cstdio>

#include <zd/lex/token.hpp>
#include <zd/text/encoding.hpp>

#define ENUM_NAME_MAPPING(name, description) #name

static const char *ENUM_NAMES[]{
    ENUM_NAME_MAPPING(unknown, "unknown"),
    ENUM_NAME_MAPPING(eof, "end of file"),
    ENUM_NAME_MAPPING(line_break, "line break"),
    ENUM_NAME_MAPPING(literal_int, "integer literal"),
    ENUM_NAME_MAPPING(literal_str, "string literal"),
    ENUM_NAME_MAPPING(name, "symbolic name"),
    ENUM_NAME_MAPPING(comma, "comma"),
    ENUM_NAME_MAPPING(colon, "colon"),
    ENUM_NAME_MAPPING(lbracket, "opening bracket"),
    ENUM_NAME_MAPPING(rbracket, "closing bracket"),
    ENUM_NAME_MAPPING(byval, "access by value"),
    ENUM_NAME_MAPPING(byref, "access by reference"),
    ENUM_NAME_MAPPING(assign, "assignment"),
    ENUM_NAME_MAPPING(plus, "plus sign"),
    ENUM_NAME_MAPPING(minus, "minus sign"),
    ENUM_NAME_MAPPING(ampersand, "ampersand"),
    ENUM_NAME_MAPPING(cpref_lt, "conditional prefix less than"),
    ENUM_NAME_MAPPING(cpref_gt, "conditional prefix greater than"),
    ENUM_NAME_MAPPING(cpref_ne, "conditional prefix not equal"),
    ENUM_NAME_MAPPING(cpref_le, "conditional prefix less or equal"),
    ENUM_NAME_MAPPING(cpref_ge, "conditional prefix greater or equal"),
    ENUM_NAME_MAPPING(subscript, "subscript"),
    ENUM_NAME_MAPPING(comment, "comment"),
    ENUM_NAME_MAPPING(directive, "directive"),
    ENUM_NAME_MAPPING(compare, "compare keyword"),
    ENUM_NAME_MAPPING(constant, "constant keyword"),
    ENUM_NAME_MAPPING(decrement, "decrement keyword"),
    ENUM_NAME_MAPPING(end, "end keyword"),
    ENUM_NAME_MAPPING(increment, "increment keyword"),
    ENUM_NAME_MAPPING(jump, "jump keyword"),
    ENUM_NAME_MAPPING(procedure, "procedure keyword"),
    ENUM_NAME_MAPPING(variable, "variable keyword"),
};

zd::ustring
zd::to_string(lex::token_type tok)
{
    if (auto cstr = to_cstr(tok))
    {
        return cstr;
    }

    char buff[12]{};
    std::snprintf(buff, 12, "%d", static_cast<int>(tok));
    return buff;
}

const char *
zd::to_cstr(lex::token_type tok)
{
    auto i = static_cast<int>(tok);
    if (0 <= i)
    {
        return ENUM_NAMES[i];
    }

    return nullptr;
}

zd::lex::token::~token()
{
    if (_value_type::text == _vtype)
    {
        _text.~ustring();
    }
}

zd::lex::token::token(const token &that)
    : _vtype{that._vtype}, _ttype{that._ttype}, _number{that._number}
{
    if (_value_type::text == _vtype)
    {
        new (&_text) ustring(that._text);
    }
}

zd::lex::token::token(token &&that) noexcept
    : _vtype{that._vtype}, _ttype{that._ttype}, _number{that._number}
{
    if (_value_type::text == _vtype)
    {
        new (&_text) ustring(std::move(that._text));
    }
}

zd::lex::token &
zd::lex::token::operator=(const token &that)
{
    if (this == &that)
    {
        return *this;
    }

    this->~token();
    _vtype = that._vtype;
    _ttype = that._ttype;
    _number = that._number;

    if (_value_type::text == _vtype)
    {
        new (&_text) ustring(that._text);
    }

    return *this;
}

zd::lex::token &
zd::lex::token::operator=(token &&that) noexcept
{
    if (this == &that)
    {
        return *this;
    }

    this->~token();
    _vtype = that._vtype;
    _ttype = that._ttype;
    _number = that._number;

    if (_value_type::text == _vtype)
    {
        new (&_text) ustring(std::move(that._text));
    }

    return *this;
}
