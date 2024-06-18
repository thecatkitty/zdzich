#pragma once

#include <zd.hpp>

namespace zd
{

class message
{
    void  *_ptr;
    size_t _len;

    message(void *ptr, size_t len) : _ptr{ptr}, _len{len}
    {
    }

  public:
    message(const message &) = delete;

    message(message &&that) noexcept
        : _ptr{std::exchange(that._ptr, nullptr)},
          _len{std::exchange(that._len, 0)}
    {
    }

    ~message();

    message &
    operator=(message &&that) noexcept
    {
        if (this == &that)
        {
            return *this;
        }

        this->~message();
        _ptr = std::exchange(that._ptr, nullptr);
        _len = std::exchange(that._len, 0);
        return *this;
    }

    void
    print(std::FILE *pf) const;

    static message
    retrieve(uint16_t         ordinal,
             size_t           argc = 0,
             const uintptr_t *argv = nullptr);
};

} // namespace zd
