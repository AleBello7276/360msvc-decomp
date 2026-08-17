#pragma once

#include "CodeAnalysis/sourceannotations.h"
#include <cstdint>
#include <stdlib.h>

/*
    @file xnew.c
    @status MATCHING

    this source file contains a few memory utilities for the compiler driver (cl.exe)
    such as allocating and freeing memory or duplicating a widechar string
*/

enum source_type { SOURCE_TYPE_1 = 1, SOURCE_TYPE_2 = 2, SOURCE_TYPE_3 = 3, SOURCE_TYPE_4 = 4 };

struct source_s {
    uint32_t unk_0x0;
    wchar_t* unk_0x4;
    wchar_t* unk_0x8;
    wchar_t* unk_0xc;
    int unk_0x10;
    source_type mSrcType;
    short unk_0x18;
    short unk_0x1a;
};

//_Static_assert(sizeof(source_s) == 0x1c, "source");

void* xnew(size_t size);
void xfree(void* ptr);
wchar_t* xstrdup(const wchar_t* source);

source_s* newsource(const wchar_t* source_name, source_type type);
