#pragma once

#include <zd.hpp>

namespace zd
{

namespace text
{

struct encoding;
struct single_byte_encoding;
struct multi_byte_encoding;

struct encoding
{
    virtual size_t
    decode(const char *buff, int &codepoint) = 0;

    virtual size_t
    encode(char *buff, int codepoint) = 0;

    virtual const char *
    get_name() const = 0;

    static encoding             *unknown;
    static single_byte_encoding *ibm852;
    static single_byte_encoding *windows_1250;
    static single_byte_encoding *iso_8859_2;
    static multi_byte_encoding  *utf_8;
    static single_byte_encoding *x_iso_or_windows;
    static single_byte_encoding *x_mazovia;

  protected:
    encoding() = default;
};

class single_byte_encoding : public encoding
{
    const char *_name;
    const int  *_mapping;

  public:
    single_byte_encoding(const char *name, const int *mapping)
        : _name{name}, _mapping{mapping}
    {
    }

    virtual size_t
    decode(const char *buff, int &codepoint) override;

    virtual size_t
    encode(char *buff, int codepoint) override;

    virtual const char *
    get_name() const override
    {
        return _name;
    }
};

class multi_byte_encoding : public encoding
{
  public:
    virtual size_t
    get_sequence_length(const char *buff) = 0;

  protected:
    multi_byte_encoding() = default;
};

} // namespace text

} // namespace zd
