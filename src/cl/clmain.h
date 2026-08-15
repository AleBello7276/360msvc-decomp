#pragma once

#include "shared.h"
#include "types.h"

extern BOOL RedirStderr;
extern passinfo_s* currPassinfo;

// throw internal error
NORETURN void internal(const wchar_t* file, INT line);
