#include <zd/containers.hpp>
#include <zd/encoding.hpp>

using namespace zd;

struct _unknown_encoding : public encoding
{
    virtual size_t
    decode(const char *buff, int &codepoint) override
    {
        auto ch = *reinterpret_cast<const uint8_t *>(buff);
        codepoint = (ch < 0x80) ? ch : -1;
        return 1;
    }

    virtual size_t
    encode(char *, int) override
    {
        return 0;
    }

    virtual ustring
    get_name() const override
    {
        return "unknown";
    }
};

static _unknown_encoding _encoding{};

encoding *encoding::unknown = &_encoding;

size_t
single_byte_encoding::decode(const char *buff, int &codepoint)
{
    auto ch = *reinterpret_cast<const uint8_t *>(buff);
    codepoint = (ch < 0x80) ? ch : _mapping[ch - 0x80];
    return 1;
}

size_t
single_byte_encoding::encode(char *buff, int codepoint)
{
    if (0x80 > codepoint)
    {
        *buff = codepoint & 0xFF;
        return 1;
    }

    auto position = std::find(_mapping, _mapping + 0x80, codepoint);
    if ((_mapping + 0x80) == position)
    {
        return 0;
    }

    *buff = 0x80 + (position - _mapping);
    return 1;
}
