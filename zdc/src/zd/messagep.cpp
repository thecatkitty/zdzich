#include <cstdio>
#include <cstdlib>

#include <zd/message.hpp>

#if defined(__ia16__) && __MISC_VISIBLE
#define ASPRINTF asiprintf
#else
#define ASPRINTF asprintf
#endif

#define ARGV1(argv) (argv)[0]
#define ARGV2(argv) ARGV1(argv), (argv)[1]

#define CASE_PRINT_ARGS(num, ptr, fmt, argv, ret)                              \
    case num:                                                                  \
        (ret) = ::ASPRINTF((ptr), (fmt), ARGV##num(argv));                     \
        break;

extern "C" const struct
{
    unsigned    id;
    const char *msg;
} MESSAGES_0409[];

using namespace zd;

message::~message()
{
    if (_ptr)
    {
        ::free(_ptr);
        _ptr = nullptr;
        _len = 0;
    }
}

void
message::print(std::FILE *pf) const
{
    std::fputs(reinterpret_cast<const char *>(_ptr), pf);
}

static const char *
_retrieve_fmt(uint16_t id)
{
    for (auto it = MESSAGES_0409; it->id; it++)
    {
        if (it->id == id)
        {
            return it->msg;
        }
    }

    return nullptr;
}

message
message::retrieve(uint16_t ordinal, size_t argc, const uintptr_t *argv)
{
    auto fmt = _retrieve_fmt(ordinal);
    if (!fmt)
    {
        return {nullptr, 0};
    }

    char  *ptr{nullptr};
    size_t length{0};

    switch (argc)
    {
        CASE_PRINT_ARGS(1, &ptr, fmt, argv, length);
        CASE_PRINT_ARGS(2, &ptr, fmt, argv, length);
    default:
        length = ::ASPRINTF(&ptr, "%s", fmt);
    }

    return {ptr, length};
}
