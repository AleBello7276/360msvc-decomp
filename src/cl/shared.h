#pragma once
#include <Windows.h>

typedef void(__stdcall* PASSINFO_FUNC)(INT t);

#define PASSINFO_FLAG_4 4
struct passinfo_s {
    char pad0[0xc];      // 0x0
    char flag;           // 0xc
    char pad2;           // 0xd
    char pad3;           // 0xe
    char pad4;           // 0xf
    int idk[2];          // 0x10
    PASSINFO_FUNC func;  // 0x18
};

//_Static_assert(sizeof(passinfo_s) == 0x1c, "passinfo");
