#pragma once

#include <cctype>

#ifdef isascii
#undef isascii
#endif

namespace zd
{

static inline bool
isascii(int ch)
{
    return (0 <= ch) && (0x80 > ch);
}

bool
isplalpha(int ch);

static inline bool
isalpha(int ch)
{
    return (isascii(ch) && std::isalpha(ch)) || isplalpha(ch);
}

static inline bool
isdigit(int ch)
{
    return isascii(ch) && std::isdigit(ch);
}

static inline bool
isalnum(int ch)
{
    return isalpha(ch) || isdigit(ch);
}

static inline bool
isspace(int ch)
{
    return isascii(ch) && std::isspace(ch);
}

} // namespace zd
