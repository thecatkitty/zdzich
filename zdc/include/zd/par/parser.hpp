#pragma once

#include <zd/lex/lexer.hpp>
#include <zd/par/node.hpp>

namespace zd
{

namespace par
{

class parser
{
    lex::lexer &_lexer;

  public:
    parser(lex::lexer &lexer) : _lexer{lexer}
    {
    }

    result<unique_node>
    handle();

    enum class error_code : uint8_t
    {
        eof = 0,
        unexpected_token = 1,
    };

  private:
    result<unique_node>
    handle_assignment(lex::token_type ttype);

    result<unique_node>
    handle_call(const ustring &callee);

    result<unique_node>
    handle_condition(lex::token_type ttype);

    result<unique_node>
    handle_declaration();

    result<unique_node>
    handle_jump();

    result<unique_node>
    handle_label();

    result<unique_node>
    handle_number(int number);

    result<unique_node>
    handle_object(lex::token_type ttype);

    result<unique_node>
    handle_operation(lex::token_type ttype);

    result<unique_node>
    handle_register(const ustring &name);

    result<unique_node>
    handle_string(const ustring &str);

    result<unique_node>
    handle_value();

    static tl::unexpected<error>
    make_error(error_code code)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::parser),
                  static_cast<uint8_t>(code)});
    }

    static tl::unexpected<error>
    make_error(error_code code, lex::token_type ttype)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::parser),
                  static_cast<uint8_t>(code), to_cstr(ttype)});
    }
};

} // namespace par

} // namespace zd
