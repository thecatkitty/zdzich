#pragma once

#include <zd.hpp>

#ifdef isascii
#undef isascii
#endif

namespace zd
{

namespace text
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
isxdigit(int ch)
{
    return isascii(ch) && std::isxdigit(ch);
}

static inline bool
isalnum(int ch)
{
    return isalpha(ch) || isdigit(ch);
}

static inline bool
isspace(int ch)
{
    return (' ' == ch) || ('\t' == ch);
}

static inline bool
is_name_start(int ch)
{
    return isalpha(ch) || ('_' == ch);
}

static inline bool
is_name_continuation(int ch)
{
    return is_name_start(ch) || isdigit(ch) || ('?' == ch);
}

} // namespace text

} // namespace zd
