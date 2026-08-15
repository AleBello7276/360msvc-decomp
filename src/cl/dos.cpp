#include "dos.h"

#include "clmain.h"
#include "util.h"

#include <io.h>
#include <stdio.h>

#define STDOUT 1
#define STDERR 2

BOOL Spawning = FALSE;
BOOL GotCtrlC = FALSE;

BOOL WINAPI NT_handling_function(DWORD CtrlType) {
    GotCtrlC = TRUE;

    if (Spawning == FALSE) {
        SetConsoleCtrlHandler(NULL, TRUE);
        done(DONE_4);
    }

    if ((currPassinfo->flag & PASSINFO_FLAG_4) != 0) {
        if (currPassinfo->func) {
            currPassinfo->func(2);
        } else {
#line 474 "e:\\bt\\278379\\vctools\\compiler\\cl\\dos.c"
            internal(__FILEW__, __LINE__);
        }
    }

    return TRUE;
}

void OS_init() {
    SetConsoleCtrlHandler(NT_handling_function, TRUE);
}

void redirect() {
    if (RedirStderr) {
        _dup2(STDOUT, STDERR);
    }
}
