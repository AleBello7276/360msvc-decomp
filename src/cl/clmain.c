#include "CodeAnalysis/sourceannotations.h"
#include "clmain.h"

#include "clui/loadui.h"
#include "dos.h"
#include "mpcl.h"
#include "util.h"
#include "xnew.h"
#include <clocale>

context_s* Context = &CX;
context_s CX = {};

#define STDOUT 1

void usage() {
    LOGO(FALSE);
    print(STDOUT, get_message(MESSAGE_ID_105));

    exit(0);
}

size_t argcount(const wchar_t* const* args) {
    size_t result = 0;
    if (args && *args) {
        do
            ++result;
        while (args[result]);
    }
    return result;
}

void build_context(const wchar_t* path_, context_s* ctx) {
    memset(ctx, 0, sizeof(context_s));
    ctx->mPath = extract_path(path_);
}

wchar_t* extract_path(const wchar_t* in_path_) {
    const size_t BUF_LEN = 256;
    wchar_t drive[MAX_PATH];
    wchar_t dir[BUF_LEN];
    wchar_t fname[BUF_LEN];
    wchar_t ext[BUF_LEN];

    _wsplitpath_s(in_path_, drive, MAX_PATH, dir, BUF_LEN, fname, BUF_LEN, ext, BUF_LEN);

    int len = wcslen(dir);
    if (len > 0) {
        wchar_t last = dir[len - 1];
        if (last != L'\\' && last != L':' && last != L'/') {
            dir[len] = L'\\';
            dir[len + 1] = L'\0';
        }
    }

    const wchar_t* final = wcscat<MAX_PATH>(drive, dir);
    return xstrdup(final);
}

int wmain(int _Argc, wchar_t** _Argv, wchar_t** _Env) {
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
    _Argv[0] = cl_path;
    build_context(cl_path, (context_s*)Context);

    if (_Argc == 1 && cl_count == 0)
        usage();

    return 0;
}
