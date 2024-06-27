#include <zd/message.hpp>

extern "C" const zd::message_record MESSAGES_0409[];
extern "C" const zd::message_record MESSAGES_0415[];

extern const zd::translation TRANSLATIONS[]{
    {"en", MESSAGES_0409},
    {"pl", MESSAGES_0415},
    {"", nullptr},
};
