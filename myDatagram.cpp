#include "myDatagram.h"

myDatagram::myDatagram(QObject *parent)
    : QObject{parent}
    //,address{address}
    //,port{port}
{
    address = QHostAddress::LocalHost;
    port = 7117;
    _udpSocket = new QUdpSocket(this);
    _udpSocket->bind(address, port);
    connect(_udpSocket, &QUdpSocket::readyRead, this, &myDatagram::readPendingDatagrams);

}

void myDatagram::sendUPD(QHostAddress address, quint16 port)
{
    QByteArray Data;
    Data.append("Hello from datagram");
    qint64 bytesSent = _udpSocket->writeDatagram(Data, address, port);
    if(bytesSent == -1){
        qDebug() << "Failed to send datagram:" << _udpSocket->errorString();
    } else {
        qDebug() << "Datagram sent to" << address << ":" << port << "Size:" << bytesSent;
    }
}

void myDatagram::readPendingDatagrams()
{
    QByteArray Buffer;
    QHostAddress sender;
    quint16 senderPort;

    // qDebug() << "Message from " << sender.toString();
    // qDebug() << "Message: " << Buffer.toStdString();
    while(_udpSocket->hasPendingDatagrams())
    {
        Buffer.resize(int(_udpSocket->pendingDatagramSize()));
        _udpSocket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);
        qDebug() << tr("Received datagram: %1" ).arg(Buffer.constData());

    }
}
