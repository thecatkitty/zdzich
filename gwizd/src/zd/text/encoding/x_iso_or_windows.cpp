#include <zd/text/encoding.hpp>

static const int X_ISO_OR_WINDOWS[]{
    /* 0x80 */ -1,
    /* 0x81 */ -1,
    /* 0x82 */ -1,
    /* 0x83 */ -1,
    /* 0x84 */ -1,
    /* 0x85 */ -1,
    /* 0x86 */ -1,
    /* 0x87 */ -1,
    /* 0x88 */ -1,
    /* 0x89 */ -1,
    /* 0x8A */ -1,
    /* 0x8B */ -1,
    /* 0x8C */ -1,
    /* 0x8D */ -1,
    /* 0x8E */ -1,
    /* 0x8F */ -1,
    /* 0x90 */ -1,
    /* 0x91 */ -1,
    /* 0x92 */ -1,
    /* 0x93 */ -1,
    /* 0x94 */ -1,
    /* 0x95 */ -1,
    /* 0x96 */ -1,
    /* 0x97 */ -1,
    /* 0x98 */ -1,
    /* 0x99 */ -1,
    /* 0x9A */ -1,
    /* 0x9B */ -1,
    /* 0x9C */ -1,
    /* 0x9D */ -1,
    /* 0x9E */ -1,
    /* 0x9F */ -1,
    /* 0xA0 */ 0x00A0, // NO-BREAK SPACE
    /* 0xA1 */ -1,
    /* 0xA2 */ 0x02D8, // BREVE
    /* 0xA3 */ 0x0141, // LATIN CAPITAL LETTER L WITH STROKE
    /* 0xA4 */ 0x00A4, // CURRENCY SIGN
    /* 0xA5 */ -1,
    /* 0xA6 */ -1,
    /* 0xA7 */ 0x00A7, // SECTION SIGN
    /* 0xA8 */ 0x00A8, // DIAERESIS
    /* 0xA9 */ -1,
    /* 0xAA */ 0x015E, // LATIN CAPITAL LETTER S WITH CEDILLA
    /* 0xAB */ -1,
    /* 0xAC */ -1,
    /* 0xAD */ 0x00AD, // SOFT HYPHEN
    /* 0xAE */ -1,
    /* 0xAF */ 0x017B, // LATIN CAPITAL LETTER Z WITH DOT ABOVE
    /* 0xB0 */ 0x00B0, // DEGREE SIGN
    /* 0xB1 */ -1,
    /* 0xB2 */ 0x02DB, // OGONEK
    /* 0xB3 */ 0x0142, // LATIN SMALL LETTER L WITH STROKE
    /* 0xB4 */ 0x00B4, // ACUTE ACCENT
    /* 0xB5 */ -1,
    /* 0xB6 */ -1,
    /* 0xB7 */ -1,
    /* 0xB8 */ 0x00B8, // CEDILLA
    /* 0xB9 */ -1,
    /* 0xBA */ 0x015F, // LATIN SMALL LETTER S WITH CEDILLA
    /* 0xBB */ -1,
    /* 0xBC */ -1,
    /* 0xBD */ 0x02DD, // DOUBLE ACUTE ACCENT
    /* 0xBE */ -1,
    /* 0xBF */ 0x017C, // LATIN SMALL LETTER Z WITH DOT ABOVE
    /* 0xC0 */ 0x0154, // LATIN CAPITAL LETTER R WITH ACUTE
    /* 0xC1 */ 0x00C1, // LATIN CAPITAL LETTER A WITH ACUTE
    /* 0xC2 */ 0x00C2, // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    /* 0xC3 */ 0x0102, // LATIN CAPITAL LETTER A WITH BREVE
    /* 0xC4 */ 0x00C4, // LATIN CAPITAL LETTER A WITH DIAERESIS
    /* 0xC5 */ 0x0139, // LATIN CAPITAL LETTER L WITH ACUTE
    /* 0xC6 */ 0x0106, // LATIN CAPITAL LETTER C WITH ACUTE
    /* 0xC7 */ 0x00C7, // LATIN CAPITAL LETTER C WITH CEDILLA
    /* 0xC8 */ 0x010C, // LATIN CAPITAL LETTER C WITH CARON
    /* 0xC9 */ 0x00C9, // LATIN CAPITAL LETTER E WITH ACUTE
    /* 0xCA */ 0x0118, // LATIN CAPITAL LETTER E WITH OGONEK
    /* 0xCB */ 0x00CB, // LATIN CAPITAL LETTER E WITH DIAERESIS
    /* 0xCC */ 0x011A, // LATIN CAPITAL LETTER E WITH CARON
    /* 0xCD */ 0x00CD, // LATIN CAPITAL LETTER I WITH ACUTE
    /* 0xCE */ 0x00CE, // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    /* 0xCF */ 0x010E, // LATIN CAPITAL LETTER D WITH CARON
    /* 0xD0 */ 0x0110, // LATIN CAPITAL LETTER D WITH STROKE
    /* 0xD1 */ 0x0143, // LATIN CAPITAL LETTER N WITH ACUTE
    /* 0xD2 */ 0x0147, // LATIN CAPITAL LETTER N WITH CARON
    /* 0xD3 */ 0x00D3, // LATIN CAPITAL LETTER O WITH ACUTE
    /* 0xD4 */ 0x00D4, // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    /* 0xD5 */ 0x0150, // LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
    /* 0xD6 */ 0x00D6, // LATIN CAPITAL LETTER O WITH DIAERESIS
    /* 0xD7 */ 0x00D7, // MULTIPLICATION SIGN
    /* 0xD8 */ 0x0158, // LATIN CAPITAL LETTER R WITH CARON
    /* 0xD9 */ 0x016E, // LATIN CAPITAL LETTER U WITH RING ABOVE
    /* 0xDA */ 0x00DA, // LATIN CAPITAL LETTER U WITH ACUTE
    /* 0xDB */ 0x0170, // LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
    /* 0xDC */ 0x00DC, // LATIN CAPITAL LETTER U WITH DIAERESIS
    /* 0xDD */ 0x00DD, // LATIN CAPITAL LETTER Y WITH ACUTE
    /* 0xDE */ 0x0162, // LATIN CAPITAL LETTER T WITH CEDILLA
    /* 0xDF */ 0x00DF, // LATIN SMALL LETTER SHARP S
    /* 0xE0 */ 0x0155, // LATIN SMALL LETTER R WITH ACUTE
    /* 0xE1 */ 0x00E1, // LATIN SMALL LETTER A WITH ACUTE
    /* 0xE2 */ 0x00E2, // LATIN SMALL LETTER A WITH CIRCUMFLEX
    /* 0xE3 */ 0x0103, // LATIN SMALL LETTER A WITH BREVE
    /* 0xE4 */ 0x00E4, // LATIN SMALL LETTER A WITH DIAERESIS
    /* 0xE5 */ 0x013A, // LATIN SMALL LETTER L WITH ACUTE
    /* 0xE6 */ 0x0107, // LATIN SMALL LETTER C WITH ACUTE
    /* 0xE7 */ 0x00E7, // LATIN SMALL LETTER C WITH CEDILLA
    /* 0xE8 */ 0x010D, // LATIN SMALL LETTER C WITH CARON
    /* 0xE9 */ 0x00E9, // LATIN SMALL LETTER E WITH ACUTE
    /* 0xEA */ 0x0119, // LATIN SMALL LETTER E WITH OGONEK
    /* 0xEB */ 0x00EB, // LATIN SMALL LETTER E WITH DIAERESIS
    /* 0xEC */ 0x011B, // LATIN SMALL LETTER E WITH CARON
    /* 0xED */ 0x00ED, // LATIN SMALL LETTER I WITH ACUTE
    /* 0xEE */ 0x00EE, // LATIN SMALL LETTER I WITH CIRCUMFLEX
    /* 0xEF */ 0x010F, // LATIN SMALL LETTER D WITH CARON
    /* 0xF0 */ 0x0111, // LATIN SMALL LETTER D WITH STROKE
    /* 0xF1 */ 0x0144, // LATIN SMALL LETTER N WITH ACUTE
    /* 0xF2 */ 0x0148, // LATIN SMALL LETTER N WITH CARON
    /* 0xF3 */ 0x00F3, // LATIN SMALL LETTER O WITH ACUTE
    /* 0xF4 */ 0x00F4, // LATIN SMALL LETTER O WITH CIRCUMFLEX
    /* 0xF5 */ 0x0151, // LATIN SMALL LETTER O WITH DOUBLE ACUTE
    /* 0xF6 */ 0x00F6, // LATIN SMALL LETTER O WITH DIAERESIS
    /* 0xF7 */ 0x00F7, // DIVISION SIGN
    /* 0xF8 */ 0x0159, // LATIN SMALL LETTER R WITH CARON
    /* 0xF9 */ 0x016F, // LATIN SMALL LETTER U WITH RING ABOVE
    /* 0xFA */ 0x00FA, // LATIN SMALL LETTER U WITH ACUTE
    /* 0xFB */ 0x0171, // LATIN SMALL LETTER U WITH DOUBLE ACUTE
    /* 0xFC */ 0x00FC, // LATIN SMALL LETTER U WITH DIAERESIS
    /* 0xFD */ 0x00FD, // LATIN SMALL LETTER Y WITH ACUTE
    /* 0xFE */ 0x0163, // LATIN SMALL LETTER T WITH CEDILLA
    /* 0xFF */ 0x02D9, // DOT ABOVE
};

static zd::text::single_byte_encoding _encoding{"x-iso-or-windows",
                                                X_ISO_OR_WINDOWS};

zd::text::single_byte_encoding *zd::text::encoding::x_iso_or_windows =
    &_encoding;
