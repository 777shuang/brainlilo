#include <windows.h>

HINSTANCE lib;

void* GetProcAddress(const wchar_t* procName);

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    lib = LoadLibrary(L"BrainLILO");
    if (!lib)
    {
        OutputDebugString(L"BrainLILO: Failed to load BrainLILO.dll");
        return 1;
    }

    typedef void (*getThisDllDirectoryPathProc)(LPWSTR);
    getThisDllDirectoryPathProc getThisDllDirectoryPath = (getThisDllDirectoryPathProc)GetProcAddress(L"getThisDllDirectoryPath");
    if (!getThisDllDirectoryPath)
    {
        FreeLibrary(lib);
        return 1;
    }

    wchar_t path[MAX_PATH];
    getThisDllDirectoryPath(path);
    wcscat(path, L"\\skip");
    HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        int id = MessageBox(
            NULL,
            L"Start BrainLILO.\nDo you want to skip this message box next time?",
            L"Are you sure?",
            MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2
        );
        
        switch (id)
        {
            case IDYES:
            hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE)
            {
                const wchar_t msg[] = L"BrainLILO: Failed to create file: \"";
                wchar_t str[sizeof(msg) / sizeof(wchar_t) + MAX_PATH];
                wcscpy(str, msg);
                wcscat(str, path);
                wcscat(str, L"\"");
                OutputDebugString(str);
            }
            else
            {
                const char msg[] = "This file is used to skip confirm message box";
                DWORD written;
                WriteFile(hFile, msg, sizeof(msg), &written, NULL);
                CloseHandle(hFile);
            }
            break;

            case IDCANCEL:
            FreeLibrary(lib);
            return 0;
            break;
        }
    }
    else { CloseHandle(hFile); }

    typedef BOOL (*RKDoSoftResetProc)();
    RKDoSoftResetProc RKDoSoftReset = (RKDoSoftResetProc)GetProcAddress(L"RKDoSoftReset");
    if (!RKDoSoftReset)
    {
        FreeLibrary(lib);
        return 1;
    }

    FreeLibrary(lib);
    return 0;
}

void* GetProcAddress(const wchar_t *procName)
{
    void* proc = (void*)GetProcAddress(lib, procName);
    if (!proc)
    {
        const wchar_t msg[] = L"BrainLILO: Failed to load proc: ";
        wchar_t str[sizeof(msg) / sizeof(wchar_t) + sizeof(procName) / sizeof(wchar_t)];
        wcscpy(str, msg);
        wcscat(str, procName);
        OutputDebugString(str);
    }
    return proc;
}
