#include <cstdio>
#include <cstring>

#include <zd/lex/lexer.hpp>

static zd::lex::pl_istream
_get_pl_stream(int argc, char *argv[])
{
    if (1 < argc)
    {
        zd::ustring         path{argv[1]};
        zd::text::encoding *encoding{zd::text::encoding::unknown};

        if (2 < argc)
        {
            if (0 == std::strcmp(argv[2], "dos"))
            {
                encoding = zd::text::encoding::ibm852;
            }
            else if (0 == std::strcmp(argv[2], "iso"))
            {
                encoding = zd::text::encoding::iso_8859_2;
            }
            else if (0 == std::strcmp(argv[2], "maz"))
            {
                encoding = zd::text::encoding::x_mazovia;
            }
            else if (0 == std::strcmp(argv[2], "utf"))
            {
                encoding = zd::text::encoding::utf_8;
            }
            else if (0 == std::strcmp(argv[2], "win"))
            {
                encoding = zd::text::encoding::windows_1250;
            }
        }

        return zd::lex::pl_istream{zd::io::min_istream{path}, encoding};
    }

    return zd::lex::pl_istream{zd::io::min_istream{stdin}};
}

extern void
print_error(const zd::error &err);

int
main(int argc, char *argv[])
{
    auto stream = _get_pl_stream(argc, argv);

    zd::lex::lexer lexer{stream};
    while (stream)
    {
        auto token = lexer.get_token();
        if (!token)
        {
            zd::error err = std::move(token.error());
            print_error(err);
            break;
        }

        std::printf("\t%-3u %-16s", lexer.get_spaces(),
                    zd::to_string(token->get_type()).data());
        if (zd::lex::token_type::eof == token->get_type())
        {
            break;
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

    return 0;
}
