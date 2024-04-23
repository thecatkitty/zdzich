#pragma once

#include <zd/encoding.hpp>
#include <zd/error.hpp>
#include <zd/min_istream.hpp>

namespace zd
{

class pl_istream
{
    min_istream _stream;
    encoding   *_encoding;

  public:
    pl_istream(min_istream &&stream, encoding *enc = encoding::unknown)
        : _stream{std::move(stream)}, _encoding{enc}
    {
    }

    pl_istream(const ustring &name, encoding *enc = encoding::unknown) noexcept
        : _stream{name}, _encoding{enc}
    {
    }

    operator bool() const noexcept
    {
        return _stream.good();
    }

    result<int>
    read() noexcept;

    encoding *
    get_encoding() const
    {
        return _encoding;
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

} // namespace zd
