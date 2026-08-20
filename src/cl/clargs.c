#include "clargs.h"

// TODO document flags

int NoLogo = 0;
int NoOverrideWarnings = 0;
int PrintReproInfo = 0;
int UseEdge = 0;
int ZgFlag = 0;
int Cmd_ScaleMemory = 0;
int CluiPathFlag = 0;
int Cmd_PchBaseAddress = 0;
int DumpPCHInfo = 0;
int Cmd_Prefast = 0;
int Cmd_OldPrefast = 0;
int Cmd_Nopft = 0;
int MspftPathFlag = 0;
int C1xxastPathFlag = 0;
int C1astPathFlag = 0;
int AnalyzePathFlag = 0;
int NativeCodeAnalysisPathFlag = 0;
int MPSeen = 0;
int ZI_Flag = 0;  // x
int fLTCG = 0;
int QHVMode = 0;  // x

// clang-format off
early_switch_scan_table_s early_args[EARLY_ARGS_COUNT] = {
    {L"nologo", &NoLogo, FALSE},  
    {L"noover", &NoOverrideWarnings, FALSE},   
    {L"Be", &PrintReproInfo, FALSE}, 
    {L"BE", &UseEdge, FALSE},
    {L"Zg", &ZgFlag, FALSE},
    {L"Zm", &Cmd_ScaleMemory, FALSE}, 
    {L"BUI", &CluiPathFlag, FALSE},
    {L"Ym", &Cmd_PchBaseAddress, FALSE},
    {L"BY", &DumpPCHInfo, FALSE}, 
    {L"analyze", &Cmd_Prefast, FALSE},
    {L"prefast", &Cmd_Prefast, FALSE},
    {L"astfe", &Cmd_OldPrefast, FALSE}, 
    {L"astfe:nopft", &Cmd_Nopft, FALSE},
    {L"astfe:Ba", &MspftPathFlag, FALSE},
    {L"astfe:Bx", &C1xxastPathFlag, FALSE}, 
    {L"astfe:B1", &C1astPathFlag, FALSE},
    {L"BZ", &AnalyzePathFlag, FALSE},
    {L"analyze:Bn", &NativeCodeAnalysisPathFlag, FALSE}, 
    {L"MP", &MPSeen, FALSE},
    {L"ZI", &ZI_Flag, FALSE}, // x
    {L"GL", &fLTCG, FALSE},
    {L"Qhypervisor", &QHVMode, FALSE}, // x
};
// clang-format on
