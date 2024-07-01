#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include <QHostAddress>
#include "DeviceController.h"
#include "DataEncryption.h"
#include "myDatagram.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_lnIPAddress_textChanged(const QString &arg1);

    void on_btnConnect_clicked();

    void device_connected();
    void device_disconnected();
    void device_stateChanged(QAbstractSocket::SocketState);
    void device_errorOccurred(QAbstractSocket::SocketError);
    void device_dataReady(const GPRS_Send_t);


    void on_btnSend_clicked();

private:
    Ui::MainWindow *ui;
    DeviceController _controller;
    myDatagram _udpClient;

    int counter;
    //methods
    void setDeviceContoller();
    void configRecvData();

};
#endif // MAINWINDOW_H
