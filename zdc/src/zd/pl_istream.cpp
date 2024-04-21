#include <cstdio>

#ifdef __ia16__
#include <unistd.h>
#endif

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
    auto ch = _stream.getc();
    if (!_stream.good())
    {
        return EOF;
    }

#ifdef __ia16__
    // Detect manually entered Ctrl+Z
    if ((26 == ch) && ::isatty(::fileno(_stream.get())))
    {
        return EOF;
    }
#endif

    _detect_encoding(ch, _stream, _encoding);

    int codepoint{};
    if (encoding::utf_8 != _encoding)
    {
        char byte = ch & 0xFF;
        _encoding->decode(&byte, codepoint);

        if (0 > codepoint)
        {
            std::fprintf(stderr,
                         "%s: unexpected byte %u in the encoding '%s'\n",
                         __FUNCTION__, ch, _encoding->get_name().data());
            return 0;
        }

        return codepoint;
    }

    // UTF-8
    char buffer[4]{static_cast<char>(ch)};

    auto length = encoding::utf_8->get_sequence_length(buffer);
    if (0 == length)
    {
        std::fprintf(stderr, "%s: invalid UTF-8 sequence\n", __FUNCTION__);
        return -1;
    }

    if ((length - 1) != _stream.read(buffer + 1, length - 1))
    {
        std::fprintf(stderr, "%s: read error in a UTF-8 sequence\n",
                     __FUNCTION__);
        return -1;
    }

    if (0 > _encoding->decode(buffer, codepoint))
    {
        std::fprintf(stderr, "%s: invalid UTF-8 sequence\n", __FUNCTION__);
        return -1;
    }

    return codepoint;
}
