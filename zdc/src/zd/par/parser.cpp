#include <cctype>
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

        case lex::token_type::ampersand:
        case lex::token_type::cpref_lt:
        case lex::token_type::cpref_gt:
        case lex::token_type::cpref_ne:
        case lex::token_type::cpref_le:
        case lex::token_type::cpref_ge:
            return handle_condition(token.get_type());

        case lex::token_type::end:
            return std::make_unique<end_node>();

        case lex::token_type::colon:
            return handle_label();

        case lex::token_type::compare:
            return handle_comparison();

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

        case lex::token_type::byref: {
            unique_node obj{};
            RETURN_IF_ERROR(obj, handle_object(token.get_type()));
            arguments.push_back(std::move(obj));
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
par::parser::handle_comparison()
{
    lex::token token{};

    unique_node left{};
    RETURN_IF_ERROR(token, _lexer.get_token());
    switch (token.get_type())
    {
    case lex::token_type::name: {
        RETURN_IF_ERROR(left, handle_register(token.get_text()));
        break;
    }

    case lex::token_type::literal_int: {
        RETURN_IF_ERROR(left, handle_number(token.get_number()));
        break;
    }

    case lex::token_type::byref: {
        RETURN_IF_ERROR(left, handle_object(token.get_type()));
        break;
    }

    default: {
        return make_error(error_code::unexpected_token, token.get_type());
    }
    }

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::comma != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    unique_node right{};
    RETURN_IF_ERROR(token, _lexer.get_token());
    switch (token.get_type())
    {
    case lex::token_type::name: {
        RETURN_IF_ERROR(right, handle_register(token.get_text()));
        break;
    }

    case lex::token_type::literal_int: {
        RETURN_IF_ERROR(right, handle_number(token.get_number()));
        break;
    }

    case lex::token_type::byref: {
        RETURN_IF_ERROR(right, handle_object(token.get_type()));
        break;
    }

    default: {
        return make_error(error_code::unexpected_token, token.get_type());
    }
    }

    return std::make_unique<comparison_node>(std::move(left), std::move(right));
}

result<par::unique_node>
par::parser::handle_condition(lex::token_type ttype)
{
    condition cond{};
    switch (ttype)
    {
    case lex::token_type::ampersand:
        cond = condition::equal;
        break;

    case lex::token_type::cpref_lt:
        cond = condition::less_than;
        break;

    case lex::token_type::cpref_gt:
        cond = condition::greater_than;
        break;

    case lex::token_type::cpref_ne:
        cond = condition::nonequal;
        break;

    case lex::token_type::cpref_le:
        cond = condition::less_or_equal;
        break;

    case lex::token_type::cpref_ge:
        cond = condition::greater_or_equal;
        break;
    }

    unique_node action{};
    RETURN_IF_ERROR(action, handle());
    return std::make_unique<condition_node>(cond, std::move(action));
}

result<par::unique_node>
par::parser::handle_declaration()
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    unique_node object{};
    RETURN_IF_ERROR(object, handle_object(token.get_type()));
    return std::make_unique<declaration_node>(std::move(object));
}

result<par::unique_node>
par::parser::handle_label()
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    if (lex::token_type::name != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    return std::make_unique<label_node>(std::move(token.get_text()));
}

result<par::unique_node>
par::parser::handle_number(int number)
{
    return std::make_unique<number_node>(number);
}

result<par::unique_node>
par::parser::handle_object(lex::token_type ttype)
{
    lex::token token{};

    object_type type{};
    switch (ttype)
    {
    case lex::token_type::byref:
        type = object_type::text;
        break;

    default:
        return make_error(error_code::unexpected_token, token.get_type());
    }

    ustring name{};
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
par::parser::handle_register(const ustring &name)
{
    cpu_register reg{};

    auto it = name.begin();
    switch (std::toupper(*it++))
    {
    case 'A':
        reg = cpu_register::a;
        break;

    case 'B':
        reg = cpu_register::b;
        break;

    case 'C':
        reg = cpu_register::c;
        break;

    case 'D':
        reg = ('I' == std::toupper(*it)) ? cpu_register::dst : cpu_register::d;
        break;

    case 'S':
        reg = cpu_register::src;
        break;
    }

    uint16_t size{};
    switch (std::toupper(*it))
    {
    case 'L':
        size = cpu_register_lbyte;
        break;

    case 'H':
        size = cpu_register_hbyte;
        break;

    case 'X':
        size = cpu_register_word;
        if (cpu_register::src == reg)
        {
            return make_error(error_code::unexpected_token,
                              lex::token_type::name);
        }
        break;

    case 'I':
        size = cpu_register_word;
        if ((cpu_register::src != reg) && (cpu_register::dst != reg))
        {
            return make_error(error_code::unexpected_token,
                              lex::token_type::name);
        }
        break;
    }

    if ((cpu_register_word != size) &&
        ((cpu_register::src == reg) || (cpu_register::dst == reg)))
    {
        return make_error(error_code::unexpected_token, lex::token_type::name);
    }

    return std::make_unique<register_node>(
        static_cast<cpu_register>(static_cast<uint16_t>(reg) | size));
}

result<par::unique_node>
par::parser::handle_string(const ustring &str)
{
    return std::make_unique<string_node>(str);
}
