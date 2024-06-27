
#include <cstdio>

#include <zd/gen/zd4_generator.hpp>
#include <zd/message.hpp>
#include <zd/unit.hpp>

#include "../../zdc/src/zdc.hpp"

void
_print_error(const zd::error &err)
{
    std::fputs("Blad: ", stderr);
    zd::message::retrieve((err.origin() << 8) | err.ordinal(), err.size(),
                          err.begin())
        .print(stderr);
    std::fputs("\n", stderr);
}

static void
_print_logo()
{
    std::printf("%s%9s%s\n\n", "Kompilator jezyka Zdzich 6.4", "",
                "(c) 2024 Mateusz Karcz (Licencja MIT)");
}

int
main(int argc, char *argv[])
{
    _print_logo();

    const char *opt_file{nullptr};
    for (int i = 1; i < argc; i++)
    {
        if ('/' != argv[i][0])
        {
            opt_file = argv[i];
        }
    }

    if (!opt_file)
    {
        std::fputs("Nie podano nazwy pliku wejsciowego!\n", stderr);
        return 1;
    }

    const char *path_dot = std::strrchr(opt_file, '.');
    path_dot = path_dot ? path_dot : (opt_file + std::strlen(opt_file));

    zd::ustring out_path{};
    out_path.reserve(path_dot - opt_file + 4);

    for (auto ptr = opt_file; ptr < path_dot; ptr++)
    {
        out_path.append(*ptr);
    }
    out_path.append('.');
    out_path.append('c');
    out_path.append('o');
    out_path.append('m');

    zd::lex::pl_istream    stream{opt_file};
    zd::lex::lexer         lexer{stream};
    zd::gen::zd4_generator generator{};
    zd::unit               unit{lexer, generator};

    auto err = std::move(unit.process());
    if (!err)
    {
        _print_error(err);
        return 1;
    }

    auto output = std::fopen(out_path.data(), "wb");
    if (!output)
    {
        std::perror(out_path.data());
        return 1;
    }

    auto link_ret = generator.link(output);
    if (!link_ret)
    {
        zd::error err = std::move(link_ret.error());
        _print_error(err);
        return 1;
    }

    std::printf("Kompilacja pliku %s do pliku %s\n", opt_file, out_path.data());
    std::printf("Dlugosc programu: %u bajtow\n", generator.get_code_size());
    std::printf("Dlugosc danych:   %u bajtow\n", generator.get_data_size());
    return 0;
}
