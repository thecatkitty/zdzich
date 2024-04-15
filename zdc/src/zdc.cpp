#include <iostream>

#include <zd/pl_istream.hpp>

int
main(int argc, char *argv[])
{
    if (2 > argc)
    {
        std::cerr << __FUNCTION__ << ": file name not provided" << std::endl;
        return 1;
    }

    zd::pl_istream istr{argv[1]};
    while (istr)
    {
        auto codepoint = istr.read();
        if (0 == codepoint)
        {
            break;
        }

        auto encoding = istr.get_encoding()->get_name();
        std::cout << codepoint << '\t' << encoding << std::endl;
    }

    return 0;
}
