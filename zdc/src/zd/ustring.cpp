#include <algorithm>
#include <cstring>
#include <new>

#include <zd/text/encoding.hpp>
#include <zd/ustring.hpp>

#define USTR_ALIGNMENT 16

using namespace zd;

// zd::ustring class

ustring::ustring(const char *str)
    : _size{std::strlen(str)},
      _capacity{(_size / USTR_ALIGNMENT + 1) * USTR_ALIGNMENT - 1},
      _data{new(std::nothrow) char[_capacity + 1]}
{
    std::copy_n(str, _size + 1, _data);
}

ustring::~ustring()
{
    if (_data)
    {
        delete[] _data;
        _data = nullptr;
    }
}

ustring::ustring(const ustring &that)
    : _size{that._size}, _capacity{that._capacity}, _data{nullptr}
{
    if (that._size)
    {
        _data = new (std::nothrow) char[that._capacity + 1];
        std::copy_n(that._data, _size + 1, _data);
    }
}

ustring &
ustring::operator=(const ustring &that)
{
    if (this == &that)
    {
        return *this;
    }

    this->~ustring();
    _size = that._size;
    _capacity = that._capacity;
    _data = new (std::nothrow) char[that._capacity + 1];
    std::copy_n(that._data, _capacity, _data);

    return *this;
}

ustring &
ustring::operator=(ustring &&that) noexcept
{
    if (this == &that)
    {
        return *this;
    }

    this->~ustring();
    _size = that._size;
    _capacity = that._capacity;
    _data = std::exchange(that._data, nullptr);

    return *this;
}

bool
ustring::reserve(size_t new_cap)
{
    if (_capacity >= new_cap)
    {
        return false;
    }

    auto aligned_cap = (new_cap / USTR_ALIGNMENT + 1) * USTR_ALIGNMENT - 1;
    auto new_data = new (std::nothrow) char[aligned_cap + 1];
    if (!new_data)
    {
        return false;
    }

    _capacity = aligned_cap;
    new_data[_size] = 0;
    if (!_data)
    {
        // Nothing to move
        _data = new_data;
        return true;
    }

    std::copy_n(_data, _size, new_data);
    delete[] std::exchange(_data, new_data);
    return true;
}

bool
ustring::append(int codepoint)
{
    char sequence[5]{};
    auto length = text::encoding::utf_8->encode(sequence, codepoint);
    if (_capacity < (_size + length))
    {
        if (!reserve(_size + length))
        {
            return false;
        }
    }

    std::copy_n(sequence, length + 1, _data + _size);
    _size += length;
    return true;
}

void
ustring::clear()
{
    _size = 0;
    if (_data)
    {
        *_data = 0;
    }
}

bool
zd::operator==(const ustring &left, const ustring &right)
{
    return (left._size == right._size) &&
           std::equal(left._data, left._data + left._size, right._data);
}

// zd::ustring::iterator class

zd::ustring::iterator::value_type
zd::ustring::iterator::operator*() const
{
#ifdef __ia16__
    int codepoint{'?'};
#else
    int codepoint{0x00FFFD};
#endif
    text::encoding::utf_8->decode(_ptr, codepoint);
    return codepoint;
}

zd::ustring::iterator &
zd::ustring::iterator::operator++()
{
    size_t length{0};
    while (0 == (length = text::encoding::utf_8->get_sequence_length(_ptr)))
    {
        _ptr++;
    }

    _ptr += length;
    return *this;
}
