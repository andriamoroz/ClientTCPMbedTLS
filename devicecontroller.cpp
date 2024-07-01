#include "DeviceController.h"

DeviceController::DeviceController(QObject *parent)
    : QObject{parent}
    , _dataEncryption{this}
    , SN{SN_0x18600001} //0x18600001 = 408944641s
{
    connect(&_socket, &QTcpSocket::connected, this, &DeviceController::onConnected);
    connect(&_socket, &QTcpSocket::disconnected, this, &DeviceController::disconnected);
    connect(&_socket, &QTcpSocket::errorOccurred, this, &DeviceController::errorOccurred);
    connect(&_socket, &QTcpSocket::stateChanged, this, &DeviceController::socket_stateChanged);
    connect(&_socket, &QTcpSocket::readyRead, this, &DeviceController::socket_readyRead);

    SOCKADDR_IN srv_addr;
    srv_addr.sin_addr.S_un.S_addr = inet_addr(_ip.toStdString().c_str()); // рандомний адрес
    srv_addr.sin_port = htons(_port);

    ModuleParam = {srv_addr, tv, SN};

    //виділення кучі
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    ghWorkHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE , SystemInfo.dwPageSize,SystemInfo.dwPageSize*2);

    TransData = (TransData_t*)HeapAlloc(ghWorkHeap, HEAP_ZERO_MEMORY, sizeof(TransData_t));
    if(TransData == NULL)
        qDebug() << "TransData is NULL";
    RecvData = (RecvData_t*)HeapAlloc(ghWorkHeap, HEAP_ZERO_MEMORY, sizeof(RecvData_t));
    if(RecvData == NULL)
        qDebug() << "RecvData is NULL";
}

DeviceController::~DeviceController()
{
    if(TransData){
        HeapFree(ghWorkHeap, 0, TransData);
    }
    if(RecvData){
        HeapFree(ghWorkHeap, 0, RecvData);
    }
    //звільнення кучі
    if(ghWorkHeap != NULL){
        HeapDestroy(ghWorkHeap);
    }
}

void DeviceController::connectToDevice(QString ip, int port)
{
    if (_socket.isOpen()) {
        if (ip == _ip && port == _port) {
            return;
        }
        _socket.close();
    }
    _ip = ip;
    _port = port;
    //_socket.connectToHost(_ip, _port);
    _iTCP_Effort = 3;
    attemptConnection();
}
// void DeviceController::connected()
// {
//     _socket.write("Hello from client");
// }

void DeviceController::disconnect()
{
    _socket.close();
}

QAbstractSocket::SocketState DeviceController::state()
{
    return _socket.state();
}

bool DeviceController::isConnected()
{
    return _socket.state() == QAbstractSocket::ConnectedState;
}

void DeviceController::send()
{
    if(_socket.isOpen())
    {
        packetSender();
    }

}


void DeviceController::socket_stateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState) {
        _socket.close();
    }
    emit stateChanged(state);
}

void DeviceController::socket_readyRead()
{


    QByteArray data = _socket.readAll();

 //на 59 крузі програма зависає
    processData(data);
    qDebug() << "Data ready emit";
    //emit dataReady(*GPRS_Send);

}

void DeviceController::onConnected()
{
    emit connected();
    qDebug() << "Connected to server. IP: " << _ip << " port: "<< _port;

    packetSender();
}

void DeviceController::attemptConnection()
{
    if(_iTCP_Effort > 0){
        _iTCP_Effort--;
        _socket.connectToHost(_ip,_port);
        QTimer::singleShot(3000, this, [this](){
            if(_socket.state() != QAbstractSocket::ConnectedState){
                qDebug() << "Connection failed. Retrying...";
                attemptConnection();
            }
        });
    }
    else{
        qDebug() << "Connection timeout";
    }
}

void DeviceController::packetSender()
{
    QMutexLocker locker(&mutexPacketSender);
    qDebug() << "inside packetSender";

    if(ghWorkHeap == NULL)
    {
        qDebug() << "Error creating heap";
        return;
    }
    // встановлюю тип команди і розмір в TransData
    //TransData->Command = GET_UNIT_TYPE;
    TransData->Command = COMMAND;
    TransData->Size = 0;

    TCHAR szBuffer[MAX_PACKET_SIZE];
    BYTE bVar;
    SYSTEMTIME st;
    char *cp;
    int iRemaining;

    pPacket = (Packet_t*)&szBuffer[0];
    GPRS_Send = (GPRS_Send_t*)pPacket->Data;
    srand((unsigned)time(NULL));
    GPRS_Send->RandomNumber = rand();
    memcpy(GPRS_Send->Payload, TransData, TransData->Size + 2 + 1); // +2 for command and size, +1 for checksum
    GetLocalTime(&st);
    bVar = (BYTE)st.wYear - 2000;
    GPRS_Send->Time = bVar << 26;
    bVar = (BYTE)st.wMonth;
    GPRS_Send->Time |= bVar << 22;
    bVar = (BYTE)st.wDay;
    GPRS_Send->Time |= bVar << 17;
    bVar = (BYTE)st.wHour;
    GPRS_Send->Time |= bVar << 12;
    bVar = (BYTE)st.wMinute;
    GPRS_Send->Time |= bVar << 6;
    bVar = (BYTE)st.wSecond;
    GPRS_Send->Time |= bVar;
    GPRS_Send->SerNmb = ModuleParam.SerNmb;
    pPacket->Size = 4 + 4 + 4 + TransData->Size + 2 + 1; // 4 for RandomNumber, 4 for Time, 4 for SerNmb, 2 for Command and Size, 1 for checksum

    qDebug() << "pPacket:" << pPacket->Data;
    if (WaitForSingleObject(_dataEncryption.hMutexEncryption, 2000) != WAIT_TIMEOUT) {
        res = (_dataEncryption.hEncryptionProc)(pPacket, _dataEncryption.Server_auth_algo,
                                                _dataEncryption.Server_auth_key,
                                                _dataEncryption.Server_priv_algo,
                                                _dataEncryption.Server_priv_key,
                                                &_dataEncryption.ServerCripto); // Encrypt the packet
        ReleaseMutex(_dataEncryption.hMutexEncryption);
    }
    else {
        qDebug() << "Error waiting for mutex in packetSender";
    }
    qDebug() << "pPacket:" << pPacket->Data;

    cp = (char*)pPacket;
    iRemaining = pPacket->Size + 2;

    QByteArray pocketData;
    pocketData.append(reinterpret_cast<const char *>(pPacket), pPacket->Size +2);

    qint64 bytesWritten = 0;

    while(bytesWritten  < pocketData.size())
    {
        qint64 written = _socket.write(pocketData.constData() + bytesWritten, pocketData.size() - bytesWritten);
        if(written == -1)
        {
            qDebug() << "Error writing data to socket";
            break;
        }
        bytesWritten += written;
        qDebug() << "Відправлено " << written << " байтів, всього відправлено" << bytesWritten;
    }


}

void DeviceController::processData(QByteArray data)
{
    SYSTEMTIME st;
    uint32_t dwRcvTime;
    //QByteArray buffer;

    //buffer.clear();
    //buffer.append(data);

    qDebug() << "buffer.size(): " << data.size();
    qDebug() << "sizeof(Packet_t)" << sizeof(Packet_t);
   // while(buffer.size() < sizeof(Packet_t)){
        qDebug() << "Зайшов в цикл в processData";
        //pPacket = reinterpret_cast<Packet_t*>(data[0]);
        pPacket = reinterpret_cast<Packet_t*>(data.data());
        //pPacket = (Packet_t*)&data[0];
        int packetSize = 2+ pPacket->Size;

        // if(data.size() < packetSize){
        //     //break;
        //     buffer.clear();
        //     return;
        // }

        qDebug() << "packetSize: " << packetSize;
        QMutexLocker locker(&mutexDecryption);
        qDebug() <<  "після локера";
        int res = (_dataEncryption.hDecryptionProc)(pPacket, _dataEncryption.Server_auth_algo
                                                    , _dataEncryption.Server_auth_key
                                                    , _dataEncryption.Server_priv_algo
                                                    , _dataEncryption.Server_priv_key
                                                    , &_dataEncryption.ServerCripto);
        qDebug() << "Після _dataEncryption // res: " << res;
        if (res != ERR_OK) {
            qDebug() << "Error decrypting packet: " << res;
            return;
        }

        qDebug() << "перед GPRS_Send";
        //buffer.remove(0, packetSize); // видаляємо з буфера оброблений пакет
        //buffer.clear(); //повністю очищаємо буфер
        //GPRS_Send = (GPRS_Send_t *)pPacket->Data;
        GPRS_Send = reinterpret_cast<GPRS_Send_t*>(pPacket->Data);
        qDebug() << "після GPRS_Send  GPRS_Send->SerNmb: " << GPRS_Send->SerNmb;
        dwRcvTime = GPRS_Send->Time;

         st.wYear = ((dwRcvTime >> 26) & 0x03f) + 2000;
         st.wMonth = (dwRcvTime >> 22) & 0x00f;
         st.wDay = (dwRcvTime >> 17) & 0x01f;
         st.wHour = (dwRcvTime >> 12) & 0x01f;
         st.wMinute = (dwRcvTime >> 6) & 0x03f;
         st.wSecond = dwRcvTime & 0x03f;
         qDebug() << "Recive time "  << st.wDay <<
                                 "." << st.wMonth <<
                                 "." << st.wYear<<
                                 " " << st.wHour <<
                                 ":" << st.wMinute <<
                                 ":" << st.wSecond;

        //RecvData = (RecvData_t *)GPRS_Send->Payload;
        RecvData = reinterpret_cast<RecvData_t*>(GPRS_Send->Payload);
        qDebug() << "RecvData->Size: " << RecvData->Size;
        qDebug() << "RecvData->Command: " << RecvData->Command;
        qDebug() << "RecvData->ReturnCode: " << RecvData->ReturnCode;
        BoxStatus_t BoxStatus; // = (UnitMem_t*)RecvData->data;
        BoxStatus.ConfigMega.d8 = RecvData->data[0];
        BoxStatus.ResDD.d8 = RecvData->data[1];
        memcpy(&BoxStatus.BoxState[0], RecvData->data + 2, NMB_BOX * sizeof(BoxState_t));
        memcpy(&BoxStatus.UPS_Coeff, RecvData->data + 2 + (NMB_BOX * sizeof(BoxState_t)), sizeof(UPS_Coeff_t));
        emit dataReady(*GPRS_Send);
        //отримую якісь леві дані, розібратсия чого не відповідають даним в конфігураторі

        qDebug() << "pBoxStatus->ConfigMega.b.NmbBox: " << BoxStatus.ConfigMega.b.NmbBox;
        qDebug() << "pBoxStatus->ConfigMega.b.UseFM: " << BoxStatus.ConfigMega.b.UseFM;

        qDebug() << "pBoxStatus->ResDD.b.Inp1: " << BoxStatus.ResDD.b.Inp1;
        qDebug() << "pBoxStatus->ResDD.b.Inp2: " << BoxStatus.ResDD.b.Inp2;
        qDebug() << "pBoxStatus->ResDD.b.Inp3: " << BoxStatus.ResDD.b.Inp3;
        qDebug() << "pBoxStatus->ResDD.b.Inp4: " << BoxStatus.ResDD.b.Inp4;
        qDebug() << "pBoxStatus->ResDD.b.Inp5: " << BoxStatus.ResDD.b.Inp5;
        qDebug() << "pBoxStatus->ResDD.b.Inp6: " << BoxStatus.ResDD.b.Inp6;

        qDebug() << "pBoxStatus->BoxState[0].AmpStat: " << BoxStatus.BoxState[0].AmpStat;
        qDebug() << "pBoxStatus->BoxState[0].BatTemp: " << BoxStatus.BoxState[0].BatTemp;
        qDebug() << "pBoxStatus->BoxState[0].BatVoltage: " << BoxStatus.BoxState[0].BatVoltage;

        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.DoorOpen: " << BoxStatus.BoxState[0].BoxStat.b.DoorOpen;
        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.LowBatt: " << BoxStatus.BoxState[0].BoxStat.b.LowBatt;
        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.Out1: " << BoxStatus.BoxState[0].BoxStat.b.Out1;
        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.RS485Error: " << BoxStatus.BoxState[0].BoxStat.b.RS485Error;
        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.TempError: " << BoxStatus.BoxState[0].BoxStat.b.TempError;
        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.SupplyDet: " << BoxStatus.BoxState[0].BoxStat.b.SupplyDet;
        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.TestON: " << BoxStatus.BoxState[0].BoxStat.b.TestON;
        qDebug() << "pBoxStatus->BoxState[0].BoxStat.b.Relay1: " << BoxStatus.BoxState[0].BoxStat.b.Relay1;

        qDebug() << "pBoxStatus->UPS_Coeff.Coeff_I: " << BoxStatus.UPS_Coeff.Coeff_I;
        qDebug() << "pBoxStatus->UPS_Coeff.Coeff_V: " << BoxStatus.UPS_Coeff.Coeff_V;
        qDebug() << "pBoxStatus->UPS_Coeff.Coeff_T: " << BoxStatus.UPS_Coeff.Coeff_T;








        //треба тільки BoxStatus_t
     //   break;18600001
    //}


}










