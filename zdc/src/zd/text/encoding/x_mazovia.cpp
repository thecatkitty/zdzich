#include <zd/text/encoding.hpp>

static const int X_MAZOVIA[]{
    /* 0x80 */ 0x00C7, // LATIN CAPITAL LETTER C WITH CEDILLA
    /* 0x81 */ 0x00FC, // LATIN SMALL LETTER U WITH DIAERESIS
    /* 0x82 */ 0x00E9, // LATIN SMALL LETTER E WITH ACUTE
    /* 0x83 */ 0x00E2, // LATIN SMALL LETTER A WITH CIRCUMFLEX
    /* 0x84 */ 0x00E4, // LATIN SMALL LETTER A WITH DIAERESIS
    /* 0x85 */ 0x00E0, // LATIN SMALL LETTER A WITH GRAVE
    /* 0x86 */ 0x0105, // LATIN SMALL LETTER A WITH OGONEK
    /* 0x87 */ 0x00E7, // LATIN SMALL LETTER C WITH CEDILLA
    /* 0x88 */ 0x00EA, // LATIN SMALL LETTER E WITH CIRCUMFLEX
    /* 0x89 */ 0x00EB, // LATIN SMALL LETTER E WITH DIAERESIS
    /* 0x8A */ 0x00E8, // LATIN SMALL LETTER E WITH GRAVE
    /* 0x8B */ 0x00EF, // LATIN SMALL LETTER I WITH DIAERESIS
    /* 0x8C */ 0x00EE, // LATIN SMALL LETTER I WITH CIRCUMFLEX
    /* 0x8D */ 0x0107, // LATIN SMALL LETTER C WITH ACUTE
    /* 0x8E */ 0x00C4, // LATIN CAPITAL LETTER A WITH DIAERESIS
    /* 0x8F */ 0x0104, // LATIN CAPITAL LETTER A WITH OGONEK
    /* 0x90 */ 0x0118, // LATIN CAPITAL LETTER E WITH OGONEK
    /* 0x91 */ 0x0119, // LATIN SMALL LETTER E WITH OGONEK
    /* 0x92 */ 0x0142, // LATIN SMALL LETTER L WITH STROKE
    /* 0x93 */ 0x00F4, // LATIN SMALL LETTER O WITH CIRCUMFLEX
    /* 0x94 */ 0x00F6, // LATIN SMALL LETTER O WITH DIAERESIS
    /* 0x95 */ 0x0106, // LATIN CAPITAL LETTER C WITH ACUTE
    /* 0x96 */ 0x00FB, // LATIN SMALL LETTER U WITH CIRCUMFLEX
    /* 0x97 */ 0x00F9, // LATIN SMALL LETTER U WITH GRAVE
    /* 0x98 */ 0x015A, // LATIN CAPITAL LETTER S WITH ACUTE
    /* 0x99 */ 0x00D6, // LATIN CAPITAL LETTER O WITH DIAERESIS
    /* 0x9A */ 0x00DC, // LATIN CAPITAL LETTER U WITH DIAERESIS
    /* 0x9B */ 0x00A2, // CENT SIGN
    /* 0x9C */ 0x0141, // LATIN CAPITAL LETTER L WITH STROKE
    /* 0x9D */ 0x00A5, // YEN SIGN
    /* 0x9E */ 0x015B, // LATIN SMALL LETTER S WITH ACUTE
    /* 0x9F */ 0x0192, // LATIN SMALL LETTER F WITH HOOK
    /* 0xA0 */ 0x0179, // LATIN CAPITAL LETTER Z WITH ACUTE
    /* 0xA1 */ 0x017B, // LATIN CAPITAL LETTER Z WITH DOT ABOVE
    /* 0xA2 */ 0x00F3, // LATIN SMALL LETTER O WITH ACUTE
    /* 0xA3 */ 0x00D3, // LATIN CAPITAL LETTER O WITH ACUTE
    /* 0xA4 */ 0x0144, // LATIN SMALL LETTER N WITH ACUTE
    /* 0xA5 */ 0x0143, // LATIN CAPITAL LETTER N WITH ACUTE
    /* 0xA6 */ 0x017A, // LATIN SMALL LETTER Z WITH ACUTE
    /* 0xA7 */ 0x017C, // LATIN SMALL LETTER Z WITH DOT ABOVE
    /* 0xA8 */ 0x00BF, // INVERTED QUESTION MARK
    /* 0xA9 */ 0x2310, // REVERSED NOT SIGN
    /* 0xAA */ 0x00AC, // NOT SIGN
    /* 0xAB */ 0x00BD, // VULGAR FRACTION ONE HALF
    /* 0xAC */ 0x00BC, // VULGAR FRACTION ONE QUARTER
    /* 0xAD */ 0x00A1, // INVERTED EXCLAMATION MARK
    /* 0xAE */ 0x00AB, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    /* 0xAF */ 0x00BB, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    /* 0xB0 */ 0x2591, // LIGHT SHADE
    /* 0xB1 */ 0x2592, // MEDIUM SHADE
    /* 0xB2 */ 0x2593, // DARK SHADE
    /* 0xB3 */ 0x2502, // BOX DRAWINGS LIGHT VERTICAL
    /* 0xB4 */ 0x2524, // BOX DRAWINGS LIGHT VERTICAL AND LEFT
    /* 0xB5 */ 0x2561, // BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE
    /* 0xB6 */ 0x2562, // BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE
    /* 0xB7 */ 0x2556, // BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE
    /* 0xB8 */ 0x2555, // BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE
    /* 0xB9 */ 0x2563, // BOX DRAWINGS DOUBLE VERTICAL AND LEFT
    /* 0xBA */ 0x2551, // BOX DRAWINGS DOUBLE VERTICAL
    /* 0xBB */ 0x2557, // BOX DRAWINGS DOUBLE DOWN AND LEFT
    /* 0xBC */ 0x255D, // BOX DRAWINGS DOUBLE UP AND LEFT
    /* 0xBD */ 0x255C, // BOX DRAWINGS UP DOUBLE AND LEFT SINGLE
    /* 0xBE */ 0x255B, // BOX DRAWINGS UP SINGLE AND LEFT DOUBLE
    /* 0xBF */ 0x2510, // BOX DRAWINGS LIGHT DOWN AND LEFT
    /* 0xC0 */ 0x2514, // BOX DRAWINGS LIGHT UP AND RIGHT
    /* 0xC1 */ 0x2534, // BOX DRAWINGS LIGHT UP AND HORIZONTAL
    /* 0xC2 */ 0x252C, // BOX DRAWINGS LIGHT DOWN AND HORIZONTAL
    /* 0xC3 */ 0x251C, // BOX DRAWINGS LIGHT VERTICAL AND RIGHT
    /* 0xC4 */ 0x2500, // BOX DRAWINGS LIGHT HORIZONTAL
    /* 0xC5 */ 0x253C, // BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL
    /* 0xC6 */ 0x255E, // BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE
    /* 0xC7 */ 0x255F, // BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE
    /* 0xC8 */ 0x255A, // BOX DRAWINGS DOUBLE UP AND RIGHT
    /* 0xC9 */ 0x2554, // BOX DRAWINGS DOUBLE DOWN AND RIGHT
    /* 0xCA */ 0x2569, // BOX DRAWINGS DOUBLE UP AND HORIZONTAL
    /* 0xCB */ 0x2566, // BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL
    /* 0xCC */ 0x2560, // BOX DRAWINGS DOUBLE VERTICAL AND RIGHT
    /* 0xCD */ 0x2550, // BOX DRAWINGS DOUBLE HORIZONTAL
    /* 0xCE */ 0x256C, // BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL
    /* 0xCF */ 0x2567, // BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE
    /* 0xD0 */ 0x2568, // BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE
    /* 0xD1 */ 0x2564, // BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE
    /* 0xD2 */ 0x2565, // BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE
    /* 0xD3 */ 0x2559, // BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE
    /* 0xD4 */ 0x2558, // BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE
    /* 0xD5 */ 0x2552, // BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE
    /* 0xD6 */ 0x2553, // BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE
    /* 0xD7 */ 0x256B, // BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE
    /* 0xD8 */ 0x256A, // BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE
    /* 0xD9 */ 0x2518, // BOX DRAWINGS LIGHT UP AND LEFT
    /* 0xDA */ 0x250C, // BOX DRAWINGS LIGHT DOWN AND RIGHT
    /* 0xDB */ 0x2588, // FULL BLOCK
    /* 0xDC */ 0x2584, // LOWER HALF BLOCK
    /* 0xDD */ 0x258C, // LEFT HALF BLOCK
    /* 0xDE */ 0x2590, // RIGHT HALF BLOCK
    /* 0xDF */ 0x2580, // UPPER HALF BLOCK
    /* 0xE0 */ 0x03B1, // GREEK SMALL LETTER ALPHA
    /* 0xE1 */ 0x00DF, // LATIN SMALL LETTER SHARP S
    /* 0xE2 */ 0x0393, // GREEK CAPITAL LETTER GAMMA
    /* 0xE3 */ 0x03C0, // GREEK SMALL LETTER PI
    /* 0xE4 */ 0x03A3, // GREEK CAPITAL LETTER SIGMA
    /* 0xE5 */ 0x03C3, // GREEK SMALL LETTER SIGMA
    /* 0xE6 */ 0x00B5, // MICRO SIGN
    /* 0xE7 */ 0x03C4, // GREEK SMALL LETTER TAU
    /* 0xE8 */ 0x03A6, // GREEK CAPITAL LETTER PHI
    /* 0xE9 */ 0x0398, // GREEK CAPITAL LETTER THETA
    /* 0xEA */ 0x03A9, // GREEK CAPITAL LETTER OMEGA
    /* 0xEB */ 0x03B4, // GREEK SMALL LETTER DELTA
    /* 0xEC */ 0x221E, // INFINITY
    /* 0xED */ 0x03C6, // GREEK SMALL LETTER PHI
    /* 0xEE */ 0x03B5, // GREEK SMALL LETTER EPSILON
    /* 0xEF */ 0x2229, // INTERSECTION
    /* 0xF0 */ 0x2261, // IDENTICAL TO
    /* 0xF1 */ 0x00B1, // PLUS-MINUS SIGN
    /* 0xF2 */ 0x2265, // GREATER-THAN OR EQUAL TO
    /* 0xF3 */ 0x2264, // LESS-THAN OR EQUAL TO
    /* 0xF4 */ 0x2320, // TOP HALF INTEGRAL
    /* 0xF5 */ 0x2321, // BOTTOM HALF INTEGRAL
    /* 0xF6 */ 0x00F7, // DIVISION SIGN
    /* 0xF7 */ 0x2248, // ALMOST EQUAL TO
    /* 0xF8 */ 0x00B0, // DEGREE SIGN
    /* 0xF9 */ 0x2219, // BULLET OPERATOR
    /* 0xFA */ 0x00B7, // MIDDLE DOT
    /* 0xFB */ 0x221A, // SQUARE ROOT
    /* 0xFC */ 0x207F, // SUPERSCRIPT LATIN SMALL LETTER N
    /* 0xFD */ 0x00B2, // SUPERSCRIPT TWO
    /* 0xFE */ 0x25A0, // BLACK SQUARE
    /* 0xFF */ 0x00A0, // NO-BREAK SPACE
};

static zd::text::single_byte_encoding _encoding{"x-mazovia", X_MAZOVIA};

zd::text::single_byte_encoding *zd::text::encoding::x_mazovia = &_encoding;