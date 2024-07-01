#include <QCoreApplication>
#include <windows.h>
#include <iostream>
#include "transit_def.h"
#include "mbedtl_def.h"
#include <synchapi.h>
//#include <winsock2.h>

#pragma comment(lib,"WS2_32")


int mbedtl()
{
    // Конвертування звичайного рядка в широкий рядок
    const wchar_t* wDllName = L"mbedtls.dll";

    WSADATA wsaData;

    BYTE res;
    CryptoSet_t ServerCripto;
    BYTE Server_priv_key[20];
    BYTE Server_priv_algo;
    BYTE Server_auth_key[20];
    BYTE Server_auth_algo;

    PRGETUSER hGetUser;
    PRDECRIPTIONPROC hDecryptionProc;
    PRENCRIPTIONPROC hEncryptionProc;
    HANDLE hMutexDecryption;
    HANDLE hMutexEncryption;

    SYSTEM_INFO	SystemInfo;
    HANDLE ghWorkHeap;

    // Завантаження бібліотеки з використанням LoadLibraryW
    HINSTANCE hMbedTl = LoadLibraryW(wDllName);
    if (!hMbedTl) {
        DWORD errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Could not open file!\n mbedtls.dll\nError code: %lu", errorCode);
        MessageBoxW(GetConsoleWindow(), errorMsg, L"Error", MB_ICONERROR | MB_OK);
        return -1;
    }
    hGetUser = (PRGETUSER)GetProcAddress(hMbedTl, "prGetUser");
    if (hGetUser) {
        res = (hGetUser)((char*)ServerCripto.user_name, NULL, NULL, &Server_priv_algo, Server_priv_key, &ServerCripto);
        res = (hGetUser)((char*)ServerCripto.user_name, &Server_auth_algo, Server_auth_key, NULL, NULL, &ServerCripto);
    } else {
        DWORD errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Could not GetProcAddress prGetUser\nError code: %lu", errorCode);
        MessageBoxW(GetConsoleWindow(), errorMsg, L"Error", MB_ICONERROR | MB_OK);
        FreeLibrary(hMbedTl);
        return -1;
    }
    hDecryptionProc = (PRDECRIPTIONPROC)GetProcAddress(hMbedTl, "prDecryptionProc");
    if (hDecryptionProc == NULL) {
        DWORD errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Could not GetProcAddress prDecryptionProc\nError code: %lu", errorCode);
        MessageBoxW(GetConsoleWindow(), errorMsg, L"Error", MB_ICONERROR | MB_OK);
        FreeLibrary(hMbedTl);
        return -1;
    }
    hEncryptionProc = (PRENCRIPTIONPROC)GetProcAddress(hMbedTl, "prEncryptionProc");
    if (hEncryptionProc == NULL) {
        DWORD errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Could not GetProcAddress prEncryptionProc\nError code: %lu", errorCode);
        MessageBoxW(GetConsoleWindow(), errorMsg, L"Error", MB_ICONERROR | MB_OK);
        FreeLibrary(hMbedTl);
        return -1;
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup() returned error code " << (unsigned int)GetLastError() << std::endl;
        FreeLibrary(hMbedTl);
        return -1;
    }
    GetSystemInfo(&SystemInfo);
    ghWorkHeap = HeapCreate(HEAP_NO_SERIALIZE, SystemInfo.dwPageSize*300,SystemInfo.dwPageSize*350);
    if (ghWorkHeap == NULL) {
        std::cout << "Failed to create Heap!\nError=" << (unsigned int)GetLastError() << std::endl;
        FreeLibrary(hMbedTl);
        return -1;
    }

    // Мютекси на звернення до порта Езернет і на шифрування/дешифрування
    hMutexDecryption = CreateMutex(NULL, FALSE, L"prDecryptionProc");
    if (hMutexDecryption == NULL) {
        printf("Failed to create mutex!\nError=%d", (unsigned int)GetLastError());
        FreeLibrary(hMbedTl);
        return -1;
    }
    hMutexEncryption = CreateMutex(NULL, FALSE, L"prEncryptionProc");
    if (hMutexEncryption == NULL) {
        printf("Failed to create mutex!\nError=%d", (unsigned int)GetLastError());
        FreeLibrary(hMbedTl);
        return -1;
    }

    // Виклик функції
    //ExampleFunction();
    std::cout << "hello" << std::endl;
    // Вивантаження бібліотеки
    FreeLibrary(hMbedTl);

    return 0;
}
