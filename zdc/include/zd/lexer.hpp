#include <cctype>
#include <iostream>

#include <zd/pl_istream.hpp>
#include <zd/token.hpp>

namespace zd
{

class lexer
{
    pl_istream &_stream;
    token_type  _last_type;
    int         _last_ch;

  public:
    lexer(pl_istream &stream)
        : _stream{stream}, _last_type{token_type::line_break}, _last_ch{}
    {
    }

    token
    get_token();
};

} // namespace zd
