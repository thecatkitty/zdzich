#include <iostream>

#include <zd/containers.hpp>
#include <zd/pl_istream.hpp>

using namespace zd;

static const int MARKERS_IBM852[]{0xA4, 0xA8, 0x9D, 0xE3, 0xE0, 0x97,
                                  0x8D, 0xBD, 0x86, 0xA9, 0x88, 0xE4,
                                  0xA2, 0x98, 0xAB, 0xBE};

static const int MARKERS_WINDOWS_1250[]{0x8C, 0xB9, 0x9C, 0x9F};

static const int MARKERS_ISO_8859_2[]{0xA1, 0xA6, 0xAC, 0xB1, 0xB6, 0xBC};

// Codes for Polish letters present in both Windows and ISO encodings
static const int MARKERS_X_ISO_OR_WINDOWS[]{0xA3, 0xAF, 0xB3, 0xBF, 0xC6, 0xCA,
                                            0xD1, 0xD3, 0xE6, 0xEA, 0xF1, 0xF3};

static encoding *
_detect_encoding(int ch, min_istream &stream, encoding *&enc)
{
    if ((encoding::unknown != enc) && (encoding::x_iso_or_windows != enc))
    {
        return enc;
    }

    if (encoding::x_iso_or_windows == enc)
    {
        if (contains(MARKERS_WINDOWS_1250, ch))
        {
            return enc = encoding::windows_1250;
        }

        if (contains(MARKERS_ISO_8859_2, ch))
        {
            return enc = encoding::iso_8859_2;
        }

        // Hard to tell
        return enc;
    }

    // encoding::unknown
    if (0xC0 > ch)
    {
        // We can't have a UTF-8 continuation byte outside of sequence
        if (contains(MARKERS_IBM852, ch))
        {
            return enc = encoding::ibm852;
        }

        if (contains(MARKERS_WINDOWS_1250, ch))
        {
            return enc = encoding::windows_1250;
        }

        if (contains(MARKERS_ISO_8859_2, ch))
        {
            return enc = encoding::iso_8859_2;
        }

        if (contains(MARKERS_X_ISO_OR_WINDOWS, ch))
        {
            return enc = encoding::x_iso_or_windows;
        }

        // Hard to tell
        return enc;
    }

    // It's either a UTF-8 leading byte, or ISO/Windows Latin-2
    auto continuation = stream.getc();
    std::ungetc(continuation, stream.get());

    if (0x80 != (continuation & 0xC0))
    {
        // It is certainly not a UTF-8 sequence
        if (contains(MARKERS_X_ISO_OR_WINDOWS, ch))
        {
            return enc = encoding::x_iso_or_windows;
        }

        // Hard to tell
        return enc;
    }

    // We can assume UTF-8 at this point
    return enc = encoding::utf_8;
}

int
pl_istream::read() noexcept
{
    int  codepoint{};
    auto ch = _stream.getc();
    if (!_stream.good())
    {
        return EOF;
    }

    _detect_encoding(ch, _stream, _encoding);
    if (encoding::utf_8 != _encoding)
    {
        char byte = ch & 0xFF;
        _encoding->decode(&byte, codepoint);

        if (0 > codepoint)
        {
            std::cerr << __FUNCTION__ << ": unexpected byte " << ch
                      << " in the encoding '" << _encoding->get_name() << "'"
                      << std::endl;
            return 0;
        }

        return codepoint;
    }

    // UTF-8
    char buffer[4]{static_cast<char>(ch)};

    auto length = encoding::utf_8->get_sequence_length(buffer);
    if (0 == length)
    {
        std::cerr << __FUNCTION__ << ": invalid UTF-8 sequence" << std::endl;
        return -1;
    }

    if ((length - 1) != _stream.read(buffer + 1, length - 1))
    {
        std::cerr << __FUNCTION__ << ": read error in a UTF-8 sequence"
                  << std::endl;
        return -1;
    }

    if (0 > _encoding->decode(buffer, codepoint))
    {
        std::cerr << __FUNCTION__ << ": invalid UTF-8 sequence" << std::endl;
        return -1;
    }

    return codepoint;
}