#include <unordered_map>

#include <zd/encoding.hpp>
#include <zd/token.hpp>

#define ENUM_NAME_MAPPING(name)                                                \
    {                                                                          \
        zd::token::name, #name                                                 \
    }

std::unordered_map<zd::token, std::string, zd::token_hash> ENUM_NAMES{
    ENUM_NAME_MAPPING(eof),
    ENUM_NAME_MAPPING(unknown),
    ENUM_NAME_MAPPING(line_break),
    ENUM_NAME_MAPPING(literal_int),
    ENUM_NAME_MAPPING(literal_str),
    ENUM_NAME_MAPPING(name),
    ENUM_NAME_MAPPING(comma),
    ENUM_NAME_MAPPING(end),
};

std::string
zd::to_string(token tok)
{
    if (ENUM_NAMES.end() != ENUM_NAMES.find(tok))
    {
        return ENUM_NAMES.at(tok);
    }

    return std::to_string(static_cast<int>(tok));
}
