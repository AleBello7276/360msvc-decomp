#pragma once

#include "types.h"
#include <wchar.h>

// throw internal error
NORETURN void internal(const wchar_t* file, size_t line);
