
#include "clmain.h"
#include "util.h"
#include <cstdint>
#include <cstring>
#include <wchar.h>

#undef __FILEW__
#define __FILEW__ L"e:\\bt\\278379\\vctools\\compiler\\cl\\util.c"

int strqlen(const wchar_t* s) {
    UINT backslashes = 0;
    UINT len = 0;
    BOOL needs_quotes = FALSE;

    if (*s != '\0') {
        for (; *s != '\0'; s++) {
            if (iswspace(*s)) {
                needs_quotes = TRUE;
            }

            if (!(*s - '\"')) {
                len += 1 + backslashes;
            }

            backslashes = (!(*s - '\\') ? backslashes + 1 : 0);

            len++;
        }

        if (needs_quotes)
            len += 2 + backslashes;
    }

    return len;
}

wchar_t* concat(wchar_t* dst, size_t src_len, const wchar_t* src);

// NON_MATCHING -> fuck this, fusckit idk how many hours i spent on this alone, damn
wchar_t* strqcpy(wchar_t* dst, const wchar_t* src) {
    wchar_t* write = dst;
    const wchar_t* in = src;
    int backslashes = 0;
    BOOL needs_quotes = false;

    if (*src) {
        for (; *in != L'\0'; in++) {
            if (iswspace(*in))
                needs_quotes = true;

            if (!(*in - L'\"')) {
                if (backslashes >= 0) {
                    for (int i = (backslashes + 1) & 1; i != 0; i--) {
                        *write++ = L'\\';
                    }

                    write += backslashes + 1;
                    backslashes = 0;
                }
                backslashes--;
            }

            backslashes = (!(*in - '\\') ? backslashes + 1 : 0);

            *write++ = *in;  // copy wchar, then advance
        }

        if (needs_quotes) {
            size_t len = write - dst;
            memmove(write + 1, write, sizeof(wchar_t) * len);
            wchar_t* end = write + 1 + len;
            *dst = L'\"';

            if (backslashes >= 0) {
                write = end;
                for (int i = (backslashes) & 1; i != 0; i--) {
                    *write++ = L'\\';
                }
                write += backslashes;
            }

            *end = L'\"';
            write = end + 1;
        }
    }

    *write = L'\0';
    return dst;
}

BOOL IsRunningUnderIDE(PHANDLE h) {
    const size_t SIZE = 64;
    WCHAR value[SIZE];

    if (GetEnvironmentVariableW(L"VS_UNICODE_OUTPUT", NULL, NULL)) {
        GetEnvironmentVariableW(L"VS_UNICODE_OUTPUT", value, SIZE);

        *h = (HANDLE)_wtoi(value);
        return TRUE;
    }
    return FALSE;
}

BOOL IsOutputToConsole(HANDLE h) {
    // is *h* char file
    if ((GetFileType(h) & ~FILE_TYPE_REMOTE) != FILE_TYPE_CHAR)
        return FALSE;

    DWORD mode;
    return GetConsoleMode(h, &mode) != FALSE;
}

wchar_t* gobblewhite(wchar_t* in) {
    while (*in != L'\0' && iswspace(*in))
        in++;

    return in;
}

wchar_t* concat(wchar_t* dst, size_t size, const wchar_t* concatee) {
    if (dst) {
        if (concatee)
            wcscat_s(dst, size, concatee);
        return &dst[wcslen(dst)];
    }

#line 54
    internal(__FILEW__, __LINE__);
}

wchar_t* concatmeta(wchar_t* dst, size_t capacity, const wchar_t* src) {
    wchar_t* out;
    size_t len;

    if (dst == nullptr || capacity == 0) {
#line 65
        internal(__FILEW__, __LINE__);
    }

    len = wcslen(dst);

    // not enough space
    if (capacity <= len) {
#line 71
        internal(__FILEW__, __LINE__);
    }

    out = dst + len;
    capacity -= len;

    if (src != NULL) {
        for (; *src != L'\0'; ++src) {
            if (capacity == 0)
                break;

            *out++ = *src;
            --capacity;

            // '%' doubles it. idk why
            if (*src == L'%' && capacity != 0) {
                *out++ = L'%';
                --capacity;
            }
        }

        if (capacity == 0) {
#line 89
            internal(__FILEW__, __LINE__);
        }

        *out = L'\0';
    }
    return out;
}

wchar_t* append(wchar_t* dst, size_t size, const wchar_t* appendee) {
    if (dst) {
        if (appendee)
            wcscpy_s(dst, size, appendee);
        return &dst[wcslen(dst)];
    }

#line 101
    internal(__FILEW__, __LINE__);
}
