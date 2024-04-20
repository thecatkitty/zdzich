#pragma once

#include <iterator>

#include <zd/encoding.hpp>

namespace zd
{

class ustring
{
  public:
    struct iterator
    {
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = char *;
        using reference = int &;

        iterator(pointer ptr) : _ptr{ptr}
        {
        }

        value_type
        operator*() const;

        iterator &
        operator++();

        iterator
        operator++(int)
        {
            auto old = *this;
            ++(*this);
            return old;
        }

        friend bool
        operator==(const iterator &left, const iterator &right)
        {
            return left._ptr == right._ptr;
        };

        friend bool
        operator!=(const iterator &left, const iterator &right)
        {
            return left._ptr != right._ptr;
        };

      private:
        pointer _ptr;
    };

  private:
    static const size_t ALIGNMENT{16};

    size_t _size;
    size_t _capacity;
    char  *_data;

  public:
    ustring() : _size{0}, _capacity{0}, _data{nullptr}
    {
    }

    ustring(const char *str);

    ~ustring();

    ustring(const ustring &that);

    ustring(ustring &&that) noexcept
        : _size{that._size}, _capacity{that._capacity},
          _data{std::exchange(that._data, nullptr)}
    {
    }

    ustring &
    operator=(const ustring &that);

    ustring &
    operator=(ustring &&that) noexcept;

    iterator
    begin() const
    {
        return iterator{_data};
    }

    iterator
    end() const
    {
        return iterator{_data + _size};
    }

    size_t
    size() const
    {
        return _size;
    }

    size_t
    capacity() const
    {
        return _capacity;
    }

    bool
    empty() const
    {
        return 0 == _size;
    }

    const char *
    data() const
    {
        return _data;
    }

    bool
    reserve(size_t new_cap);

    bool
    append(int codepoint);

    friend bool
    operator==(const ustring &left, const ustring &right);

    friend bool
    operator!=(const ustring &left, const ustring &right)
    {
        return !(left == right);
    }
};

} // namespace zd
