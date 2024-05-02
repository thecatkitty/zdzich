#pragma once

#include <algorithm>
#include <array>

namespace zd
{

template <typename Container, typename T>
inline bool
contains(const Container &container, const T &value)
{
    return std::end(container) !=
           std::find(std::begin(container), std::end(container), value);
}

template <typename Container, typename T>
inline size_t
get_index(const Container &container, const T &value)
{
    return std::find(std::begin(container), std::end(container), value) -
           std::begin(container);
}

template <typename T> class range
{
    const T     *_items;
    const size_t _size;

  public:
    range(T *const items, size_t size) : _items{items}, _size{size}
    {
    }

    T const *
    begin() const
    {
        return _items;
    }

    T const *
    end() const
    {
        return _items + _size;
    }
};

} // namespace zd
