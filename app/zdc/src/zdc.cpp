#include <cstdio>

#include <zd/containers.hpp>
#include <zd/gen/text_generator.hpp>
#include <zd/gen/zd4_generator.hpp>
#include <zd/message.hpp>
#include <zd/unit.hpp>

#include "zdc.hpp"

static int
action_compiler(zd::lex::lexer &lexer, std::FILE *output);

static int
action_lexer(zd::lex::lexer &lexer);

static int
action_parser(zd::lex::lexer &lexer);

static void
print_error(const zd::error &err);

int
main(int argc, char *argv[])
{
    // Process command line arguments
    const char *opt_action{"C"};
    const char *opt_enc{""};
    const char *opt_input{""};
    const char *opt_output{""};

    for (auto arg : zd::range<char *>(argv + 1, argc - 1))
    {
        if ('-' != arg[0])
        {
            opt_input = arg;
            continue;
        }

        if (('e' == arg[1]) && (':' == arg[2]))
        {
            opt_enc = arg + 3;
            continue;
        }

        if (('o' == arg[1]) && (':' == arg[2]))
        {
            opt_output = arg + 3;
            continue;
        }

        if (zd::contains("CLP", arg[1]))
        {
            opt_action = arg + 1;
            continue;
        }
    }

    // Prepare configuration from arguments
    zd::text::encoding *encoding = zd::text::encoding::from_name(opt_enc);
    zd::lex::pl_istream stream{(opt_input[0] && std::strcmp("--", opt_input))
                                   ? zd::io::min_istream{opt_input}
                                   : stdin,
                               encoding};

    zd::lex::lexer lexer{stream};

    // Execute the actual action
    if ('C' == *opt_action)
    {
        if (!*opt_output)
        {
            zd::message::retrieve(MSG_ERROR).print(stderr);
            zd::message::retrieve(MSG_NO_OUTPUT).print(stderr);
            std::fputs("\n", stderr);
            return 1;
        }

        auto output = std::fopen(opt_output, "wb");
        if (!output)
        {
            std::perror(opt_output);
            return 1;
        }

        return action_compiler(lexer, output);
    }

    if ('L' == *opt_action)
    {
        return action_lexer(lexer);
    }

    if ('P' == *opt_action)
    {
        return action_parser(lexer);
    }

    assert(false && "unhandled action");
    return 1;
}

int
action_compiler(zd::lex::lexer &lexer, std::FILE *output)
{
    zd::gen::zd4_generator generator{};
    zd::unit               unit{lexer, generator};

    auto err = std::move(unit.process());
    if (!err)
    {
        print_error(err);
        return 1;
    }

    auto link_ret = generator.link(output);
    if (!link_ret)
    {
        zd::error err = std::move(link_ret.error());
        print_error(err);
        return 1;
    }

    return 0;
}

int
action_lexer(zd::lex::lexer &lexer)
{
    while (true)
    {
        auto token = lexer.get_token();
        if (!token)
        {
            zd::error err = std::move(token.error());
            print_error(err);
            return 1;
        }

        std::printf("\t%-3u %-16s", lexer.get_spaces(),
                    zd::to_string(token->get_type()).data());
        if (zd::lex::token_type::eof == token->get_type())
        {
            return 0;
        }

        auto text = token->get_text();
        if (!text.empty())
        {
            std::printf("%s", token->get_text().data());
        }
        else if (zd::lex::token_type::literal_int == token->get_type())
        {
            std::printf("%d", token->get_number());
        }

        std::puts("");
    }
}

int
action_parser(zd::lex::lexer &lexer)
{
    zd::gen::text_generator generator{stdout};
    zd::unit                unit{lexer, generator};

    auto err = std::move(unit.process());
    if (!err)
    {
        print_error(err);
        return 1;
    }

    return 0;
}

void
print_error(const zd::error &err)
{
    auto path = err.file().empty() ? "input" : err.file().data();
    std::fprintf(stderr, "%s:", path);
    if (err.line())
    {
        std::fprintf(stderr, "%u:", err.line());

        if (err.column())
        {
            std::fprintf(stderr, "%u:", err.column());
        }
    }

    std::fputc(' ', stderr);
    zd::message::retrieve(MSG_ERROR).print(stderr);
    zd::message::retrieve((err.origin() << 8) | err.ordinal(), err.size(),
                          err.begin())
        .print(stderr);
    std::fputs("\n", stderr);
}
