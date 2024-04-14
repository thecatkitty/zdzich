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

    virtual std::string
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
