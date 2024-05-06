#pragma once

#include <zd/error.hpp>
#include <zd/io/min_istream.hpp>
#include <zd/text/encoding.hpp>

namespace zd
{

namespace lex
{

class pl_istream
{
    io::min_istream _stream;
    text::encoding *_encoding;

  public:
    pl_istream(io::min_istream &&stream,
               text::encoding   *enc = text::encoding::unknown)
        : _stream{std::move(stream)}, _encoding{enc}
    {
    }

    pl_istream(const ustring  &name,
               text::encoding *enc = text::encoding::unknown) noexcept
        : _stream{name}, _encoding{enc}
    {
    }

    operator bool() const noexcept
    {
        return _stream.good();
    }

    result<int>
    read() noexcept;

    text::encoding *
    get_encoding() const
    {
        return _encoding;
    }

    const ustring &
    get_path() const
    {
        return _stream.get_path();
    }

    enum class error_code : uint8_t
    {
        unexpected_byte = 0,
        invalid_sequence = 1,
        read_error = 2,
    };

  private:
    static tl::unexpected<error>
    make_error(error_code code)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::stream),
                  static_cast<uint8_t>(code)});
    }

    static tl::unexpected<error>
    make_error(error_code code, char byte, const char *encoding)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::stream),
                  static_cast<uint8_t>(code), byte, encoding});
    }
};

} // namespace lex

} // namespace zd
