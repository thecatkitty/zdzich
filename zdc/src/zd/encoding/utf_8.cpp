#include <iostream>

#include <zd/encoding.hpp>

using namespace zd;

static const char LEAD_MASK[]{0, 0x1F, 0x0F, 0x07, 0x03};
static const char CONTINUATION_MASK{0x3F};

struct _utf8_encoding : public multi_byte_encoding
{
    virtual size_t
    decode(const char *buff, int &codepoint) override
    {
        auto length = get_sequence_length(buff);
        if (0 == length)
        {
            return 0;
        }

        if (1 == length)
        {
            codepoint = buff[0];
            return 1;
        }

        codepoint = buff[0] & LEAD_MASK[length];
        for (int i = 1; i < length; i++)
        {
            char byte = buff[i];
            if (0x80 != (byte & 0xC0))
            {
                std::cerr << __FUNCTION__ ": invalid continuation byte " << byte
                          << std::endl;
                return 0;
            }

            codepoint <<= 6;
            codepoint |= (buff[i] & CONTINUATION_MASK);
        }

        return length;
    }

    virtual std::string
    get_name() const override
    {
        return "utf-8";
    }

    virtual size_t
    get_sequence_length(const char *buff) override
    {
        char lead = *buff;

        if (0xC0 != (lead & 0xC0))
        {
            return 1;
        }

        if (0 == (lead & 0x20))
        {
            return 2;
        }

        if (0 == (lead & 0x10))
        {
            return 3;
        }

        if (0 == (lead & 0x08))
        {
            return 4;
        }

        std::cerr << __FUNCTION__ ": invalid lead byte " << lead << std::endl;
        return 0;
    }
};

static _utf8_encoding _encoding{};

multi_byte_encoding *encoding::utf_8 = &_encoding;
