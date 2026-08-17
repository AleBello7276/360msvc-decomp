#include "dos.h"

#include "clmain.h"
#include "shared.h"
#include "util.h"

#include <cstddef>
#include <io.h>
#include <stdio.h>
#include <wchar.h>

#undef __FILEW__
#define __FILEW__ L"e:\\bt\\278379\\vctools\\compiler\\cl\\dos.c"

#define STDOUT 1
#define STDERR 2

BOOL Spawning = FALSE;
BOOL GotCtrlC = FALSE;

INT ExecuteCleanupPass(passinfo_s* passinfo_, LPCWSTR module_name) {
    HMODULE hModule;
    FARPROC func;

    hModule = GetModuleHandleW(module_name);
    if (hModule) {
        func = GetProcAddress(hModule, "_CloseTypeServerPDB@0");
        if (func == nullptr) {
            cmderr(CMD_ERR_0x1f67, L"_CloseTypeServerPDB@0", module_name);
            return -1;
        }

        passinfo_->mCloseTypeSrvFunc = func;
        func();
    }

    return 0;
}

int execute(int param_1, passinfo_s* passinfo_, LPCWSTR lib, LPCWSTR param_4, LPCWSTR** param_5) {
    if (*param_5 == nullptr) {
#line 245
        internal(__FILEW__, __LINE__);
    }

    if (param_1 == 0) {
        if (param_4) {
            int line = 4;
            if (passinfo_->flag & PASSINFO_FLAG_4) {
                int argIndex = 1;  // skip argv[0]
                LPCWSTR* arg = param_5[1];

                while (arg != NULL) {
                    size_t len = wcslen(*arg);
                    line += len + 1;

                    argIndex++;
                    arg = param_5[argIndex];
                }
            }
        }
    }

    return 0;
}

BOOL WINAPI NT_handling_function(DWORD CtrlType) {
    GotCtrlC = TRUE;

    if (Spawning == FALSE) {
        SetConsoleCtrlHandler(NULL, TRUE);
        done(DONE_4);
    }

    if ((currPassinfo->flag & PASSINFO_FLAG_4) != 0) {
        if (currPassinfo->mAbortFunc) {
            currPassinfo->mAbortFunc(ABORT_CODE_2);
        } else {
#line 474
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
