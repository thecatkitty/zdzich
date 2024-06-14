#include <zd/text/characters.hpp>
#include <zd/text/encoding.hpp>
#include <zd/text/pl_string.hpp>

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
zd::text::isplalpha(int codepoint)
{
    return pl_toascii(codepoint) != codepoint;
}

int
zd::text::pl_toascii(int codepoint)
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
    auto length = zd::text::encoding::utf_8->decode(ptr, codepoint);
    if (0 == length)
    {
        return false;
    }

    ptr += length;
    return true;
}

bool
zd::text::pl_streqi(const ustring &left, const ustring &right)
{
    return pl_streqi(left.data(), right.data());
}

bool
zd::text::pl_streqi(const char *left, const char *right)
{
    while (*left && *right)
    {
        if (isascii(*left) && isascii(*right))
                          {
            if (std::toupper(*left) == std::toupper(*right))
            {
                return false;
                          }
        }
        else if (*left != *right)
        {
            return false;
        }

        left++;
        right++;
    }

    if ((0 != *left) || (0 != *right))
    {
    return false;
    }

    return true;
}

bool
zd::text::pl_streqai(const ustring &left, const ustring &right)
{
    return pl_streqai(left.data(), right.data());
}

bool
zd::text::pl_streqai(const char *left, const char *right)
{
    while (*left && *right)
    {
        int codepoint_left, codepoint_right;

        if (!_load_codepoint(left, codepoint_left))
        {
            return false;
        }

        if (!_load_codepoint(right, codepoint_right))
        {
            return false;
        }

        if (std::toupper(pl_toascii(codepoint_left)) !=
            std::toupper(pl_toascii(codepoint_right)))
        {
            return false;
        }
    }

    if ((0 != *left) || (0 != *right))
    {
        return false;
    }

    return true;
}
