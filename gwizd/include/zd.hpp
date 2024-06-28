#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <climits>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <utility>

#include <zd/error.hpp>
#include <zd/ustring.hpp>

#ifndef PATH_MAX
#ifdef _WIN32
#define PATH_MAX 260
#else
#include <sys/syslimits.h>
#endif
#endif
