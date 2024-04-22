#include <cstdio>
#include <new>

#include <zd/encoding.hpp>
#include <zd/token.hpp>

#define ENUM_NAME_MAPPING(name, description) #name

static const char *ENUM_NAMES[]{
    ENUM_NAME_MAPPING(unknown, "unknown"),
    ENUM_NAME_MAPPING(eof, "end of file"),
    ENUM_NAME_MAPPING(line_break, "line break"),
    ENUM_NAME_MAPPING(literal_int, "integer literal"),
    ENUM_NAME_MAPPING(literal_str, "string literal"),
    ENUM_NAME_MAPPING(name, "symbolic name"),
    ENUM_NAME_MAPPING(comma, "comma"),
    ENUM_NAME_MAPPING(lbracket, "opening bracket"),
    ENUM_NAME_MAPPING(rbracket, "closing bracket"),
    ENUM_NAME_MAPPING(comment, "comment"),
    ENUM_NAME_MAPPING(end, "end keyword"),
    ENUM_NAME_MAPPING(procedure, "procedure keyword"),
};

zd::ustring
zd::to_string(token_type tok)
{
    auto i = static_cast<int>(tok);
    if (0 <= i)
    {
        return ENUM_NAMES[i];
    }

    char buff[12]{};
    std::snprintf(buff, 12, "%d", i);
    return buff;
}

zd::token::~token()
{
    if (_value_type::text == _vtype)
    {
        _text.~ustring();
    }
}

zd::token::token(const token &that)
    : _vtype{that._vtype}, _ttype{that._ttype}, _number{that._number}
{
    if (_value_type::text == _vtype)
    {
        new (&_text) ustring(that._text);
    }
}

zd::token::token(token &&that) noexcept
    : _vtype{that._vtype}, _ttype{that._ttype}, _number{that._number}
{
    if (_value_type::text == _vtype)
    {
        new (&_text) ustring(std::move(that._text));
    }
}

zd::token &
zd::token::operator=(const token &that)
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

zd::token &
zd::token::operator=(token &&that) noexcept
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
