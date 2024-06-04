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

    if (1 == str.size())
    {
        // Flags
        switch (std::toupper(*it))
        {
        case 'C':
            return par::cpu_register::flag_c;

        case 'D':
            return par::cpu_register::flag_d;

        case 'I':
            return par::cpu_register::flag_i;
        }

        return par::cpu_register::invalid;
    }

    // General purpose register
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
        auto       pos = track();
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
                return handle_assignment(pos, lex::token_type::ampersand,
                                         cpu_register::invalid,
                                         std::move(token.get_text()));
            }

            return handle_condition(pos, lex::token_type::ampersand, token);
        }

        case lex::token_type::cpref_lt:
        case lex::token_type::cpref_gt:
        case lex::token_type::cpref_ne:
        case lex::token_type::cpref_le:
        case lex::token_type::cpref_ge:
            return handle_condition(pos, token.get_type());

        case lex::token_type::byref:
        case lex::token_type::byval:
            return handle_assignment(pos, token.get_type());

        case lex::token_type::directive:
            return handle_directive(pos, token.get_text());

        case lex::token_type::end:
            return handle_end(pos);

        case lex::token_type::colon:
            return handle_label(pos);

        case lex::token_type::compare:
        case lex::token_type::decrement:
        case lex::token_type::increment:
            return handle_operation(pos, token.get_type());

        case lex::token_type::jump:
            return handle_jump(pos);

        case lex::token_type::name: {
            auto reg = _to_cpu_register(token.get_text());
            return (cpu_register::invalid == reg)
                       ? handle_call(pos, token.get_text())
                       : handle_assignment(pos, lex::token_type::name, reg);
        }

        case lex::token_type::procedure:
            return handle_procedure(pos);

        case lex::token_type::constant:
        case lex::token_type::variable:
            return handle_declaration(pos, lex::token_type::constant ==
                                               token.get_type());

        default:
            return make_error(error_code::not_a_command, token.get_type());
        }
    }
}

par::position
par::parser::track()
{
    return {_lexer.get_path(), _lexer.get_line(), _lexer.get_column()};
}

result<par::unique_node>
par::parser::handle_assignment(const position &pos,
                               lex::token_type ttype,
                               cpu_register    reg,
                               ustring         name)
{
    unique_node target{};
    if (cpu_register::invalid == reg)
    {
        RETURN_IF_ERROR(target, handle_object(pos, ttype, name));
    }
    else
    {
        RETURN_IF_ERROR(target, handle_register(pos, reg));
    }

    auto       pos_op = track();
    lex::token token{};

    RETURN_IF_ERROR(token, _lexer.get_token());
    switch (token.get_type())
    {
    case lex::token_type::minus:
        // Boolean disable
        return std::make_unique<assignment_node>(
            pos_op, std::move(target),
            std::make_unique<number_node>(pos_op, 0));

    case lex::token_type::plus:
        // Boolean enable
        return std::make_unique<assignment_node>(
            pos_op, std::move(target),
            std::make_unique<number_node>(pos_op, 1));

    case lex::token_type::assign:
        // A regular assignment
        break;

    default:
        return make_error(error_code::unexpected_token, token.get_type(),
                          "assignment");
    }

    auto        pos_val = track();
    unique_node source{};
    RETURN_IF_ERROR(source, handle_value(pos_val));

    return std::make_unique<assignment_node>(pos, std::move(target),
                                             std::move(source));
}

result<par::unique_node>
par::parser::handle_call(const position &pos,
                         const ustring  &callee,
                         bool            enclosed)
{
    node_list  arguments{};
    bool       bare{true};
    bool       more{true};
    lex::token token{};

    while (more)
    {
        auto pos_arg = track();
        auto result = handle_value(pos_arg);
        if (!result)
        {
            auto err = std::move(result.error());
            if (err.is<parser>(error_code::unexpected_token))
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

            if (err.is<parser>(error_code::name_expected))
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
                    arguments.push_back(
                        std::make_unique<string_node>(pos_arg, str));
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
                auto arg_str = arg->as<string_node>();

                ustring str{};
                if (enclosed)
                {
                    str.append('(');
                }
                str.append(arg_str->value);
                str.append(')');

                enclosed = false;
                arguments.push_back(
                    std::make_unique<string_node>(pos_arg, str));
                continue;
            }

            if (arg->is<number_node>())
            {
                // Irregular string literal reconstruction
                // ZDC-DLL.INC:108 - piszl   1)   Zmien nazwe pliku
                auto arg_num = arg->as<number_node>();

                char buff[20]{};
                std::snprintf(buff, sizeof(buff), "%s%d) ", enclosed ? "(" : "",
                              arg_num->value);

                ustring str{buff};
                str.append(token.get_text());

                enclosed = false;
                arguments.push_back(
                    std::make_unique<string_node>(pos_arg, str));
                continue;
            }

            enclosed = false;
            return make_error(error_code::unexpected_token, token.get_type(),
                              "call");

        case lex::token_type::literal_str:
            // Irregular string literal reconstruction:
            // LEK05-03.ZDI:6 - Pisz 1. Kurczak
            if (arg->is<number_node>())
            {
                auto arg_num = arg->as<number_node>();

                char buff[20]{};
                std::snprintf(buff, sizeof(buff), "%d%*s", arg_num->value,
                              _lexer.get_spaces(), "");

                ustring str{buff};
                str.append(token.get_text());

                arguments.push_back(
                    std::make_unique<string_node>(pos_arg, str));
                continue;
            }

            // Pass through
        default: {
            return make_error(error_code::unexpected_token, token.get_type(),
                              "call");
        }
        }
    }

    if (!arguments.empty())
    {
        bare = false;
    }

    return std::make_unique<call_node>(pos, callee, std::move(arguments), bare);
}

result<par::unique_node>
par::parser::handle_condition(const position   &pos,
                              lex::token_type   ttype,
                              const lex::token &head)
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
    return std::make_unique<condition_node>(pos, cond, std::move(action));
}

result<par::unique_node>
par::parser::handle_declaration(const position &pos, bool is_const)
{
    auto       pos_obj = track();
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    unique_node object{};

    if (is_const)
    {
        RETURN_IF_ERROR(object, handle_assignment(pos_obj, token.get_type()));
    }
    else
    {
        RETURN_IF_ERROR(object, handle_object(pos_obj, token.get_type()));
    }

    return std::make_unique<declaration_node>(pos, std::move(object), is_const);
}

result<par::unique_node>
par::parser::handle_directive(const position &pos, const ustring &directive)
{
    auto name_end =
        std::find_if(directive.begin(), directive.end(), text::isspace);
    if (directive.end() != name_end)
    {
        ustring name{};
        std::for_each(directive.begin(), name_end, [&name](int ch) {
            name.append(ch);
        });

        if (text::pl_streqi("Wstaw", name))
        {
            // #Wstaw
            auto path_begin =
                std::find_if_not(name_end, directive.end(), text::isspace);
            if (directive.end() == path_begin)
            {
                return make_error(error_code::path_expected);
            }

            return std::make_unique<include_node>(pos, path_begin.get());
        }

        if (text::pl_streqi("WstawBin", name))
        {
            // #WstawBin
            auto path_begin =
                std::find_if_not(name_end, directive.end(), text::isspace);
            if (directive.end() == path_begin)
            {
                return make_error(error_code::path_expected);
            }

            return std::make_unique<include_node>(pos, path_begin.get(), true);
        }
    }

    // #,
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

            if (255 < number)
            {
                return make_error(error_code::out_of_range, number);
            }
        }
        else if (',' == *it)
        {
            bytes.push_back(static_cast<uint8_t>(number));
            number = 0;
        }
        else
        {
            return make_error(error_code::cannot_emit, *it);
        }

        it++;
    }
    bytes.push_back(static_cast<uint8_t>(number));

    return std::make_unique<emit_node>(pos, std::move(bytes));
}

result<par::unique_node>
par::parser::handle_end(const position &pos)
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
        return handle_call(pos, "Koniec", true);

    case lex::token_type::eof:
    case lex::token_type::line_break:
        return std::make_unique<end_node>(pos);

    default:
        return make_error(error_code::unexpected_token, token.get_type(),
                          "end");
    }

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::lbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "end");
    }

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::rbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "end");
    }

    return std::make_unique<end_node>(pos, std::move(name));
}

result<par::unique_node>
par::parser::handle_jump(const position &pos)
{
    auto       pos_colon = track();
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::colon != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "jump");
    }

    unique_node target{};
    RETURN_IF_ERROR(target, handle_label(pos_colon));
    return std::make_unique<jump_node>(pos, std::move(target));
}

result<par::unique_node>
par::parser::handle_label(const position &pos)
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    if (!_can_be_name(token))
    {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "label");
    }

    return std::make_unique<label_node>(pos, std::move(token.get_text()));
}

result<par::unique_node>
par::parser::handle_number(const position &pos, int number)
{
    return std::make_unique<number_node>(pos, number);
}

result<par::unique_node>
par::parser::handle_object(const position &pos,
                           lex::token_type ttype,
                           ustring         name)
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
        return make_error(error_code::unexpected_token, ttype, "object");
    }

    if (!name.empty())
    {
        return std::make_unique<object_node>(pos, std::move(name), type);
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
            return std::make_unique<string_node>(pos, std::move(str));
        }

        return make_error(error_code::name_expected, ttype, token.get_type());
    }

    return std::make_unique<object_node>(pos, std::move(token.get_text()),
                                         type);
}

result<par::unique_node>
par::parser::handle_operation(const position &pos, lex::token_type ttype)
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

    auto        pos_left = track();
    unique_node left{};
    RETURN_IF_ERROR(left, handle_value(pos_left));

    auto       pos_right = track();
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());
    switch (token.get_type())
    {
    case lex::token_type::comma: {
        unique_node right{};
        RETURN_IF_ERROR(right, handle_value(pos_right));
        return std::make_unique<operation_node>(pos, op, std::move(left),
                                                std::move(right));
    }

    case lex::token_type::eof:
    case lex::token_type::line_break:
        if ((operation::subtract == op) || (operation::add == op))
        {
            return std::make_unique<operation_node>(
                pos, op, std::move(left),
                std::make_unique<number_node>(pos_right, 1));
        }
        // Pass through
    default:
        return make_error(error_code::unexpected_token, token.get_type(),
                          "operation");
    }
}

result<par::unique_node>
par::parser::handle_procedure(const position &pos)
{
    lex::token token{};
    RETURN_IF_ERROR(token, _lexer.get_token());

    if (!_can_be_name(token))
    {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "procedure");
    }

    ustring name = std::move(token.get_text());

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::lbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "procedure");
    }

    RETURN_IF_ERROR(token, _lexer.get_token());
    if (lex::token_type::rbracket != token.get_type())
    {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "procedure");
    }

    node_list body{};
    while (true)
    {
        auto result = handle();
        if (!result)
        {
            if (result.error().is<parser>(error_code::eof))
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
            auto end = node->as<end_node>();
            if (!end->name.empty() && text::pl_streqai(name, end->name))
            {
                break;
            }
        }

        body.push_back(std::move(node));
    }

    return std::make_unique<procedure_node>(pos, name, std::move(body));
}

result<par::unique_node>
par::parser::handle_register(const position &pos, cpu_register reg)
{
    return std::make_unique<register_node>(pos, reg);
}

result<par::unique_node>
par::parser::handle_string(const position &pos, const ustring &str, int spaces)
{
    if (!spaces)
    {
        return std::make_unique<string_node>(pos, str);
    }

    ustring padded_str{};
    for (int i = 0; i < spaces; i++)
    {
        padded_str.append(' ');
    }
    padded_str.append(str);
    return std::make_unique<string_node>(pos, padded_str);
}

result<par::unique_node>
par::parser::handle_subscript(const position &pos)
{
    auto        pos_val = track();
    unique_node value{};
    RETURN_IF_ERROR(value, handle_value(pos_val));
    return std::make_unique<subscript_node>(pos, std::move(value));
}

result<par::unique_node>
par::parser::handle_value(const position &pos)
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
            RETURN_IF_ERROR(value, handle_register(pos, reg));
            break;
        }
        // Pass through
    }
    case lex::token_type::literal_str: {
        auto spaces = _lexer.get_spaces();
        RETURN_IF_ERROR(value, handle_string(pos, token.get_text(),
                                             (spaces < 1) ? 0 : (spaces - 1)));
        break;
    }

    case lex::token_type::literal_int: {
        RETURN_IF_ERROR(value, handle_number(pos, token.get_number()));
        break;
    }

    case lex::token_type::ampersand:
    case lex::token_type::byref:
    case lex::token_type::byval: {
        RETURN_IF_ERROR(value, handle_object(pos, token.get_type()));
        break;
    }

    case lex::token_type::subscript:
        RETURN_IF_ERROR(value, handle_subscript(pos));
        break;

    case lex::token_type::eof:
    case lex::token_type::line_break: {
        return nullptr;
    }

    default: {
        return make_error(error_code::unexpected_token, token.get_type(),
                          "value");
    }
    }

    return std::move(value);
}
