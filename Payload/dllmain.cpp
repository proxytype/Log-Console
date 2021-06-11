// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <winternl.h>
#include "detours.h"
#include <iostream>
#include <fstream>
#include <combaseapi.h>

using namespace std;

wofstream filestream;
wstring filename;

typedef BOOL(WINAPI* realWriteConsoleW)(
          HANDLE  hConsoleOutput,
          const VOID* lpBuffer,
          DWORD   nNumberOfCharsToWrite,
          LPDWORD lpNumberOfCharsWritten,
          LPVOID  lpReserved
    );

typedef BOOL(WINAPI* realReadConsoleW)(
    HANDLE  hConsoleInput,
    LPVOID  lpBuffer,
    DWORD   nNumberOfCharsToRead,
    LPDWORD lpNumberOfCharsRead,
    LPVOID  pInputControl
    );

realWriteConsoleW _realWriteConsoleW = (realWriteConsoleW)GetProcAddress(GetModuleHandle(L"kernelbase.dll"), "WriteConsoleW");
realReadConsoleW _realReadConsoleW = (realReadConsoleW)GetProcAddress(GetModuleHandle(L"kernelbase.dll"), "ReadConsoleW");

BOOL _WriteConsoleW(HANDLE  hConsoleOutput,
    VOID* lpBuffer,
    DWORD   nNumberOfCharsToWrite,
    LPDWORD lpNumberOfCharsWritten,
    LPVOID  lpReserved) {

    BOOL val = _realWriteConsoleW(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);

    if (val == TRUE) {
        wstring f((LPWSTR)lpBuffer);
        filestream << f;
    }
    return val;
}

BOOL _ReadConsoleW(HANDLE  hConsoleInput,
    LPVOID  lpBuffer,
    DWORD   nNumberOfCharsToRead,
    LPDWORD lpNumberOfCharsRead,
    LPVOID  pInputControl) {

    BOOL val = _realReadConsoleW(hConsoleInput, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, pInputControl);

    if (val == TRUE) {
        wstring f((LPWSTR)lpBuffer);
        filestream << f;
    }

    return val;
}


void attachDetours() {

    RegDisablePredefinedCache();
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((PVOID*)&_realWriteConsoleW, _WriteConsoleW);
    DetourAttach((PVOID*)&_realReadConsoleW, _ReadConsoleW);

    DetourTransactionCommit();
}

void deAttachDetours() {

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach((PVOID*)&_realWriteConsoleW, _WriteConsoleW);
    DetourDetach((PVOID*)&_realReadConsoleW, _ReadConsoleW);

    DetourTransactionCommit();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        
        GUID guid;
        CoCreateGuid(&guid);

        OLECHAR* guidString;
        StringFromCLSID(guid, &guidString);

        filename.append(L"c:\\tmp\\");
        filename.append(guidString);
        filename.append(L".txt");

        filestream.open(filename);
        attachDetours();
        break;
    case DLL_PROCESS_DETACH:
        deAttachDetours();
        filestream.close();
        break;
    }
    return TRUE;
}

