#pragma once

#include "CodeAnalysis/sourceannotations.h"
#include "shared.h"
#include "types.h"
#include <cstdint>

struct context_s {
    uint32_t unk0x0;
};

extern context_s* Context;
extern context_s CX;

extern BOOL RedirStderr;
extern passinfo_s* currPassinfo;
extern const wchar_t* const ErrorReportingDlls;

// throw internal error
NORETURN void internal(const wchar_t* file, INT line);

size_t argcount(const wchar_t* const* args);
void build_context(const wchar_t*, context_s*);
