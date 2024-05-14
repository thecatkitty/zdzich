#include <cstdio>

#include <zd/containers.hpp>
#include <zd/gen/text_generator.hpp>
#include <zd/gen/zd4_generator.hpp>
#include <zd/lex/lexer.hpp>
#include <zd/message.hpp>
#include <zd/par/parser.hpp>

typedef bool (*node_callback)(zd::par::node *, void *);

static int
action_compiler(zd::lex::lexer &lexer, std::FILE *output);

static int
action_lexer(zd::lex::lexer &lexer);

static int
action_parser(zd::lex::lexer &lexer,
              node_callback   callback = nullptr,
              void           *context = nullptr);

static bool
is_path_separator(int ch);

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
    zd::text::encoding *encoding =
        (0 == std::strcmp(opt_enc, "dos"))   ? zd::text::encoding::ibm852
        : (0 == std::strcmp(opt_enc, "iso")) ? zd::text::encoding::iso_8859_2
        : (0 == std::strcmp(opt_enc, "maz")) ? zd::text::encoding::x_mazovia
        : (0 == std::strcmp(opt_enc, "utf")) ? zd::text::encoding::utf_8
        : (0 == std::strcmp(opt_enc, "win")) ? zd::text::encoding::windows_1250
                                             : zd::text::encoding::unknown;

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
            std::fputs("error: no output file name provided (argument -o)\n",
                       stderr);
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

    std::fprintf(stderr, "unknown action %c\n", *opt_action);
    return 1;
}

int
action_compiler(zd::lex::lexer &lexer, std::FILE *output)
{
    zd::gen::zd4_generator generator{};

    auto status = action_parser(
        lexer,
        [](zd::par::node *node, void *context) -> bool {
            if (!node->generate(
                    reinterpret_cast<zd::gen::generator *>(context)))
            {
                std::fputs("compilation error!\n", stderr);
                return false;
            }

            return true;
        },
        &generator);
    if (0 != status)
    {
        return status;
    }

    generator.link(output);
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
action_parser(zd::lex::lexer &lexer, node_callback callback, void *context)
{
    zd::par::parser         parser{lexer};
    zd::gen::text_generator text_generator{stdout};

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

                int status = action_parser(inc_lexer, callback, context);
                if (0 != status)
                {
                    return status;
                }

                continue;
            }
        }

        node->generate(&text_generator);

        if (callback)
        {
            if (!callback(node.get(), context))
            {
                return 1;
            }
        }
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

void
print_error(const zd::error &err)
{
    auto path = err.file().empty() ? "input" : err.file().data();
    std::fprintf(stderr, "%s:%u:%u: error: ", path, err.line(), err.column());

    zd::message::retrieve((err.origin() << 8) | err.ordinal(), err.size(),
                          err.begin())
        .print(stderr);

    std::fputs("\n", stderr);
}
