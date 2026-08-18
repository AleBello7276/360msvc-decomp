#include "loadui.h"

#include "cl/clmain.h"
#include "stdlib.h"
#include <cstring>
#include <wchar.h>

#undef __FILEW__
#define __FILEW__ L"e:\\bt\\278379\\vctools\\compiler\\clui\\loadui.c"

// fck this
HRESULT LoadUILibrary(wchar_t* dir, wchar_t* lib_name, DWORD flags, HINSTANCE* out_module, wchar_t* out_path,
                      size_t out_path_size, DWORD* out_lang) {
    wchar_t path[PATH_BUFFER_SIZE];
    path[0] = L'\0';
    memset(&path[1], 0, sizeof(path) - sizeof(wchar_t));

    LCID matchedID = -1;
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
    if (len >= PATH_BUFFER_SIZE - 1)
        len = PATH_BUFFER_SIZE - 1;

    const size_t LOCALE_COUNT = 3;
    LCID localeIDs[LOCALE_COUNT];
    localeIDs[0] = GetUserDefaultUILanguage();
    localeIDs[1]
        = MAKELCID(MAKELANGID(PRIMARYLANGID(localeIDs[0]), SUBLANG_DEFAULT), SORTIDFROMLCID(localeIDs[0]));
    localeIDs[2] = ENGLISH_US_ID;

    BOOL found = FALSE;

    for (int i = 0; (unsigned)i < LOCALE_COUNT; i++) {
        int j = 0;
        for (; j < i; j++)
            if (localeIDs[j] == localeIDs[i])
                break;
        if (j < i)
            continue;

        // listen.. i have no idea why this works or why it was proably written originally like this
        LCID* locale = localeIDs;

        path[len] = L'\0';

        const size_t BUF_SIZE = 10;
        wchar_t numBuf[BUF_SIZE];
        _itow_s(locale[i], numBuf, BUF_SIZE, BUF_SIZE);
        wcsncat_s(path, PATH_BUFFER_SIZE, numBuf, _TRUNCATE);
        wcsncat_s(path, PATH_BUFFER_SIZE, L"\\", _TRUNCATE);
        wcsncat_s(path, PATH_BUFFER_SIZE, lib_name, _TRUNCATE);

        if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
            continue;

        UINT consoleCP = GetConsoleOutputCP();

        if (locale[i] == ENGLISH_US_ID) {
            matchedID = locale[i];
            found = TRUE;
            break;
        }

        WORD primary = PRIMARYLANGID(locale[i]);
        if (primary != LANG_ARABIC && primary != LANG_HEBREW) {
            const size_t DATA_SIZE = 12;
            wchar_t lcData[DATA_SIZE];

            LANGID lang = (LANGID)locale[i];
            GetLocaleInfoW(lang, LOCALE_IDEFAULTCODEPAGE, lcData, DATA_SIZE);
            int oemCP = _wtoi(lcData);

            GetLocaleInfoW(lang, LOCALE_IDEFAULTANSICODEPAGE, lcData, DATA_SIZE);
            int ansiCP = _wtoi(lcData);

            if (consoleCP == CP_UTF8 || consoleCP == oemCP || consoleCP == ansiCP) {
                matchedID = locale[i];
                found = TRUE;
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

        len = wcslen(path);
        if (len >= PATH_BUFFER_SIZE - 1)
            len = PATH_BUFFER_SIZE - 1;

        if (hFind != INVALID_HANDLE_VALUE) {
            while (FindNextFileW(hFind, &findData)) {
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
            }
            FindClose(hFind);
        }
    }

    if (SUCCEEDED(hr)) {
        if (out_lang) {
            if (matchedID != -1) {
                *out_lang = matchedID;
            } else {
#line 210
                internal(__FILEW__, __LINE__);
            }
        }

        if (out_module != nullptr) {
            *out_module = LoadLibraryExW(path, nullptr, flags);
            hr = *out_module ? S_OK : E_FAIL;
        }

        if (out_path != nullptr)
            wcscpy_s(out_path, out_path_size, path);
        return hr;
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
