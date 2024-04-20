#include <zd/ustring.hpp>

namespace zd
{

// Drop Polish diacritics
int
pl_toascii(int codepoint);

// Check equality of two Polish strings, ignore case
bool
pl_streqi(const ustring &left, const ustring &right);

// Check equality of two Polish strings, ignore diacritics and case
bool
pl_streqai(const ustring &left, const ustring &right);

} // namespace zd
