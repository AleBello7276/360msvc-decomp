

#include "types.h"
#include <wchar.h>

NORETURN void internal(const wchar_t* file, size_t line);

void* xnew(size_t size) {
    void* p = malloc(size);

    if (p == 0) {
#line 90 "e:\\bt\\278379\\vctools\\compiler\\cl\\xnew.c"
        internal(__FILEW__, __LINE__);
    }

    return p;
}

void xfree(void* ptr) {
    if (ptr)
        free(ptr);
}

wchar_t* xstrdup(const wchar_t* source) {
    wchar_t* cpy;

    if (!source)
        return NULL;

    cpy = _wcsdup(source);
    if (!cpy)
#line 108 "e:\\bt\\278379\\vctools\\compiler\\cl\\xnew.c"
        internal(__FILEW__, __LINE__);

    return cpy;
}
