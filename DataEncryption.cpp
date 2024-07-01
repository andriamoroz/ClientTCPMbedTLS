
#include "DataEncryption.h"

#pragma comment(lib,"WS2_32")

DataEncryption::DataEncryption(QObject *parent)
    : QObject{parent}


{

    strcpy_s((char*)ServerCripto.user_name, sizeof(ServerCripto.user_name), UserName);
    strcpy_s((char*)ServerCripto.engine_id, sizeof(ServerCripto.engine_id), EngineID);
    strcpy_s((char*)ServerCripto.auth_password, sizeof(ServerCripto.auth_password), AuthPassword);
    strcpy_s((char*)ServerCripto.priv_password, sizeof(ServerCripto.priv_password), PrivPassword);
    ServerCripto.security_level = PRIV_FLAG | AUTH_FLAG;
    ServerCripto.auth_protocol = AUTH_ALGO_SHA;
    ServerCripto.priv_protocol = PRIV_ALGO_AES;

    wDllName = L"mbedtls.dll";

    hMbedTl = LoadLibraryW(wDllName);
    if (!hMbedTl) {
        DWORD errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Could not open file!\n mbedtls.dll\nError code: %lu", errorCode);
        MessageBoxW(GetConsoleWindow(), errorMsg, L"Error", MB_ICONERROR | MB_OK);
        //return -1;
        throw std::runtime_error("Could not open file!\n mbedtls.dll\nError code: " + std::to_string(errorCode));
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
        //return -1;
        throw std::runtime_error("Could not GetProcAddress prGetUser\nError code: " + std::to_string(errorCode));
    }
    hDecryptionProc = (PRDECRIPTIONPROC)GetProcAddress(hMbedTl, "prDecryptionProc");
    if (hDecryptionProc == NULL) {
        DWORD errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Could not GetProcAddress prDecryptionProc\nError code: %lu", errorCode);
        MessageBoxW(GetConsoleWindow(), errorMsg, L"Error", MB_ICONERROR | MB_OK);
        FreeLibrary(hMbedTl);
        throw std::runtime_error("Could not GetProcAddress prDecryptionProc\nError code: " + std::to_string(errorCode));
    }
    hEncryptionProc = (PRENCRIPTIONPROC)GetProcAddress(hMbedTl, "prEncryptionProc");
    if (hEncryptionProc == NULL) {
        DWORD errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, 256, L"Could not GetProcAddress prEncryptionProc\nError code: %lu", errorCode);
        MessageBoxW(GetConsoleWindow(), errorMsg, L"Error", MB_ICONERROR | MB_OK);
        FreeLibrary(hMbedTl);
        throw std::runtime_error("Could not GetProcAddress prEncryptionProc\nError code: " + std::to_string(errorCode));
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        DWORD errorCode = GetLastError();
        std::cout << "WSAStartup() returned error code " << (unsigned int)GetLastError() << std::endl;
        FreeLibrary(hMbedTl);
        throw std::runtime_error("WSAStartup() returned error code  " + std::to_string(errorCode));
    }
    GetSystemInfo(&SystemInfo);
    ghWorkHeap = HeapCreate(HEAP_NO_SERIALIZE, SystemInfo.dwPageSize*300,SystemInfo.dwPageSize*350);
    if (ghWorkHeap == NULL) {
        DWORD errorCode = GetLastError();
        std::cout << "Failed to create Heap!\nError = " << (unsigned int)GetLastError() << std::endl;
        FreeLibrary(hMbedTl);
        throw std::runtime_error("Failed to create Heap!\nError = " + std::to_string(errorCode));
    }

    // Мютекси на звернення до порта Езернет і на шифрування/дешифрування
    hMutexDecryption = CreateMutex(NULL, FALSE, L"prDecryptionProc");
    if (hMutexDecryption == NULL) {
        DWORD errorCode = GetLastError();
        std::cout << "Failed to create mutex!\nError=%d" << (unsigned int)GetLastError() << std::endl;
        FreeLibrary(hMbedTl);
        throw std::runtime_error("Failed to create mutex!\nError=%d " + std::to_string(errorCode));
    }
    hMutexEncryption = CreateMutex(NULL, FALSE, L"prEncryptionProc");
    if (hMutexEncryption == NULL) {
        DWORD errorCode = GetLastError();
        std::cout << "Failed to create mutex!\nError=%d" << (unsigned int)GetLastError() << std::endl;
        FreeLibrary(hMbedTl);
        throw std::runtime_error("Failed to create mutex!\nError=%d " + std::to_string(errorCode));
    }

    // Виклик функції
    //ExampleFunction();
    std::cout << "hello" << std::endl;
    // Вивантаження бібліотеки

}

DataEncryption::~DataEncryption()
{
    FreeLibrary(hMbedTl);
}
