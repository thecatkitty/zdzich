#include <cstdio>

#include <zd/par/parser.hpp>

using namespace zd;

result<par::unique_node>
par::parser::handle()
{
    while (true)
    {
        lex::token token{};
        RETURN_IF_ERROR(token, _lexer.get_token());

        switch (token.get_type())
        {
        case lex::token_type::line_break:
        case lex::token_type::comment:
            continue;

        case lex::token_type::eof:
            return make_error(error_code::eof);

        case lex::token_type::end:
            return std::make_unique<end_node>();

        case lex::token_type::name:
            return handle_call(token.get_text());

        case lex::token_type::variable:
            return handle_declaration();

        default:
            return make_error(error_code::unexpected_token, token.get_type());
        }
    }
}

result<par::unique_node>
par::parser::handle_call(const ustring &callee)
{
    node_list  arguments{};
    bool       more{true};
    lex::token token{};

    while (more)
    {
        RETURN_IF_ERROR(token, _lexer.get_token());

        switch (token.get_type())
        {
        case lex::token_type::literal_str: {
            unique_node str{};
            RETURN_IF_ERROR(str, handle_string(token.get_text()));
            arguments.push_back(std::move(str));
            break;
        }

        case lex::token_type::literal_int: {
            unique_node num{};
            RETURN_IF_ERROR(num, handle_number(token.get_number()));
            arguments.push_back(std::move(num));
            break;
        }

        case lex::token_type::line_break:
        case lex::token_type::eof: {
            more = false;
            break;
        }

        default: {
            return make_error(error_code::unexpected_token, token.get_type());
        }
        }
    }

    return std::make_unique<call_node>(callee, std::move(arguments));
}

result<par::unique_node>
par::parser::handle_declaration()
{
    lex::token  token{};
    object_type type{};

    RETURN_IF_ERROR(token, _lexer.get_token());
    switch (token.get_type())
    {
    case lex::token_type::byref:
        type = object_type::text;
        break;

    default:
        return make_error(error_code::unexpected_token, token.get_type());
    }

    unique_node object{};
    RETURN_IF_ERROR(object, handle_object(type));
    return std::make_unique<declaration_node>(std::move(object));
}

result<par::unique_node>
par::parser::handle_number(int number)
{
    return std::make_unique<number_node>(number);
}

result<par::unique_node>
par::parser::handle_object(object_type type)
{
    lex::token token{};
    ustring    name{};

    RETURN_IF_ERROR(token, _lexer.get_token());
    switch (token.get_type())
    {
    case lex::token_type::name:
        name = std::move(token.get_text());
        break;

    default:
        return make_error(error_code::unexpected_token, token.get_type());
    }

    return std::make_unique<object_node>(name, type);
}

result<par::unique_node>
par::parser::handle_string(const ustring &str)
{
    return std::make_unique<string_node>(str);
}
