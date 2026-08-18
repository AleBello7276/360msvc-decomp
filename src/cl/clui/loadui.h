#pragma once

#include <Windows.h>

#define PATH_BUFFER_SIZE 1024

#define ENGLISH_US_ID 1033

/**/
HINSTANCE LoadCLUI(wchar_t* outDir, size_t cchOutDir);

/**/
HRESULT LoadUILibrary(wchar_t* dir, wchar_t* lib_name, DWORD idk, HINSTANCE* out_module, wchar_t* idk2,
                      size_t some_size, DWORD* idk3);

/**/
wchar_t* fullccpath();

/**/
HINSTANCE LoadSearchPath(wchar_t* path, size_t size);
