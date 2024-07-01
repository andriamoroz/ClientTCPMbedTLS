#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QTimer>
#include "mbedtl_def.h"
#include "transit_def.h"
#include "DataEncryption.h"


class DeviceController : public QObject
{
    Q_OBJECT
public:
    explicit DeviceController(QObject *parent = nullptr);
    ~DeviceController();
    void connectToDevice(QString ip, int port);
    void disconnect();
    QAbstractSocket::SocketState state();
    bool isConnected();
    void send();
    void sendUDP();

signals:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState);
    void errorOccurred(QAbstractSocket::SocketError);
    void dataReady(const GPRS_Send_t);

private slots:
    void socket_stateChanged(QAbstractSocket::SocketState state);
    void socket_readyRead();
    void onConnected();
private:
    QTcpSocket _socket;

    QString _ip;
    int _port;
    SOCKADDR_IN srv_addr;
    int _iTCP_Effort;
    QMutex mutexDecryption;
    QMutex mutexPacketSender;
    DataEncryption _dataEncryption;

    //параметри модуля
    timeval tv;
    DWORD SN;
    ModuleParam_t ModuleParam;

    HANDLE ghWorkHeap; //куча

    TransData_t* TransData; // структура для передачі даних
    RecvData_t* RecvData; // структура для отримання даних
    Packet_t *pPacket;
    GPRS_Send_t *GPRS_Send;

    BYTE res;


    void attemptConnection();
    void packetSender();
    void processData(QByteArray data);
};

#endif // DEVICECONTROLLER_H
