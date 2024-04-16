#include <zd/characters.hpp>
#include <zd/encoding.hpp>
#include <zd/pl_string.hpp>

static const struct _code_mapping
{
    int  codepoint;
    char ascii;
} POLISH_LETTERS[]{
    {0x00D3, 'O'}, {0x00F3, 'o'}, {0x0104, 'A'}, {0x0105, 'a'}, {0x0106, 'C'},
    {0x0107, 'c'}, {0x0118, 'E'}, {0x0119, 'e'}, {0x0141, 'L'}, {0x0142, 'l'},
    {0x0143, 'N'}, {0x0144, 'n'}, {0x015A, 'S'}, {0x015B, 's'}, {0x0179, 'Z'},
    {0x017A, 'z'}, {0x017B, 'Z'}, {0x017C, 'z'},
};

bool
zd::isplalpha(int codepoint)
{
    return pl_toascii(codepoint) != codepoint;
}

int
zd::pl_toascii(int codepoint)
{
    if (0x80 > codepoint)
    {
        return codepoint;
    }

    for (auto &mapping : POLISH_LETTERS)
    {
        if (mapping.codepoint > codepoint)
        {
            break;
        }

        if (mapping.codepoint == codepoint)
        {
            return mapping.ascii;
        }
    }

    return codepoint;
}

static bool
_load_codepoint(const char *&ptr, int &codepoint)
{
    auto length = zd::encoding::utf_8->decode(ptr, codepoint);
    if (0 == length)
    {
        return false;
    }

    ptr += length;
    return true;
}

bool
zd::pl_streqi(const std::string &left, const std::string &right)
{
    return std::equal(left.begin(), left.end(), right.begin(), right.end(),
                      [](char a, char b) -> bool {
                          if (isascii(a) && isascii(b))
                          {
                              return std::toupper(a) == std::toupper(b);
                          }

                          return a == b;
                      });
    return false;
}

bool
zd::pl_streqai(const std::string &left, const std::string &right)
{
    auto ptr_left = left.c_str();
    auto ptr_right = right.c_str();

    while (*ptr_left && *ptr_right)
    {
        int codepoint_left, codepoint_right;

        if (!_load_codepoint(ptr_left, codepoint_left))
        {
            return false;
        }

        if (!_load_codepoint(ptr_right, codepoint_right))
        {
            return false;
        }

        if (std::toupper(pl_toascii(codepoint_left)) !=
            std::toupper(pl_toascii(codepoint_right)))
        {
            return false;
        }
    }

    if ((0 != *ptr_left) || (0 != *ptr_right))
    {
        return false;
    }

    return true;
}
