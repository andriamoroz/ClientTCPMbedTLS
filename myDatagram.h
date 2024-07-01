#ifndef MYDATAGRAM_H
#define MYDATAGRAM_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

class myDatagram : public QObject
{
    Q_OBJECT
public:
    explicit myDatagram(QObject *parent);

    void sendUPD(QHostAddress address, quint16 port);

public slots:
    void readPendingDatagrams();
private:
    QUdpSocket *_udpSocket;
    QHostAddress address;
    quint16 port;
};

#endif // MYDATAGRAM_H
