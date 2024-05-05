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
    handle(const lex::token &head = lex::token{});

    enum class error_code : uint8_t
    {
        eof = 0,
        unexpected_token = 1,
        unexpected_eof = 2,
        unknown_directive = 3,
        out_of_range = 4,
    };

  private:
    result<unique_node>
    handle_assignment(lex::token_type ttype,
                      cpu_register    reg = cpu_register::invalid,
                      ustring         name = ustring{});

    result<unique_node>
    handle_call(const ustring &callee, bool enclosed = false);

    result<unique_node>
    handle_condition(lex::token_type   ttype,
                     const lex::token &head = lex::token{});

    result<unique_node>
    handle_declaration(bool is_const);

    result<unique_node>
    handle_directive(const ustring &directive);

    result<unique_node>
    handle_end();

    result<unique_node>
    handle_jump();

    result<unique_node>
    handle_label();

    result<unique_node>
    handle_number(int number);

    result<unique_node>
    handle_object(lex::token_type ttype, ustring name = ustring{});

    result<unique_node>
    handle_operation(lex::token_type ttype);

    result<unique_node>
    handle_procedure();

    result<unique_node>
    handle_register(cpu_register reg);

    result<unique_node>
    handle_string(const ustring &str, int spaces = 0);

    result<unique_node>
    handle_subscript();

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

    static tl::unexpected<error>
    make_error(error_code code, int number)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::parser),
                  static_cast<uint8_t>(code), number});
    }
};

} // namespace par

} // namespace zd
