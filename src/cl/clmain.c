#include "CodeAnalysis/sourceannotations.h"
#include "clmain.h"

#include "dos.h"
#include "loadui.h"
#include "mpcl.h"
#include "util.h"
#include <clocale>

context_s* Context = &CX;
context_s CX;

size_t argcount(const wchar_t* const* args) {
    size_t result = 0;
    if (args && *args) {
        do
            ++result;
        while (args[result]);
    }
    return result;
}

void build_context(const wchar_t* path, context_s* ctx) {
    for (int i = 10; i != 0; i--) {
        ctx->unk0x0 = 0;
        ctx = ctx + 1;
    }
}

int _wmain(int _Argc, wchar_t** _Argv, wchar_t** _Env) {
    // some init i guess
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    setlocale(LC_ALL, "");
    SetErrorMode(SEM_FAILCRITICALERRORS);

    // 0 args !?
    if (_Argc < 1)
        cmderr(CMD_ERR_8030);

    OS_init();
    if (MP_is_server() == FALSE) {
        get_mutex();
    }

    wchar_t* CL_var = nullptr;
    wchar_t* _CL__var = nullptr;
    _wdupenv_s(&CL_var, nullptr, L"CL");
    _wdupenv_s(&_CL__var, nullptr, L"_CL_");

    wchar_t** args_CL = sztoszv(CL_var, TRUE);
    size_t cl_count = argcount(args_CL);
    wchar_t** args__CL_ = sztoszv(_CL__var, TRUE);
    size_t _cl__count = argcount(args__CL_);
    free(CL_var);
    CL_var = nullptr;
    free(_CL__var);
    _CL__var = nullptr;

    wchar_t* cl_path = fullccpath();

    return 0;
}
