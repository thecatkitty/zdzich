#include <cstdio>

#include <zd/lexer.hpp>

static zd::pl_istream
_get_pl_stream(int argc, char *argv[])
{
    if (1 < argc)
    {
        zd::ustring path{argv[1]};
        return zd::pl_istream{zd::min_istream{path}};
    }

    return zd::pl_istream{zd::min_istream{stdin}};
}

int
main(int argc, char *argv[])
{
    auto stream = _get_pl_stream(argc, argv);

    zd::lexer lexer{stream};
    while (stream)
    {
        auto token = lexer.get_token();
        if (!token)
        {
            zd::error err = std::move(token.error());
            std::fprintf(stderr, "error %02X%02X", err.origin(), err.ordinal());
            for (auto arg : err)
            {
                std::fprintf(stderr, " %p", reinterpret_cast<void *>(arg));
            }
            std::fputs("\n", stderr);
            break;
        }

        std::printf("\t%-16s", zd::to_string(token->get_type()).data());
        if (zd::token_type::eof == token->get_type())
        {
            break;
        }

        auto text = token->get_text();
        if (!text.empty())
        {
            std::printf("%s", token->get_text().data());
        }
        else if (zd::token_type::literal_int == token->get_type())
        {
            std::printf("%d", token->get_number());
        }

        std::puts("");
    }

    return 0;
}
