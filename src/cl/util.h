#pragma once

#include "types.h"

#define DONE_4 4

#define CMD_ERR_0x1f67 0x1f67

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

NORETURN void done(INT code);
void cmderr(DWORD error, ...);
