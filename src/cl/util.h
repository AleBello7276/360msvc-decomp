#pragma once

#include "types.h"

#define DONE_4 4

#define CMD_ERR_0x1f67 0x1f67

NORETURN void done(INT code);
void cmderr(DWORD error, ...);
