#include <QtGui>
#include "tripplanner.h"

TripPlanner::TripPlanner(QWidget *parent)
    : QDialog(parent)
{
    socket_available = 0;
    setupUi(this);

    searchButton = buttonBox->addButton(tr("&Search"),
                                        QDialogButtonBox::ActionRole);
    stopButton = buttonBox->addButton(tr("S&top"),
                                      QDialogButtonBox::ActionRole);
    stopButton->setEnabled(false);
    buttonBox->button(QDialogButtonBox::Close)->setText(tr("&Quit"));

    QDateTime dateTime = QDateTime::currentDateTime();
    dateEdit->setDate(dateTime.date());
    timeEdit->setTime(QTime(dateTime.time().hour(), 0));

    progressBar->hide();
    progressBar->setSizePolicy(QSizePolicy::Preferred,
                               QSizePolicy::Ignored);

    tableWidget->verticalHeader()->hide();
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(searchButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopSearch()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

//    connect(&pn532.tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
//    connect(&tcpSocket, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()));
//    connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(updateTableWidget()));
//    connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));

//    connect(&pn532.tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
//    connect(&pn532.tcpSocket, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()));
//    connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(updateTableWidget()));
//    connect(&pn532.tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(&pn532, SIGNAL(getFrame(quint8*,int)), this, SLOT(addList(quint8*,int)));
}

void TripPlanner::connectToServer()
{
#if 1
//    tcpSocket.connectToHost(QHostAddress::QHostAddress("132.147.160.1"),8888);
#elif 0
    tcpSocket.connectToHost(QHostAddress::LocalHost, 6178);
#else
    tcpSocket.connectToHost("tripserver.zugbahn.de", 6178);
#endif

    pn532.connectToServer("192.168.2.1", 8888);
    tableWidget->setRowCount(0);
    searchButton->setEnabled(false);
    stopButton->setEnabled(true);
    statusLabel->setText(tr("Connecting to server..."));
    progressBar->show();

    nextBlockSize = 0;
}

void TripPlanner::sendRequest()
{
//    pn532.startWork(PN532_THREAD::WORK_MODE_READ_MIFARE_M1FS50);
//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_3);
//    out << quint16(0) << quint8('S') << fromComboBox->currentText()
//        << toComboBox->currentText() << dateEdit->date()
//        << timeEdit->time();

//    if (departureRadioButton->isChecked()) {
//        out << quint8('D');
//    } else {
//        out << quint8('A');
//    }
//    out.device()->seek(0);
//    out << quint16(block.size() - sizeof(quint16));
//    tcpSocket.write(block);

//    statusLabel->setText(tr("Sending request..."));
/*    quint8 buf[300];
    const quint8 ack[6] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    int i;

    pn532_wake_up();

    buf[0] = 0xD4;
    buf[1] = 0x02;
    pn532_send_pkt(buf, 2);

    statusLabel->setText(tr("Get Version..."));

    switch( pn532_receive_pkt(buf, 6, 300) ){
        case -1:
            statusLabel->setText(tr("read timeout"));
            break;
        case -2:
            statusLabel->setText(tr("read available 0"));
            break;
        case -3:
            statusLabel->setText(tr("readRawData error"));
            break;
    }
    for(i=0; i<6; i++){
        if(buf[i]!=ack[i]){
            break;
        }
    }
    if(i==6){
        statusLabel->setText(tr("Receive ack"));
    }
    addList(buf, 6);

    pn532_receive_pkt(buf, 13, 30);
    addList(buf, 13);
*/
//    if()
//    tcpSocket.waitForReadyRead(300);
}

void TripPlanner::addList(quint8 *buf, int len)
{
    const char hex_tab[]="0123456789ABCDEF";
    char buf_hex[300];
    quint8 tmp[300];
    pn532.readSemBuf(tmp, len);
    int row = tableWidget->rowCount();
    tableWidget->setRowCount(row + 1);

    buf_hex[3*len] = 0;
    for(int i=0; i<len; i++){
        buf_hex[3*i] = hex_tab[(tmp[i]>>4)];
        buf_hex[3*i+1] = hex_tab[(tmp[i]&0x0F)];
        buf_hex[3*i+2] = ' ';

//        buf_hex[3*i] = hex_tab[(buf[i]>>4)];
//        buf_hex[3*i+1] = hex_tab[(buf[i]&0x0F)];
//        buf_hex[3*i+2] = ' ';
    }
    tableWidget->setItem(row, 0, new QTableWidgetItem(tr("%1").arg(row)));
    tableWidget->setItem(row, 1, new QTableWidgetItem(buf_hex));

    tableWidget->resizeColumnsToContents();
}

void TripPlanner::updateTableWidget()
{
//    char hex_tab[]="0123456789ABCDEF";
//    char buf_hex[300];
//    QDataStream in(&pn532.tcpSocket);
//    in.setVersion(QDataStream::Qt_4_3);
//    qint64 size, size_copy;
//    quint8 buf[300];
//    size = pn532.tcpSocket.bytesAvailable();
//    size_copy = size;

//    in.readRawData((char *)buf,size);
//    int row = tableWidget->rowCount();

//    tableWidget->setRowCount(row + 1);

//    buf_hex[3*size_copy] = 0;
//    for(int i=0; i<size_copy; i++){
//        buf_hex[3*i] = hex_tab[(buf[i]>>4)];
//        buf_hex[3*i+1] = hex_tab[(buf[i]&0x0F)];
//        buf_hex[3*i+2] = ' ';
//    }

//    tableWidget->setItem(row, 0, new QTableWidgetItem(tr("%1").arg(row)));
//    tableWidget->setItem(row, 1, new QTableWidgetItem(buf_hex));

    //statusLabel->setText(tr("Reciev character"));
/*
    forever {
        int row = tableWidget->rowCount();

        if (nextBlockSize == 0) {
            if (tcpSocket.bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }

        if (nextBlockSize == 0xFFFF) {
            closeConnection();
            statusLabel->setText(tr("Found %1 trip(s)").arg(row));
            break;
        }

        if (tcpSocket.bytesAvailable() < nextBlockSize)
            break;

        QDate date;
        QTime departureTime;
        QTime arrivalTime;
        quint16 duration;
        quint8 changes;
        QString trainType;

        in >> date >> departureTime >> duration >> changes >> trainType;
        arrivalTime = departureTime.addSecs(duration * 60);

        tableWidget->setRowCount(row + 1);

        QStringList fields;
        fields << date.toString(Qt::LocalDate)
               << departureTime.toString(tr("hh:mm"))
               << arrivalTime.toString(tr("hh:mm"))
               << tr("%1 hr %2 min").arg(duration / 60)
                                    .arg(duration % 60)
               << QString::number(changes)
               << trainType;
        for (int i = 0; i < fields.count(); ++i)
            tableWidget->setItem(row, i,
                                 new QTableWidgetItem(fields[i]));
        nextBlockSize = 0;
    }
    */

}

void TripPlanner::stopSearch()
{
    statusLabel->setText(tr("Search stopped"));
    closeConnection();
}

void TripPlanner::connectionClosedByServer()
{
    if (nextBlockSize != 0xFFFF)
        statusLabel->setText(tr("Error: Connection closed by server"));
    closeConnection();
}
#include <iostream>
void TripPlanner::error(QAbstractSocket::SocketError err)
{
//    statusLabel->setText(pn532.tcpSocket.errorString());
//    std :: cerr << qPrintable(pn532.tcpSocket.errorString());
    closeConnection();
}

void TripPlanner::closeConnection()
{
//    pn532.startWork(PN532_THREAD::WORK_MODE_IDLE);
//    pn532.tcpSocket.waitForBytesWritten();
//    pn532.tcpSocket.close();
    pn532.closeConnection();
    searchButton->setEnabled(true);
    stopButton->setEnabled(false);
    progressBar->hide();
}

/*
void TripPlanner :: pn532_wake_up()
{
    QByteArray block;
    quint8 buf[5]={0x55, 0x55, 0x00, 0x00, 0x00};
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    out.writeRawData((const char *)buf, 5);
    pn532.tcpSocket.write(block);
}

void TripPlanner :: pn532_send_pkt(quint8 *cmd, int len)
{
    uint8_t buf[64], checksum;
    int i;
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0xFF;
    buf[3] = (uint8_t)len;
    buf[4] = ~buf[3] + 1;
    checksum = 0;
    for(i=0; i<len; i++) {
        buf[4+i+1] = cmd[i];
        checksum+=cmd[i];
    }
    buf[4+len+1] = (~checksum+1);
    buf[4+len+1+1] = 0x00;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_4_3);
    out.writeRawData((const char *)buf, 4+len+1+1+1);

    pn532.tcpSocket.write(block);
}

int TripPlanner :: pn532_receive_pkt(quint8 *cmd, int len, int timeout)
{
    int received_bytes_count = 0;
    int available_bytes_count = 0;
    const int expected_bytes_count = (int)len;
    int read_size = 0;
    int res;

    QDataStream in(&pn532.tcpSocket);
    in.setVersion(QDataStream::Qt_4_3);

    do{
        if(socket_available >= expected_bytes_count){
            res = in.readRawData((char *)cmd, expected_bytes_count);
            if(res <= 0){
                return -1;
            }
            socket_available -= expected_bytes_count;
            received_bytes_count = res;
        }else if(socket_available){
            res = in.readRawData((char *)cmd, socket_available);
            if(res <= 0){
                return -1;
            }
            socket_available = 0;
            received_bytes_count += res;
        }else{
            if(pn532.tcpSocket.waitForReadyRead(timeout) == 0){
                if(received_bytes_count){
                    return received_bytes_count;
                }
                return -1;
            }

            available_bytes_count = pn532.tcpSocket.bytesAvailable();
            if(available_bytes_count == 0){
                return -2;
            }

            if(available_bytes_count >= (expected_bytes_count - received_bytes_count)){
                read_size = expected_bytes_count - received_bytes_count;
                socket_available = available_bytes_count - read_size;
            }else{
                read_size = available_bytes_count;
            }

            res = in.readRawData((char *)cmd, read_size);
            if(res <= 0){
                return -3;
            }
            received_bytes_count += res;

        }
    }while(expected_bytes_count > received_bytes_count);

    return received_bytes_count;
}*/


























