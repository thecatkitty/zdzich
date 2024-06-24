#include <clocale>
#include <cstdio>

#include <zd/message.hpp>

#if defined(__ia16__) && __MISC_VISIBLE
#define ASPRINTF asiprintf
#else
#define ASPRINTF asprintf
#endif

#define ARGV1(argv) (argv)[0]
#define ARGV2(argv) ARGV1(argv), (argv)[1]
#define ARGV3(argv) ARGV2(argv), (argv)[2]

#define CASE_PRINT_ARGS(num, ptr, fmt, argv, ret)                              \
    case num:                                                                  \
        (ret) = ::ASPRINTF((ptr), (fmt), ARGV##num(argv));                     \
        break;

struct _message
{
    unsigned    id;
    const char *msg;
};

extern "C" const _message MESSAGES_0409[];
extern "C" const _message MESSAGES_0415[];

static const _message *MESSAGES{nullptr};

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

#ifdef __ia16__
struct _dos_country_data
{
    uint16_t date_format;
    char     currency[5];
    char     thousands_separator[2];
    char     decimal_separator[2];
    char     date_separator[2];
    char     time_separator[2];
    uint8_t  currency_style;
    uint8_t  fractional_digits;
    uint8_t  time_format;
    uint32_t case_map_call;
    char     list_separator[2];
    char     _reserved[10];
};

static int
_dos_get_country_data(_dos_country_data &out)
{
    uint16_t ax, bx, dx;
    asm volatile("int $0x21; pushf; pop %%dx"
                 : "=a"(ax), "=b"(bx), "=d"(dx)
                 : "a"(0x3800), "d"(&out));
    return (dx & 1) ? -ax : bx;
}

static const char *
_get_dos_locale()
{
    _dos_country_data cdata{};

    auto ccode = _dos_get_country_data(cdata);
    switch (ccode)
    {
    case 48:
        return "pl";
    }

    return "en";
}
#endif

static const char *
_retrieve_fmt(uint16_t id)
{
    if (nullptr == MESSAGES)
    {
        const char *loc = std::setlocale(LC_ALL, "");
#ifdef __ia16__
        if (0 == std::strcmp("C", loc))
        {
            loc = _get_dos_locale();
        }
#endif
        auto lang_len = std::strcspn(loc, ".-_");
        if (0 == std::memcmp("pl", loc, lang_len))
        {
            MESSAGES = MESSAGES_0415;
        }
        else
        {
            MESSAGES = MESSAGES_0409;
        }
    }

    for (auto it = MESSAGES; it->id; it++)
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
        CASE_PRINT_ARGS(3, &ptr, fmt, argv, length);
    default:
        length = ::ASPRINTF(&ptr, "%s", fmt);
    }

    return {ptr, length};
}
