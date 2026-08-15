#pragma once

#include <stdlib.h>

/*
    @file xnew.c
    @status MATCHING

    this source file contains a few memory utilities for the compiler driver (cl.exe)
    such as allocating and freeing memory or duplicating a widechar string
*/

void* xnew(size_t size);
void xfree(void* ptr);
wchar_t* xstrdup(const wchar_t* source);
