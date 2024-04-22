#include <zd/pl_istream.hpp>
#include <zd/token.hpp>

namespace zd
{

class lexer
{
    pl_istream &_stream;
    token_type  _last_type;
    int         _ch;

  public:
    lexer(pl_istream &stream)
        : _stream{stream}, _last_type{token_type::line_break}, _ch{}
    {
    }

    token
    get_token();
};

} // namespace zd
