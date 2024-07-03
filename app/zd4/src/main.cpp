#include <cstdio>

#include <zd/gen/zd4_generator.hpp>
#include <zd/message.hpp>
#include <zd/unit.hpp>

#include <generated/version.h>

#include "zd4.h"

static void
_print_error(const zd::error &err)
{
    zd::message::retrieve(MSG_ERROR).print(stderr);
    zd::message::retrieve((err.origin() << 8) | err.ordinal(), err.size(),
                          err.begin())
        .print(stderr);
    std::fputs("\n", stderr);
}

static void
_print_logo()
{
    zd4_clear();
    zd::message::retrieve(MSG_HEADER).print(stdout);
    std::printf("%s%9s", " " VER_FILEVERSION_STR, "");
    zd::message::retrieve(MSG_COPYRIGHT).print(stdout);
    std::puts("\n");
}

int
main(int argc, char *argv[])
{
    _print_logo();

    // Process command line arguments
    zd4_arguments args{argc, argv};

    char buff[PATH_MAX];
    if (!args.input)
    {
        zd::message::retrieve(MSG_INPUT_PROMPT).print(stdout);
        std::fgets(buff, PATH_MAX, stdin);
        zd4_clear();

        buff[std::strlen(buff) - 1] = 0;
        if (!buff[0])
        {
            return 1;
        }

        args.input = buff;
    }

    if (0 == std::strcmp("*", args.input))
    {
        return zd4_list_zdi();
    }

    const char *path_dot = std::strrchr(args.input, '.');
    path_dot = path_dot ? path_dot : (args.input + std::strlen(args.input));

    zd::ustring out_path{};
    out_path.reserve(path_dot - args.input + 4);

    for (auto ptr = args.input; ptr < path_dot; ptr++)
    {
        out_path.append(*ptr);
    }
    out_path.append('.');
    out_path.append('c');
    out_path.append('o');
    out_path.append('m');

    zd::lex::pl_istream    stream{args.input};
    zd::lex::lexer         lexer{stream};
    zd::gen::zd4_generator generator{};
    zd::unit               unit{lexer, generator, std::move(args.inc_dirs)};

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

    uintptr_t margv[]{(uintptr_t)args.input, (uintptr_t)out_path.data()};
    zd::message::retrieve(MSG_SUMMARY_INOUT, 2, margv);

    margv[0] = (uintptr_t)generator.get_code_size();
    zd::message::retrieve(MSG_SUMMARY_CODE, 1, margv);

    margv[0] = (uintptr_t)generator.get_data_size();
    zd::message::retrieve(MSG_SUMMARY_DATA, 1, margv);

    return 0;
}
