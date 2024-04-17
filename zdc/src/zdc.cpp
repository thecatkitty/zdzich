#include <iomanip>
#include <iostream>

#include <zd/lexer.hpp>

static zd::pl_istream
_get_pl_stream(int argc, char *argv[])
{
    if (1 < argc)
    {
        std::string path{argv[1]};
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
        std::cout << '\t' << std::setw(16) << std::left << zd::to_string(token);

        switch (token)
        {
        case zd::token::name:
        case zd::token::literal_str: {
            std::cout << lexer.get_string();
            break;
        }

        case zd::token::literal_int: {
            std::cout << lexer.get_integer();
            break;
        }
        }

        std::cout << std::endl;
    }

    return 0;
}
