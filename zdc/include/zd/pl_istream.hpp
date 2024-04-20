#pragma once

#include <zd/encoding.hpp>
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

    int
    read() noexcept;

    encoding *
    get_encoding() const
    {
        return _encoding;
    }
};

} // namespace zd
