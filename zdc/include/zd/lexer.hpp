#include <zd/error.hpp>
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

    result<token>
    get_token();

    enum class error_code : uint8_t
    {
        invalid_newline = 0,
        unexpected_character = 1,
    };

  private:
    result<void>
    scan_while(ustring &out, bool (*predicate)(int));

    static tl::unexpected<error>
    make_error(error_code code)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::lexer),
                  static_cast<uint8_t>(code)});
    }

    static tl::unexpected<error>
    make_error(error_code code, int character, token_type ttype)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::lexer),
                  static_cast<uint8_t>(code), character, to_cstr(ttype)});
    }
};

} // namespace zd
