#include "xnew.h"

#include "clmain.h"
#include "new_delete.h"

#undef __FILEW__
#define __FILEW__ L"e:\\bt\\278379\\vctools\\compiler\\cl\\xnew.c"

void* xnew(size_t size) {
    void* p = malloc(size);

    if (p == 0) {
#line 90
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
#line 108
        internal(__FILEW__, __LINE__);

    return cpy;
}

source_s* newsource(const wchar_t* source_name, source_type type) {
    source_s* out = new source_s;
    out->mSrcType = type;
    out->unk_0x18 = 1;
    wchar_t* dup = xstrdup(source_name);
    out->unk_0x10 = 0;
    out->unk_0x0 = 0;
    out->unk_0x4 = dup;
    out->unk_0x8 = dup;
    out->unk_0xc = dup;

    out->unk_0x1a = -1;
    return out;
}
