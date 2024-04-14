#include <iostream>

#include <zd/pl_istream.hpp>

int
main(int argc, char *argv[])
{
    std::cout << zd::encoding::unknown->get_name() << std::endl;
    std::cout << zd::encoding::ibm852->get_name() << std::endl;
    std::cout << zd::encoding::windows_1250->get_name() << std::endl;
    std::cout << zd::encoding::iso_8859_2->get_name() << std::endl;
    std::cout << zd::encoding::utf_8->get_name() << std::endl;
    std::cout << zd::encoding::x_iso_or_windows->get_name() << std::endl;
    return 0;
}
