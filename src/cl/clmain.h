#pragma once

#include "CodeAnalysis/sourceannotations.h"
#include "shared.h"
#include "types.h"
#include <cstdint>

struct context_s {
    uint32_t unk0x00;
    uint8_t pad0x04[0x20];
    wchar_t* mPath;
};

extern context_s* Context;
extern context_s CX;

extern BOOL RedirStderr;
extern passinfo_s* currPassinfo;
extern const wchar_t* const ErrorReportingDlls;

// throw internal error
NORETURN void internal(const wchar_t* file, INT line);

/* */
NORETURN void usage();

void LOGO(BOOL);

/* */
size_t argcount(const wchar_t* const* args);

/* */
void build_context(const wchar_t* path_, context_s* ctx);

/* */
wchar_t* extract_path(const wchar_t*);

/* */
void early_switch_scan(const wchar_t* const* args_, int count_);
