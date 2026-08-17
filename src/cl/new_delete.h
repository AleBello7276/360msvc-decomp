#pragma once

#include "cl/xnew.h"

/* sill gotta figure if this is what it was actually done */

inline void* operator new(size_t size) {
    return xnew(size);
}

inline void operator delete(void* p) {
    xfree(p);
}
