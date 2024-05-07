#include <cstdio>
#include <cstring>

#include <zd/containers.hpp>
#include <zd/gen/text_generator.hpp>
#include <zd/lex/lexer.hpp>
#include <zd/par/parser.hpp>

extern void
print_error(const zd::error &err);

static int
action_lexer(zd::lex::lexer &lexer);

static int
action_parser(zd::lex::lexer &lexer);

static bool
is_path_separator(int ch);

int
main(int argc, char *argv[])
{
    // Process command line arguments
    const char *opt_action{"P"};
    const char *opt_enc{""};
    const char *opt_path{""};

    for (auto arg : zd::range<char *>(argv + 1, argc - 1))
    {
        if ('-' != arg[0])
        {
            opt_path = arg;
            continue;
        }

        if (('e' == arg[1]) && (':' == arg[2]))
        {
            opt_enc = arg + 3;
            continue;
        }

        if (zd::contains("LP", arg[1]))
        {
            opt_action = arg + 1;
            continue;
        }
    }

    // Prepare configuration from arguments
    zd::text::encoding *encoding =
        (0 == std::strcmp(opt_enc, "dos"))   ? zd::text::encoding::ibm852
        : (0 == std::strcmp(opt_enc, "iso")) ? zd::text::encoding::iso_8859_2
        : (0 == std::strcmp(opt_enc, "maz")) ? zd::text::encoding::x_mazovia
        : (0 == std::strcmp(opt_enc, "utf")) ? zd::text::encoding::utf_8
        : (0 == std::strcmp(opt_enc, "win")) ? zd::text::encoding::windows_1250
                                             : zd::text::encoding::unknown;

    zd::lex::pl_istream stream{(opt_path[0] && std::strcmp("--", opt_path))
                                   ? zd::io::min_istream{opt_path}
                                   : stdin,
                               encoding};

    zd::lex::lexer lexer{stream};

    // Execute the actual action
    if ('L' == *opt_action)
    {
        return action_lexer(lexer);
    }

    if ('P' == *opt_action)
    {
        return action_parser(lexer);
    }

    std::fprintf(stderr, "unknown action %c\n", *opt_action);
    return 1;
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
    zd::par::parser         parser{lexer};
    zd::gen::text_generator generator{stdout};

    zd::result<zd::par::unique_node> result{};
    while (true)
    {
        result = std::move(parser.handle());
        if (!result)
        {
            zd::error err = std::move(result.error());
            if (err.is<zd::par::parser>(zd::par::parser::error_code::eof))
            {
                // End of file
                return 0;
            }

            print_error(err);
            return 1;
        }

        auto &node = *result;
        if (node->is<zd::par::include_node>())
        {
            auto inc_node = static_cast<zd::par::include_node *>(node.get());
            if (!inc_node->is_binary)
            {
                // Inclusion directive - #Wstaw
                zd::ustring inc_path{};

                auto &self = lexer.get_path();
                if (!self.empty())
                {
                    // Get parent path
                    auto dir_end = ++zd::find_last_if(self.begin(), self.end(),
                                                      is_path_separator);

                    // Create included file path
                    std::for_each(self.begin(), dir_end, [&inc_path](int ch) {
                        inc_path.append(ch);
                    });
                }

                inc_path.append(inc_node->name);

                // Process the included file
                zd::lex::pl_istream inc_stream{
                    inc_path, lexer.get_stream().get_encoding()};
                zd::lex::lexer inc_lexer{inc_stream};

                int status = action_parser(inc_lexer);
                if (0 != status)
                {
                    return status;
                }

                continue;
            }
        }

        node->generate(&generator);
        std::puts("");
    }
}

bool
is_path_separator(int ch)
{
#if defined(_WIN32) || defined(__ia16__)
    if ('\\' == ch)
    {
        return true;
    }
#endif

    return '/' == ch;
}
