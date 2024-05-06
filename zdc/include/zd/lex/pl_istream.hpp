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
    unsigned        _line;
    unsigned        _column;

  public:
    pl_istream(io::min_istream &&stream,
               text::encoding   *enc = text::encoding::unknown)
        : _stream{std::move(stream)}, _encoding{enc}, _line{1}, _column{1}
    {
    }

    pl_istream(const ustring  &name,
               text::encoding *enc = text::encoding::unknown) noexcept
        : _stream{name}, _encoding{enc}, _line{1}, _column{1}
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

    unsigned
    get_line() const
    {
        return _line;
    }

    unsigned
    get_column() const
    {
        return _column;
    }

    enum class error_code : uint8_t
    {
        unexpected_byte = 0,
        invalid_sequence = 1,
        read_error = 2,
    };

  private:
    tl::unexpected<error>
    make_error(error_code code)
    {
        return tl::make_unexpected(error{
            static_cast<uint8_t>(error_origin::stream),
            static_cast<uint8_t>(code), get_path(), get_line(), get_column()});
    }

    tl::unexpected<error>
    make_error(error_code code, char byte, const char *encoding)
    {
        return tl::make_unexpected(
            error{static_cast<uint8_t>(error_origin::stream),
                  static_cast<uint8_t>(code), get_path(), get_line(),
                  get_column(), byte, encoding});
    }
};

} // namespace lex

} // namespace zd
