#include <iostream>

#include <zd/encoding.hpp>

using namespace zd;

static const unsigned char LEAD_PREAMBLE[]{0, 0xC0, 0xE0, 0xF0};
static const unsigned char LEAD_MASK[]{0, 0x1F, 0x0F, 0x07, 0x03};
static const unsigned char CONTINUATION_PREAMBLE{0x80};
static const unsigned char CONTINUATION_MASK{0x3F};

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
                std::cerr << __FUNCTION__ << ": invalid continuation byte "
                          << byte << std::endl;
                return 0;
            }

            codepoint <<= 6;
            codepoint |= (buff[i] & CONTINUATION_MASK);
        }

        return length;
    }

    virtual size_t
    encode(char *buff, int codepoint) override
    {
        if ((0 > codepoint) || (0x10FFFF < codepoint))
        {
            return 0;
        }

        if (0x80 > codepoint)
        {
            buff[0] = codepoint;
            return 1;
        }

        size_t continuation_bytes = 0;
        if (0x00007F < codepoint)
        {
            continuation_bytes++;

            if (0x0007FF < codepoint)
            {
                continuation_bytes++;

                if (0x00FFFF < codepoint)
                {
                    continuation_bytes++;
                }
            }
        }

        for (auto i = continuation_bytes; i > 0; i--)
        {
            buff[i] = CONTINUATION_PREAMBLE | (codepoint & CONTINUATION_MASK);
            codepoint >>= 6;
        }

        buff[0] = LEAD_PREAMBLE[continuation_bytes] | codepoint;
        return 1 + continuation_bytes;
    }

    virtual ustring
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

        std::cerr << __FUNCTION__ << ": invalid lead byte " << lead
                  << std::endl;
        return 0;
    }
};

static _utf8_encoding _encoding{};

multi_byte_encoding *encoding::utf_8 = &_encoding;
