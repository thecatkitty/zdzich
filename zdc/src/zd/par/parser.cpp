#include <cctype>
#include <cstdio>

#include <zd/par/parser.hpp>
#include <zd/text/pl_string.hpp>

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

        case lex::token_type::byref:
        case lex::token_type::byval:
            return handle_assignment(token.get_type());

        case lex::token_type::end:
            return handle_end();

        case lex::token_type::colon:
            return handle_label();

        case lex::token_type::compare:
        case lex::token_type::decrement:
        case lex::token_type::increment:
            return handle_operation(token.get_type());

        case lex::token_type::jump:
            return handle_jump();

        case lex::token_type::name:
            return handle_call(token.get_text());

        case lex::token_type::procedure:
            return handle_procedure();

        case lex::token_type::variable:
            return handle_declaration();

        default:
            return make_error(error_code::unexpected_token, token.get_type());
        }
    }
}

result<par::unique_node>
par::parser::handle_assignment(lex::token_type ttype)
{
    unique_node target{};
    RETURN_IF_ERROR(target, handle_object(ttype));

    lex::token token{};

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::assign != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    unique_node source{};
    RETURN_IF_ERROR(source, handle_value());

    return std::make_unique<assignment_node>(std::move(target),
                                             std::move(source));
}

result<par::unique_node>
par::parser::handle_call(const ustring &callee)
{
    node_list  arguments{};
    bool       enclosed{false};
    bool       more{true};
    lex::token token{};

    while (more)
    {
        auto result = handle_value();
        if (!result)
        {
            auto err = std::move(result.error());
            if (err.is(error_origin::parser, error_code::unexpected_token))
            {
                auto tts = reinterpret_cast<const char *>(err[0]);
                if (!enclosed && to_cstr(lex::token_type::lbracket) == tts)
                {
                    // Opening bracket
                    enclosed = true;
                    continue;
                }

                if (enclosed && to_cstr(lex::token_type::rbracket) == tts)
                {
                    // Closing bracket
                    enclosed = false;
                    continue;
                }
            }

            return tl::make_unexpected(std::move(result.error()));
        }

        unique_node arg = std::move(*result);
        if (!arg)
        {
            // End of arguments
            break;
        }

        RETURN_IF_ERROR(token, _lexer.get_token());
        switch (token.get_type())
        {
        case lex::token_type::eof:
        case lex::token_type::line_break:
            more = false;
            // Pass through
        case lex::token_type::comma:
            arguments.push_back(std::move(arg));
            continue;

        default: {
            return make_error(error_code::unexpected_token, token.get_type());
        }
        }
    }

    return std::make_unique<call_node>(callee, std::move(arguments));
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
par::parser::handle_end()
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    ustring name{};
    switch (token.get_type())
    {
    case lex::token_type::name:
        name = token.get_text();
        break;

    case lex::token_type::eof:
    case lex::token_type::line_break:
        return std::make_unique<end_node>();

    default:
        return make_error(error_code::unexpected_token, token.get_type());
    }

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::lbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::rbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    return std::make_unique<end_node>(std::move(name));
}

result<par::unique_node>
par::parser::handle_jump()
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::colon != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    unique_node target{};
    RETURN_IF_ERROR(target, handle_label());
    return std::make_unique<jump_node>(std::move(target));
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
    object_type type{};
    switch (ttype)
    {
    case lex::token_type::byref:
        type = object_type::text;
        break;

    case lex::token_type::byval:
        type = object_type::word;
        break;

    default:
        return make_error(error_code::unexpected_token, ttype);
    }

    lex::token token{};

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
par::parser::handle_operation(lex::token_type ttype)
{
    operation op{};
    switch (ttype)
    {
    case lex::token_type::compare:
        op = operation::compare;
        break;

    case lex::token_type::decrement:
        op = operation::subtract;
        break;

    case lex::token_type::increment:
        op = operation::add;
        break;
    }

    unique_node left{};
    RETURN_IF_ERROR(left, handle_value());

    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::comma != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    unique_node right{};
    RETURN_IF_ERROR(right, handle_value());

    return std::make_unique<operation_node>(op, std::move(left),
                                            std::move(right));
}

result<par::unique_node>
par::parser::handle_procedure()
{
    lex::token token{};

    ustring name{};
    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::name != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }
    name = token.get_text();

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::lbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::rbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    node_list body{};
    while (true)
    {
        auto result = handle();
        if (!result)
        {
            if (result.error().is(error_origin::parser, error_code::eof))
            {
                return make_error(error_code::unexpected_eof,
                                  lex::token_type::procedure);
            }

            return tl::make_unexpected(std::move(result.error()));
        }
        unique_node node = std::move(*result);

        // Check for the end of procedure
        if (node->is<end_node>())
        {
            auto end = reinterpret_cast<end_node *>(node.get());
            if (!end->get_name().empty() &&
                text::pl_streqai(name, end->get_name()))
            {
                break;
            }
        }

        body.push_back(std::move(node));
    }

    return std::make_unique<procedure_node>(name, std::move(body));
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

result<par::unique_node>
par::parser::handle_value()
{
    lex::token  token{};
    unique_node value{};

    RETURN_IF_ERROR(token, _lexer.get_token());
    switch (token.get_type())
    {
    case lex::token_type::name: {
        RETURN_IF_ERROR(value, handle_register(token.get_text()));
        break;
    }

    case lex::token_type::literal_int: {
        RETURN_IF_ERROR(value, handle_number(token.get_number()));
        break;
    }

    case lex::token_type::literal_str: {
        RETURN_IF_ERROR(value, handle_string(token.get_text()));
        break;
    }

    case lex::token_type::byref:
    case lex::token_type::byval: {
        RETURN_IF_ERROR(value, handle_object(token.get_type()));
        break;
    }

    case lex::token_type::eof:
    case lex::token_type::line_break: {
        return nullptr;
    }

    default: {
        return make_error(error_code::unexpected_token, token.get_type());
    }
    }

    return std::move(value);
}
