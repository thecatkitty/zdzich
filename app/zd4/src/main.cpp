#include <cstdio>

#include <zd/gen/zd4_generator.hpp>
#include <zd/message.hpp>
#include <zd/unit.hpp>

#include "zd4.h"

static void
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
    zd4_clear();
    std::printf("%s%9s%s\n\n", "Kompilator jezyka Zdzich 6.4", "",
                "(c) 2024 Mateusz Karcz (Licencja MIT)");
}

int
main(int argc, char *argv[])
{
    _print_logo();

    // Process command line arguments
    const char *opt_file{nullptr};

    std::list<zd::ustring> inc_dirs{};

    for (int i = 1; i < argc; i++)
    {
        if ('/' != argv[i][0])
        {
            opt_file = argv[i];
            continue;
        }

        if ('I' == argv[i][1])
        {
            inc_dirs.push_back(argv[i] + 2);
            continue;
        }
    }

    char buff[PATH_MAX];
    if (!opt_file)
    {
        std::printf("Podaj nazwe pliku do kompilacji: ");
        std::fgets(buff, PATH_MAX, stdin);
        zd4_clear();

        buff[std::strlen(buff) - 1] = 0;
        if (!buff[0])
        {
            return 1;
        }

        opt_file = buff;
    }

    if (0 == std::strcmp("*", opt_file))
    {
        return zd4_list_zdi();
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
    zd::unit               unit{lexer, generator, std::move(inc_dirs)};

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
