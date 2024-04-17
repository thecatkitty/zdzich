#include <cctype>
#include <iostream>

#include <zd/pl_istream.hpp>
#include <zd/token.hpp>

namespace zd
{

class lexer
{
    pl_istream &_stream;
    token       _last;
    int         _saved_ch;
    unsigned    _integer;
    std::string _string;

  public:
    lexer(pl_istream &stream)
        : _stream{stream}, _last{token::line_break}, _saved_ch{}, _integer{},
          _string{}
    {
    }

    token
    get_token();

    unsigned
    get_integer() const
    {
        return _integer;
    }

    std::string
    get_string() const
    {
        return _string;
    }
};

} // namespace zd
