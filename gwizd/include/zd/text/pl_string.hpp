#pragma once

#include <zd.hpp>

namespace zd
{

namespace text
{

// Drop Polish diacritics
int
pl_toascii(int codepoint);

// Check equality of two Polish strings, ignore case
bool
pl_streqi(const ustring &left, const ustring &right);

// Check equality of two Polish C strings, ignore case
bool
pl_streqi(const char *left, const char *right);

// Check equality of two Polish strings, ignore diacritics and case
bool
pl_streqai(const ustring &left, const ustring &right);

// Check equality of two Polish C strings, ignore diacritics and case
bool
pl_streqai(const char *left, const char *right);

} // namespace text

} // namespace zd
