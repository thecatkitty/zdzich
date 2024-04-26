#include <zd/ustring.hpp>

namespace zd
{

enum class token_type
{
    unknown,
    eof,
    line_break,
    literal_int,
    literal_str,
    name,
    comma,     // ,
    colon,     // :
    lbracket,  // (
    rbracket,  // )
    byval,     // %
    byref,     // $
    assign,    // =
    cpref_eq,  // &
    cpref_lt,  // <
    cpref_gt,  // >
    cpref_ne,  // <>
    cpref_le,  // &<
    cpref_ge,  // &>
    comment,   // *, Koment, Komentarz
    compare,   // Porównaj
    constant,  // Stała
    end,       // Koniec
    jump,      // Skok
    procedure, // Procedura
    variable,  // Zmienna
};

ustring
to_string(token_type tok);

const char *
to_cstr(token_type tok);

class token
{
    enum class _value_type
    {
        none,
        number,
        text
    };

    token_type _ttype;

    _value_type _vtype;
    union {
        int     _number;
        ustring _text;
    };

  public:
    token() : _vtype{_value_type::none}, _ttype{token_type::unknown}, _number{}
    {
    }

    token(token_type type) : _vtype{_value_type::none}, _ttype{type}, _number{}
    {
    }

    token(token_type type, int value)
        : _vtype{_value_type::number}, _ttype{type}, _number{value}
    {
    }

    token(token_type type, ustring &&value)
        : _vtype{_value_type::text}, _ttype{type}, _text{std::move(value)}
    {
    }

    ~token();

    token(const token &that);

    token(token &&that) noexcept;

    token &
    operator=(const token &that);

    token &
    operator=(token &&that) noexcept;

    token_type
    get_type() const
    {
        return _ttype;
    }

    int
    get_number() const
    {
        return (_value_type::number == _vtype) ? _number : 0;
    }

    ustring
    get_text() const
    {
        return (_value_type::text == _vtype) ? _text : std::move(ustring{});
    }
};

} // namespace zd
