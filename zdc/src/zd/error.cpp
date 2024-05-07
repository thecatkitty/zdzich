#include <zd/error.hpp>

using namespace zd;

error::~error()
{
    if (!_argv)
    {
        return;
    }

    // Call all argument finalizers
    for (auto it = _argv; it < _argv + _argc; it++)
    {
        auto finalizer = reinterpret_cast<void (*)(uintptr_t)>(it[_argc]);
        if (finalizer)
        {
            finalizer(*it);
        }
    }

    // Destroy the buffer
    delete[] _argv;
    _argv = nullptr;
}

error &
error::operator=(error &&that) noexcept
{
    if (this == &that)
    {
        return *this;
    }

    this->~error();
    _origin = that._origin;
    _ordinal = that._ordinal;
    _file = std::move(that._file);
    _line = that._line;
    _column = that._column;
    _argc = std::exchange(that._argc, 0);
    _argv = std::exchange(that._argv, nullptr);
    return *this;
}

const uintptr_t
error::operator[](size_t i) const
{
    if (_argc <= i)
    {
        return 0;
    }

    return _argv[i];
}
