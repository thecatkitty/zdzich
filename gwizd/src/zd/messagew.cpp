#include <cstdio>
#include <io.h>
#include <windows.h>

#include <zd/message.hpp>

using namespace zd;

message::~message()
{
    if (_ptr)
    {
        ::LocalFree(_ptr);
        _ptr = nullptr;
        _len = 0;
    }
}

void
message::print(std::FILE *pf) const
{
    auto fd = ::_fileno(pf);
    auto handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
    ::WriteConsoleW(handle, _ptr, _len, NULL, NULL);
}

message
message::retrieve(uint16_t ordinal, size_t argc, const uintptr_t *argv)
{
    static_assert(sizeof(uintptr_t) == sizeof(DWORD_PTR),
                  "DWORD_PTR differs from uintptr_t on this platform!");

    LPWSTR buffer{nullptr};
    size_t length = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY |
            FORMAT_MESSAGE_FROM_HMODULE,
        nullptr, ordinal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&buffer), 0,
        const_cast<va_list *>(reinterpret_cast<const va_list *>(argv)));
    return {buffer, length};
}
