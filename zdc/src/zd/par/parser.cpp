#include <cctype>
#include <cstdio>

#include <zd/par/parser.hpp>
#include <zd/text/characters.hpp>
#include <zd/text/pl_string.hpp>

using namespace zd;

static bool
_can_be_name(const lex::token &token)
{
    switch (token.get_type())
    {
    case lex::token_type::end:
    case lex::token_type::name:
        return true;
    }

    return false;
}

static par::cpu_register
_to_cpu_register(const ustring &str)
{
    par::cpu_register reg{};

    auto it = str.begin();
    switch (std::toupper(*it++))
    {
    case 'A':
        reg = par::cpu_register::a;
        break;

    case 'B':
        reg = par::cpu_register::b;
        break;

    case 'C':
        reg = par::cpu_register::c;
        break;

    case 'D':
        reg = ('I' == std::toupper(*it)) ? par::cpu_register::dst
                                         : par::cpu_register::d;
        break;

    case 'S':
        reg = par::cpu_register::src;
        break;
    }

    uint16_t size{};
    switch (std::toupper(*it))
    {
    case 'L':
        size = par::cpu_register_lbyte;
        break;

    case 'H':
        size = par::cpu_register_hbyte;
        break;

    case 'X':
        size = par::cpu_register_word;
        if (par::cpu_register::src == reg)
        {
            return par::cpu_register::invalid;
        }
        break;

    case 'I':
        size = par::cpu_register_word;
        if ((par::cpu_register::src != reg) && (par::cpu_register::dst != reg))
        {
            return par::cpu_register::invalid;
        }
        break;

    default:
        return par::cpu_register::invalid;
    }

    it++;
    if (str.end() != it)
    {
        return par::cpu_register::invalid;
    }

    if ((par::cpu_register_word != size) &&
        ((par::cpu_register::src == reg) || (par::cpu_register::dst == reg)))
    {
        return par::cpu_register::invalid;
    }

    return static_cast<par::cpu_register>(static_cast<uint16_t>(reg) | size);
}

result<par::unique_node>
par::parser::handle(const lex::token &head)
{
    bool first{true};
    while (true)
    {
        lex::token token{};

        if (first && (lex::token_type::unknown != head.get_type()))
        {
            token = head;
        }
        else
        {
            RETURN_IF_ERROR(token, _lexer.get_token());
        }

        first = false;

        switch (token.get_type())
        {
        case lex::token_type::line_break:
        case lex::token_type::comment:
            continue;

        case lex::token_type::eof:
            return make_error(error_code::eof);

        case lex::token_type::ampersand: {
            RETURN_IF_ERROR(token, _lexer.get_token());
            if (lex::token_type::name == token.get_type())
            {
                return handle_assignment(lex::token_type::ampersand,
                                         cpu_register::invalid,
                                         std::move(token.get_text()));
            }

            return handle_condition(lex::token_type::ampersand, token);
        }

        case lex::token_type::cpref_lt:
        case lex::token_type::cpref_gt:
        case lex::token_type::cpref_ne:
        case lex::token_type::cpref_le:
        case lex::token_type::cpref_ge:
            return handle_condition(token.get_type());

        case lex::token_type::byref:
        case lex::token_type::byval:
            return handle_assignment(token.get_type());

        case lex::token_type::directive:
            return handle_directive(token.get_text());

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

        case lex::token_type::name: {
            auto reg = _to_cpu_register(token.get_text());
            return (cpu_register::invalid == reg)
                       ? handle_call(token.get_text())
                       : handle_assignment(lex::token_type::name, reg);
        }

        case lex::token_type::procedure:
            return handle_procedure();

        case lex::token_type::constant:
        case lex::token_type::variable:
            return handle_declaration(lex::token_type::constant ==
                                      token.get_type());

        default:
            return make_error(error_code::unexpected_token, token.get_type());
        }
    }
}

result<par::unique_node>
par::parser::handle_assignment(lex::token_type ttype,
                               cpu_register    reg,
                               ustring         name)
{
    unique_node target{};
    if (cpu_register::invalid == reg)
    {
        RETURN_IF_ERROR(target, handle_object(ttype, name));
    }
    else
    {
        RETURN_IF_ERROR(target, handle_register(reg));
    }

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
par::parser::handle_call(const ustring &callee, bool enclosed)
{
    node_list  arguments{};
    bool       bare{true};
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
                    bare = false;
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

            if (err.is(error_origin::parser, error_code::name_expected))
            {
                auto tts = reinterpret_cast<const char *>(err[1]);
                if ((to_cstr(lex::token_type::eof) == tts) ||
                    (to_cstr(lex::token_type::line_break) == tts))
                {
                    // Irregular string literal reconstruction
                    // CZASN.INC:410 - Pisz %
                    auto tcs = reinterpret_cast<const char *>(err[0]);
                    char character =
                        (to_cstr(lex::token_type::ampersand) == tcs) ? '&'
                        : (to_cstr(lex::token_type::byval) == tcs)   ? '%'
                                                                     : '$';

                    ustring str{};
                    str.append(character);
                    arguments.push_back(std::make_unique<string_node>(str));
                    break;
                }
            }

            return tl::make_unexpected(std::move(err));
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

        case lex::token_type::rbracket:
            if (arg->is<string_node>())
            {
                // Irregular string literal reconstruction
                // DOGIER.INC:14 - PiszL (N)owa gra czy (k)oniec?
                auto arg_str = reinterpret_cast<string_node *>(arg.get());

                ustring str{};
                if (enclosed)
                {
                    str.append('(');
                }
                str.append(arg_str->value);
                str.append(')');

                enclosed = false;
                arguments.push_back(std::make_unique<string_node>(str));
                continue;
            }

            if (arg->is<number_node>())
            {
                // Irregular string literal reconstruction
                // ZDC-DLL.INC:108 - piszl   1)   Zmien nazwe pliku
                auto arg_num = reinterpret_cast<number_node *>(arg.get());

                char buff[20]{};
                std::snprintf(buff, sizeof(buff), "%s%d) ", enclosed ? "(" : "",
                              arg_num->value);

                ustring str{buff};
                str.append(token.get_text());

                enclosed = false;
                arguments.push_back(std::make_unique<string_node>(str));
                continue;
            }

            enclosed = false;
            return make_error(error_code::unexpected_token, token.get_type());

        case lex::token_type::literal_str:
            // Irregular string literal reconstruction:
            // LEK05-03.ZDI:6 - Pisz 1. Kurczak
            if (arg->is<number_node>())
            {
                auto arg_num = reinterpret_cast<number_node *>(arg.get());

                char buff[20]{};
                std::snprintf(buff, sizeof(buff), "%d%*s", arg_num->value,
                              _lexer.get_spaces(), "");

                ustring str{buff};
                str.append(token.get_text());

                arguments.push_back(std::make_unique<string_node>(str));
                continue;
            }

            // Pass through
        default: {
            return make_error(error_code::unexpected_token, token.get_type());
        }
        }
    }

    if (!arguments.empty())
    {
        bare = false;
    }

    return std::make_unique<call_node>(callee, std::move(arguments), bare);
}

result<par::unique_node>
par::parser::handle_condition(lex::token_type ttype, const lex::token &head)
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
    RETURN_IF_ERROR(action, handle(head));
    return std::make_unique<condition_node>(cond, std::move(action));
}

result<par::unique_node>
par::parser::handle_declaration(bool is_const)
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    unique_node object{};

    if (is_const)
    {
        RETURN_IF_ERROR(object, handle_assignment(token.get_type()));
    }
    else
    {
        RETURN_IF_ERROR(object, handle_object(token.get_type()));
    }

    return std::make_unique<declaration_node>(std::move(object), is_const);
}

result<par::unique_node>
par::parser::handle_directive(const ustring &directive)
{
    auto it = directive.begin();
    if (',' != *it)
    {
        return make_error(error_code::unknown_directive);
    }

    it++;

    std::vector<uint8_t> bytes{};
    int                  number{};
    while (directive.end() != it)
    {
        if (text::isdigit(*it))
        {
            number *= 10;
            number += *it - '0';
        }
        else if (',' == *it)
        {
            if (255 < number)
            {
                return make_error(error_code::out_of_range, number);
            }

            bytes.push_back(static_cast<uint8_t>(number));
            number = 0;
        }
        else
        {
            return make_error(error_code::unexpected_token,
                              lex::token_type::unknown);
        }

        it++;
    }
    bytes.push_back(static_cast<uint8_t>(number));

    return std::make_unique<emit_node>(std::move(bytes));
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

    // SAVER.INC:51 - Koniec KONIEC()
    case lex::token_type::end:
        name = "Koniec";
        break;

    // SAVER.INC:45 - KONIEC()
    case lex::token_type::lbracket:
        return handle_call("Koniec", true);

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

    if (!_can_be_name(token))
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
par::parser::handle_object(lex::token_type ttype, ustring name)
{
    object_type type{};
    switch (ttype)
    {
    case lex::token_type::ampersand:
        type = object_type::byte;
        break;

    case lex::token_type::byref:
        type = object_type::text;
        break;

    case lex::token_type::byval:
        type = object_type::word;
        break;

    default:
        return make_error(error_code::unexpected_token, ttype);
    }

    if (!name.empty())
    {
        return std::make_unique<object_node>(std::move(name), type);
    }

    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    if (!_can_be_name(token))
    {
        int prefix = (lex::token_type::ampersand == ttype) ? '&'
                     : (lex::token_type::byval == ttype)   ? '%'
                                                           : '$';
        if (lex::token_type::literal_str == token.get_type())
        {
            // Irregular string literal reconstruction
            // CZASN.INC:397 - Pisz %<SP>
            ustring str{};
            str.append(prefix);

            if (lex::token_type::literal_str == token.get_type())
            {
                auto spaces = _lexer.get_spaces();
                for (int i = 0; i < spaces; i++)
                {
                    str.append(' ');
                }

                str.append(token.get_text());
            }
            return std::make_unique<string_node>(std::move(str));
        }

        return make_error(error_code::name_expected, ttype, token.get_type());
    }

    return std::make_unique<object_node>(std::move(token.get_text()), type);
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
    switch (token.get_type())
    {
    case lex::token_type::comma: {
        unique_node right{};
        RETURN_IF_ERROR(right, handle_value());
        return std::make_unique<operation_node>(op, std::move(left),
                                                std::move(right));
    }

    case lex::token_type::eof:
    case lex::token_type::line_break:
        if ((operation::subtract == op) || (operation::add == op))
        {
            return std::make_unique<operation_node>(
                op, std::move(left), std::make_unique<number_node>(1));
        }
        // Pass through
    default:
        return make_error(error_code::unexpected_token, token.get_type());
    }
}

result<par::unique_node>
par::parser::handle_procedure()
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    if (!_can_be_name(token))
    {
        return make_error(error_code::unexpected_token, token.get_type());
    }

    ustring name = std::move(token.get_text());

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
            if (!end->name.empty() && text::pl_streqai(name, end->name))
            {
                break;
            }
        }

        body.push_back(std::move(node));
    }

    return std::make_unique<procedure_node>(name, std::move(body));
}

result<par::unique_node>
par::parser::handle_register(cpu_register reg)
{
    return std::make_unique<register_node>(reg);
}

result<par::unique_node>
par::parser::handle_string(const ustring &str, int spaces)
{
    if (!spaces)
    {
        return std::make_unique<string_node>(str);
    }

    ustring padded_str{};
    for (int i = 0; i < spaces; i++)
    {
        padded_str.append(' ');
    }
    padded_str.append(str);
    return std::make_unique<string_node>(padded_str);
}

result<par::unique_node>
par::parser::handle_subscript()
{
    unique_node value{};
    RETURN_IF_ERROR(value, handle_value());
    return std::make_unique<subscript_node>(std::move(value));
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
        auto reg = _to_cpu_register(token.get_text());
        if (cpu_register::invalid != reg)
        {
            RETURN_IF_ERROR(value, handle_register(reg));
            break;
        }
        // Pass through
    }
    case lex::token_type::literal_str: {
        auto spaces = _lexer.get_spaces();
        RETURN_IF_ERROR(value, handle_string(token.get_text(),
                                             (spaces < 1) ? 0 : (spaces - 1)));
        break;
    }

    case lex::token_type::literal_int: {
        RETURN_IF_ERROR(value, handle_number(token.get_number()));
        break;
    }

    case lex::token_type::ampersand:
    case lex::token_type::byref:
    case lex::token_type::byval: {
        RETURN_IF_ERROR(value, handle_object(token.get_type()));
        break;
    }

    case lex::token_type::subscript:
        RETURN_IF_ERROR(value, handle_subscript());
        break;

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
