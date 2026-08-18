#include "loadui.h"

#include "cl/clmain.h"
#include "stdlib.h"
#include <cstring>
#include <wchar.h>

#undef __FILEW__
#define __FILEW__ L"e:\\bt\\278379\\vctools\\compiler\\clui\\loadui.c"

// not matching, will stay that way for a while..
HRESULT LoadUILibrary(wchar_t* dir, wchar_t* lib_name, DWORD flags, HINSTANCE* out_module, wchar_t* out_path,
                      size_t out_path_size, DWORD* out_lang) {
    wchar_t path[PATH_BUFFER_SIZE];
    path[0] = L'\0';
    memset(&path[1], 0, sizeof(path) - sizeof(wchar_t));

    DWORD matchedID = (DWORD)-1;
    HRESULT hr = E_FAIL;

    if (dir == nullptr || *dir == L'\0') {
#line 53
        internal(__FILEW__, __LINE__);
        return hr;
    }

    if (lib_name == nullptr || *lib_name == L'\0') {
#line 54
        internal(__FILEW__, __LINE__);
        return hr;
    }

    if (out_module != nullptr)
        *out_module = nullptr;

    wcsncpy_s(path, PATH_BUFFER_SIZE, dir, _TRUNCATE);

    size_t len = wcslen(path);
    if (path[len - 1] != L'\\')
        wcsncat_s(path, PATH_BUFFER_SIZE, L"\\", _TRUNCATE);

    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
        return E_FAIL;

    len = wcslen(path);
    if (len > PATH_BUFFER_SIZE - 1)
        len = PATH_BUFFER_SIZE - 1;

    DWORD langs[3];
    langs[0] = GetUserDefaultUILanguage();
    langs[1] = MAKELANGID(PRIMARYLANGID(langs[0]), SUBLANG_DEFAULT);
    langs[2] = 0x409;  // English (US)

    bool found = false;

    for (int i = 0; i < 3 && !found; i++) {
        int j;
        for (j = 0; j < i; j++)
            if (langs[j] == langs[i])
                break;
        if (j != i)
            continue;

        wchar_t numBuf[10];
        path[len] = L'\0';
        _itow_s(langs[i], numBuf, 10, 10);
        wcsncat_s(path, PATH_BUFFER_SIZE, numBuf, _TRUNCATE);
        wcsncat_s(path, PATH_BUFFER_SIZE, L"\\", _TRUNCATE);
        wcsncat_s(path, PATH_BUFFER_SIZE, lib_name, _TRUNCATE);

        if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
            continue;

        UINT consoleCP = GetConsoleOutputCP();

        if (langs[i] == 0x409) {
            matchedID = langs[i];
            found = true;
            break;
        }

        WORD primary = PRIMARYLANGID((WORD)langs[i]);
        if (primary != LANG_ARABIC && primary != LANG_HEBREW) {
            LCID locale = (LCID)langs[i];
            WCHAR lcData[12];

            GetLocaleInfoW(locale, LOCALE_IDEFAULTCODEPAGE, lcData, 12);
            int oemCP = _wtoi(lcData);

            GetLocaleInfoW(locale, LOCALE_IDEFAULTANSICODEPAGE, lcData, 12);
            int ansiCP = _wtoi(lcData);

            if (consoleCP == CP_UTF8 || consoleCP == (UINT)oemCP || consoleCP == (UINT)ansiCP) {
                matchedID = langs[i];
                found = true;
                break;
            }
        }
    }

    if (found) {
        hr = S_OK;
    } else {
        path[len] = L'\0';
        wcsncat_s(path, PATH_BUFFER_SIZE, L"*.*", _TRUNCATE);

        len = wcslen(path);

        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW(path, &findData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    continue;
                if (wcscmp(findData.cFileName, L".") == 0)
                    continue;
                if (wcscmp(findData.cFileName, L"..") == 0)
                    continue;

                path[len - 3] = L'\0';
                wcsncat_s(path, PATH_BUFFER_SIZE, findData.cFileName, _TRUNCATE);
                wcsncat_s(path, PATH_BUFFER_SIZE, L"\\", _TRUNCATE);
                wcsncat_s(path, PATH_BUFFER_SIZE, lib_name, _TRUNCATE);

                if (GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES) {
                    matchedID = (DWORD)_wtol(findData.cFileName);
                    hr = S_OK;
                    break;
                }
            } while (FindNextFileW(hFind, &findData));
            FindClose(hFind);
        }
    }

    if (FAILED(hr)) {
        if (out_lang != nullptr) {
            if (matchedID != (DWORD)-1) {
                *out_lang = matchedID;
            }
#line 210
            internal(__FILEW__, __LINE__);
        } else {
            if (out_module != nullptr) {
                *out_module = LoadLibraryExW(path, nullptr, flags);

                if (*out_module == nullptr) {
                    hr = E_FAIL;
                }
            }

            if (out_path != nullptr)
                wcscpy_s(out_path, out_path_size, path);
        }
    }
    return hr;
}

wchar_t* fullccpath() {
    wchar_t* out = nullptr;
    _get_wpgmptr(&out);
    return out;
}

HINSTANCE LoadSearchPath(wchar_t* path, size_t size) {
    wchar_t* envPath = NULL;
    HINSTANCE module = NULL;
    wchar_t dir[PATH_BUFFER_SIZE];

    _wdupenv_s(&envPath, NULL, L"PATH");

    if (envPath != NULL) {
        wchar_t* p = envPath;

        while (*p != L'\0') {
            // skip spaces and ;
            while (*p == L' ' || *p == L';')
                p++;

            if (*p == L'\0')
                break;

            // search directory until ';'
            int len = 0;
            wchar_t* dst = dir;

            while (*p != L'\0' && *p != L';') {
                if (++len >= PATH_BUFFER_SIZE)
                    break;

                *dst++ = *p++;
            }

            *dst = L'\0';

            // try to load clui.dll
            if (len != 0 && len < PATH_BUFFER_SIZE) {
                LoadUILibrary(dir, L"clui.dll", 2, &module, path, size, NULL);

                if (module != NULL)
                    break;
            }
        }
    }

    free(envPath);
    return module;
}
