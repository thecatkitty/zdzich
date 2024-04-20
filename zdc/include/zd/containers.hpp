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

} // namespace zd
