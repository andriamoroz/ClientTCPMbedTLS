#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaEnum>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,_controller(this)
    ,_udpClient(this)

{
    ui->setupUi(this);
    setDeviceContoller();
    counter = 0;

    try{
        DataEncryption dataEncryption;
    }catch(const std::runtime_error& e){
        std::cerr << e.what() << std::endl; // prints the error message
        qDebug() << "Error in DataEncryption";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lnIPAddress_textChanged(const QString &arg1)
{
    QString state = "0";
    if (arg1 == "...") {
        state = "";
    } else {
        QHostAddress address(arg1);
        if (QAbstractSocket::IPv4Protocol == address.protocol()) {
            state = "1";
        }
    }
    ui->lnIPAddress->setProperty("state", state);
    style()->polish(ui->lnIPAddress);
}


void MainWindow::on_btnConnect_clicked()
{
    if(ui->comboBox->currentText() == "TCP")
    {
        if (_controller.isConnected()) {
            _controller.disconnect();
        } else {
            auto ip = ui->lnIPAddress->text();
            auto port = ui->spnPort->value();
            _controller.connectToDevice(ip, port);
        }
    }
    else if(ui->comboBox->currentText() == "UDP")
    {
        QHostAddress address(ui->lnIPAddress->text());
        _udpClient.sendUPD(address,ui->spnPort->value());
    }

}

void MainWindow::device_connected()
{
    ui->lstConsole->addItem("Connected To Device");
    ui->btnConnect->setText("Disconnect");
    ui->grpSendData->setEnabled(true);
}

void MainWindow::device_disconnected()
{
    ui->lstConsole->addItem("Disconnected from Device");
    ui->btnConnect->setText("Connect");
    ui->grpSendData->setEnabled(false);

}

void MainWindow::device_stateChanged(QAbstractSocket::SocketState state)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState >();
    ui->lstConsole->addItem(metaEnum.valueToKey(state));
}

void MainWindow::device_errorOccurred(QAbstractSocket::SocketError error)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError >();
    ui->lstConsole->addItem(metaEnum.valueToKey(error));

}

void MainWindow::device_dataReady(const GPRS_Send_t GPRS_Send)
{
    RecvData_t* RecvData = (RecvData_t *)GPRS_Send.Payload;;
    SYSTEMTIME st;
    uint32_t Time;
    DWORD SerNmb;
    DWORD RandomNumber;
    uint8_t Payload[1024];
    Time = GPRS_Send.Time;
    st.wYear = ((Time >> 26) & 0x03f) + 2000;
    st.wMonth = (Time >> 22) & 0x00f;
    st.wDay = (Time >> 17) & 0x01f;
    st.wHour = (Time >> 12) & 0x01f;
    st.wMinute = (Time >> 6) & 0x03f;
    st.wSecond = Time & 0x03f;

    SerNmb = GPRS_Send.SerNmb;
    //QString hexa = QString::number(SerNmb, 16);

    memcpy(Payload, GPRS_Send.Payload, sizeof(GPRS_Send.Payload));

    QString data = QString("Data: %1.%2.%3 Time: %4:%5:%6, SerNmb: %7 "
                           "\n"
                           "Command: %8, Size: %9")
                       .arg(st.wDay)
                       .arg(st.wMonth)
                       .arg(st.wYear)
                       .arg(st.wHour)
                       .arg(st.wMinute)
                       .arg(st.wSecond)
                       .arg(QString::number(SerNmb, 16))
                       .arg(RecvData->Command)
                       .arg(RecvData->Size);

    qDebug() << counter++ ;

    ui->lstConsole->addItem(QString(data));
}

void MainWindow::setDeviceContoller()
{
    connect(&_controller, &DeviceController::connected, this, &MainWindow::device_connected);
    connect(&_controller, &DeviceController::disconnected, this, &MainWindow::device_disconnected);
    connect(&_controller, &DeviceController::stateChanged, this, &MainWindow::device_stateChanged);
    connect(&_controller, &DeviceController::errorOccurred, this, &MainWindow::device_errorOccurred);
    connect(&_controller, &DeviceController::dataReady, this, &MainWindow::device_dataReady);
}

void MainWindow::configRecvData()
{
    // SOCKADDR_IN srv_addr;
    // timeval tv;
    // DWORD SN = 70; // рандомне число
    // srv_addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.1"); // рандомний адрес
    // srv_addr.sin_port = htons(7120);
    // HANDLE ghWorkHeap; // handle to the work heap
    // int iError;
    // ModuleParam_t ModuleParam = {srv_addr, tv, SN};
    // // HANDLE hMutex = pVoiceUnit->hMutexCMD;
    // //TransData_t* TransData = (TransData_t*)HeapAlloc(ghWorkHeap, HEAP_ZERO_MEMORY, sizeof(TransData_t));
    // RecvData_t* RecvData = (RecvData_t*)HeapAlloc(ghWorkHeap, HEAP_ZERO_MEMORY, sizeof(RecvData_t));
    // // TransData->Command = GET_UNIT_TYPE;
    // // TransData->Size = 0;

    // iError = VoiceUnitCMDRequestCrypt(&ModuleParam, TransData, (TCHAR*)RecvData);

    // if ((RecvData->ReturnCode & 0x80) || (iError)) {
    //     if (RecvData->ReturnCode == ERR_ENCRYPTION) {
    //        // MessageBox(hDlg, "Доступ зашифровано!", "Error", MB_ICONERROR | MB_OK);
    //     }
    //     HeapFree(ghWorkHeap, 0, TransData);
    //     HeapFree(ghWorkHeap, 0, RecvData);
    //     EndDialog(hDlg, FALSE);
    //     return TRUE;
    // }
    // pVoiceUnit->MB_Version = (RecvData->data[0]) >> 4;
    // pVoiceUnit->MB_Type = (RecvData->data[0]) & 0x0f;

}




void MainWindow::on_btnSend_clicked()
{
    //auto message = ui->lnMessage->text().trimmed();
    _controller.send();
}



