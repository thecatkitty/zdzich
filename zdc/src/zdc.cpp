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
        std::cout << '\t' << std::setw(16) << std::left
                  << zd::to_string(token.get_type());

        auto text = token.get_text();
        if (!text.empty())
        {
            std::cout << token.get_text();
        }
        else if (zd::token_type::literal_int == token.get_type())
        {
            std::cout << token.get_number();
        }

        std::cout << std::endl;
    }

    return 0;
}
