#pragma once

#include <Windows.h>
#include <stdlib.h>

/*
    @file dos.c
    @status


*/

extern BOOL Spawning;
extern BOOL GotCtrlC;

struct passinfo_s;

INT ExecuteCleanupPass(passinfo_s* passinfo_, LPCWSTR module_name);

BOOL WINAPI NT_handling_function(DWORD CtrlType);
void OS_init();
void redirect();
