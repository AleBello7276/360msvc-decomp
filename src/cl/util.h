#pragma once

#include "CodeAnalysis/sourceannotations.h"
#include "types.h"

#define DONE_4 4

#define CMD_ERR_0x1f67 0x1f67
#define CMD_ERR_8030 8030

#define MESSAGE_ID_105 105

template <size_t N>
wchar_t* wcscat(wchar_t (&dest)[N], const wchar_t* src) {
    return wcscat_s(dest, N, src) == 0 ? dest : nullptr;
}

/*
    strqlen (string quoted (!?!?) lenght)
    length of wchars needed to write `s` as an escaped, optionally
    quoted, Windows command-line argument.
*/
int strqlen(const wchar_t* s);

/* check if compiler is running in visual studio */
BOOL IsRunningUnderIDE(void**);

/**/
BOOL IsOutputToConsole(HANDLE h);

/* skips white characters (i love this name lol) */
wchar_t* gobblewhite(wchar_t* in);

/**/
wchar_t* concat(wchar_t* dst, size_t size, const wchar_t* concatee);

/**/
wchar_t* concatmeta(wchar_t* dst, size_t size, const wchar_t* concatee);

/**/
wchar_t* append(wchar_t* dst, size_t size, const wchar_t* appendee);

/* */
NORETURN void done(INT code);

/* */
void cmderr(DWORD error, ...);

/* */
wchar_t** sztoszv(wchar_t*, BOOL);

/* */
void print(int stream_, const wchar_t* format_, ...);

/* */
const wchar_t* get_message(UINT ID_);
