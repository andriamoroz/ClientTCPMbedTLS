#ifndef DATAENCRYPTION_H
#define DATAENCRYPTION_H

#include <QObject>
#include <QCoreApplication>
#include <windows.h>
#include <iostream>
#include <synchapi.h>
#include "mbedtl_def.h"
#include "transit_def.h"


const char UserName[] = "Artem";
const char EngineID[] = "012345678901";
const char AuthPassword[] = "Artem1";
const char PrivPassword[] = "Artem2";


class DataEncryption : public QObject
{
    Q_OBJECT
public:
    explicit DataEncryption(QObject *parent = nullptr);
    ~DataEncryption();

    const wchar_t* wDllName;

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

    HINSTANCE hMbedTl;

    WSADATA wsaData;

signals:


};

#endif // DATAENCRYPTION_H
