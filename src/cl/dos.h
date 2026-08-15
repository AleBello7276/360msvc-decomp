#pragma once

#include <Windows.h>
#include <stdlib.h>

/*
    @file dos.c
    @status


*/

extern BOOL Spawning;
extern BOOL GotCtrlC;

BOOL WINAPI NT_handling_function(DWORD CtrlType);
void OS_init();
void redirect();
