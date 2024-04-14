#include <string>

namespace zd
{

// Drop Polish diacritics
int
pl_toascii(int codepoint);

// Check equality of two Polish strings, ignore diacritics and case
bool
pl_streqai(const std::string &left, const std::string &right);

} // namespace zd
