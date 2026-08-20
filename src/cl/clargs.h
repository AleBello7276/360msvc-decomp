#pragma once

#include "excpt.h"
#include <Windows.h>
#include <cstdint>

/*
    existance of this is a speculation and a test,
    it doesnt appear in object contribution for compiler args
    related stuff, so a similar TU may existed but linker *evicted* it
    and so data have no object contribution assigned
*/

/* single entry */
struct early_switch_scan_table_s {
    const wchar_t* mMnemonic;
    int* mStatus;
    int idk;
};

#define EARLY_ARGS_COUNT 22
extern early_switch_scan_table_s early_args[EARLY_ARGS_COUNT];

extern int NoLogo;
extern int NoOverrideWarnings;
extern int PrintReproInfo;
extern int UseEdge;
extern int ZgFlag;
extern int Cmd_ScaleMemory;
extern int CluiPathFlag;
extern int Cmd_PchBaseAddress;
extern int DumpPCHInfo;
extern int Cmd_Prefast;
extern int Cmd_OldPrefast;
extern int Cmd_Nopft;
extern int MspftPathFlag;
extern int C1xxastPathFlag;
extern int C1astPathFlag;
extern int AnalyzePathFlag;
extern int NativeCodeAnalysisPathFlag;
extern int MPSeen;
extern int ZI_Flag;  // x
extern int fLTCG;
extern int QHVMode;  // x
