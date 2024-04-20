#pragma once

#include <cstdio>
#include <utility>

#include <zd/ustring.hpp>

namespace zd
{

class min_istream
{
    std::FILE *_file;

  public:
    min_istream() noexcept : _file{nullptr}
    {
    }

    min_istream(std::FILE *file) noexcept : _file{file}
    {
    }

    min_istream(const ustring &name) noexcept
        : _file{fopen(name.data(), "rb")}
    {
    }

    min_istream(const min_istream &) = delete;

    min_istream(min_istream &&that) noexcept
        : _file{std::exchange(that._file, nullptr)}
    {
    }

    min_istream &
    operator=(const min_istream &) = delete;

    min_istream &
    operator=(min_istream &&that) noexcept
    {
        if ((nullptr != _file) && (stdin != _file))
        {
            std::fclose(_file);
        }

        _file = std::exchange(that._file, nullptr);
        return *this;
    }

    ~min_istream()
    {
        if ((nullptr != _file) && (stdin != _file))
        {
            std::fclose(_file);
            _file = nullptr;
        }
    }

    std::FILE *
    get() const noexcept
    {
        return _file;
    }

    operator bool() const noexcept
    {
        return nullptr != _file;
    }

    std::size_t
    read(void *buffer, std::size_t size) noexcept
    {
        return std::fread(buffer, 1, size, _file);
    }

    int
    getc() noexcept
    {
        return std::fgetc(_file);
    }

    bool
    good() const noexcept
    {
        return (nullptr != _file) && !std::feof(_file) && !std::ferror(_file);
    }
};

} // namespace zd
