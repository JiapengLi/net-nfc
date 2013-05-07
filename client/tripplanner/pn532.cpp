#include <QtNetwork>
#include <QtCore>
#include <QtGlobal>
#include <QMetaType>
#include <iostream>
#include "pn532.h"

const uint8_t PN532_THREAD :: ack[6]={0x00,  0x00, 0xFF, 0x00, 0xFF, 0x00};
const int PN532_THREAD :: BufferSize = 1024;

PN532_THREAD :: PN532_THREAD()
{
    freeSpace = new QSemaphore(BufferSize);
    usedSpace = new QSemaphore(0);
    consumerCnt = 0;
    producerCnt = 0;

    socket_available = 0;
    work_mode = WORK_MODE_IDLE;
    start();
}

void PN532_THREAD :: semBufProducer(quint8 data)
{
    freeSpace->acquire();
//    std :: cerr << "write buf\r\n";
    semBuffer[producerCnt++] = data;
    if(producerCnt == BufferSize){
        producerCnt = 0;
    }
    usedSpace->release();
}

quint8 PN532_THREAD :: semBufConsumer()
{
    quint8 ret;
    usedSpace->acquire();
//    std :: cerr << "read buf\r\n";
    ret = semBuffer[consumerCnt++];
    if(consumerCnt == BufferSize){
        consumerCnt = 0;
    }
    freeSpace->release();

    return ret;
}

void PN532_THREAD :: writeSemBuf(quint8 *buf, int len)
{
    int i;
    for(i=0; i<len; i++){
        semBufProducer(buf[i]);
    }
}

void PN532_THREAD :: readSemBuf(quint8 *buf, int len)
{
    int i;
    for(i=0; i<len; i++){
        buf[i] = semBufConsumer();
    }
}

int PN532_THREAD :: setWorkMode(workMode_t mode)
{
    work_mode = mode;
    return 0;
}

void PN532_THREAD::connectToServer(QString addr, quint16 port)
{
    std :: cerr << "connect to server\r\n";
    hostaddr = addr;
    portnum = port;
    setWorkMode(WORK_MODE_CONNECT_TO_HOST);
    if(isRunning()){
        return;
    }
    start();
}

void PN532_THREAD ::begin()
{
    emit connected();
    setWorkMode(WORK_MODE_BEGIN);
    std::cerr << "begin\r\n";
}

void PN532_THREAD :: error(QAbstractSocket::SocketError err)
{
    if(err == QAbstractSocket::SocketTimeoutError){
        /** ignore timeout error */
        return;
    }
    emit errors();
    std :: cerr << "error ocurred\r\n";
    std :: cerr << qPrintable(tcpSocket->errorString()) << "\r\n";
}

void PN532_THREAD :: closeConnection()
{
    setWorkMode(WORK_MODE_STOP);
}

void PN532_THREAD :: connectionClosedByServer()
{
    emit disconnected();
    std::cerr << "Error: Connection closed by server\r\n";
//    closeConnection();
}

void PN532_THREAD :: run()
{
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    tcpSocket = new QTcpSocket;
//    QTcpSocket tcpSocket0;
    bool flag;


    connect(tcpSocket, SIGNAL(connected()), this, SLOT(begin()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),\
            this, SLOT(error(QAbstractSocket::SocketError)), Qt::DirectConnection);
    flag = false;
    while(!flag){
        if(tcpSocket->state() != QAbstractSocket::UnconnectedState){
            if(tcpSocket->waitForDisconnected(10)){
                tcpSocket->close();
                flag = true;
            }
        }
        switch(work_mode){
        case WORK_MODE_IDLE:
            break;
        case WORK_MODE_CONNECT_TO_HOST:
            work_mode = WORK_MODE_IDLE;
            tcpSocket->connectToHost(QHostAddress::QHostAddress(hostaddr),portnum);
            if (tcpSocket->waitForConnected(1000)){
                 qDebug("Connected!");
            }else{
                 qDebug("Unconnected!");
            }

            break;
        case WORK_MODE_BEGIN:
           // work_mode = WORK_MODE_IDLE;
            work_mode = WORK_MODE_READ_MIFARE_M1FS50;
            std::cerr << "work mode begin\r\n";
            wake_up();

            GetFirmwareVersion(buf);
            SAMConfiguration(buf);

            break;
        case WORK_MODE_READ_MIFARE_M1FS50:
            InListPassiveTarget(buf);
            break;
        case WORK_MODE_STOP:
            work_mode = WORK_MODE_IDLE;
//            tcpSocket->waitForBytesWritten();
            tcpSocket->close();
            flag = true;
            break;
        }
    }
    std :: cerr << "thread quit\r\n";
}

int PN532_THREAD :: SAMConfiguration(quint8 *buf, uint8_t mode, uint8_t timeout, uint8_t irq)
{
    buf[0] = 0xD4;
#if 1
    buf[1] = CmdSAMConfiguration;
#else
    buf[1] = 0x02;
#endif
    buf[2] = mode;
    buf[3] = timeout;
    buf[4] = irq;

    if(send_pkt_wait_ack(buf, 5) <0){
        return -1;
    }
    if(receive_pkt(buf, 2+7, PN532_READTIMEOUT) != 2+7){
        return -2;
    }
    if(frame_check(buf, 2+7)){
        puthex(buf, 2+7);

        writeSemBuf(buf, 2+7);

        emit getFrame(buf, 2+7);
    }
    return 0;
}

int PN532_THREAD :: GetFirmwareVersion(quint8 *buf)
{
    buf[0] = 0xD4;
    buf[1] = CmdGetFirmwareVersion;

    if(send_pkt_wait_ack(buf, 2) <0){
        return -1;
    }

    if(receive_pkt(buf, 6+7, PN532_READTIMEOUT) != 6+7){
        return -2;
    }
    if(frame_check(buf, 6+7)){
        puthex(buf, 6+7);

        writeSemBuf(buf, 6+7);

        emit getFrame(buf, 6+7);
    }
    return 0;
}

int PN532_THREAD :: InListPassiveTarget(quint8 *buf, uint8_t MaxTg, uint8_t BrTy, uint8_t *InitiatorData, uint8_t len)
{

    int i, res, pkt_len, id_len;

    buf[0] = 0xD4;
    buf[1] = CmdInListPassiveTarget;
    buf[2] = MaxTg;
    buf[3] = BrTy;
    if(InitiatorData != NULL && len > 0){
        for(i=0; i<len; i++){
            buf[4] = InitiatorData[i];
        }
    }

    if(send_pkt_wait_ack(buf, 4) <0){
        return -1;
    }

    res = receive_pkt(buf, 64, 30);
    if( res<=0 ){
        return -2;
    }
    pkt_len = frame_check(buf, res);
    if(pkt_len < 0){
        std::cerr << "frame check error" << pkt_len;
        return -3;
    }

    if(buf[6] != (CmdInListPassiveTarget+1)){

        return -4;
    }

    switch(BrTy){
    case PN532_BRTY_ISO14443A:
        id_len = buf[12];
        writeSemBuf(buf+13, id_len);
        emit getFrame(buf+13, id_len);
        emit getId(buf+13, id_len);
        break;
    case PN532_BRTY_ISO14443B:
        break;
    case PN532_BRTY_212KBPS:
        break;
    case PN532_BRTY_424KBPS:
        break;
    case PN532_BRTY_JEWEL:
        break;
    }
//    emit getFrame(buf+13, id_len);

    return 0;
}

int PN532_THREAD :: frame_check(uint8_t *buf, int len)
{
    int i,pkt_len;
    uint8_t checksum;

    if(len < 9){
        /** frame is too short */
        return -1;
    }
    if(buf[0] || buf[1]){
        /** fame preamble error */
        return -2;
    }
    if( (buf[1] != 0x00) || (buf[2] != 0xFF) ){
        /** frame start code error */
        return -3;
    }
    if(buf[3] < 2){
        /** frame len error */
        return -4;
    }
    if( (buf[3]+buf[4])&0xFF ){
        /** frame len checksum error */
        std::cerr << "frame error" << buf[3] << buf[4] << (buf[3]+buf[4]) << "\r\n";
        return -5;
    }
    if(len<(buf[3]+7)){
        /** frame is short for length */
        return -6;
    }

    pkt_len = buf[3];
    checksum = 0;

    for(i=0; i<pkt_len+1; i++){
        checksum += buf[5+i];
    }

    if(checksum || (buf[5]!= 0xD5) ){
        /** frame payload checksum error */
        return -7;
    }

    return pkt_len;
}

int PN532_THREAD :: frame_analyse(uint8_t *buf, int pkt_len)
{
    switch(buf[6]){
        case CmdInListPassiveTarget:
        switch(pkt_len){
            case 2:

                break;
        }
            break;

    }
    return 0;
}

void PN532_THREAD :: wake_up()
{
    QByteArray block;
    quint8 buf[5]={0x55, 0x55, 0x00, 0x00, 0x00};
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    out.writeRawData((const char *)buf, 5);
    tcpSocket->write(block);
}

int PN532_THREAD :: send_pkt(quint8 *cmd, int len)
{
    uint8_t buf[64], checksum;
    int i, res;
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

    res = tcpSocket->write(block);
    if((res-7) == len){
//        std::cerr << "write num:" << res << "\r\n";
        return len;
    }
    tcpSocket->waitForBytesWritten();
    return -1;
}

int PN532_THREAD :: wait_ack()
{
    quint8 buf[6];
    int i=0;
    if(receive_pkt(buf, 6, PN532_READTIMEOUT) == 6){
//        std::cerr << "ack received\r\n";
        for(i=0; i<6; i++){
            if(buf[i]!=ack[i]){
                break;
            }
        }
        if(i==6){
            return 0;
        }
    }
    return -1;
}

int PN532_THREAD :: send_pkt_wait_ack(quint8 *cmd, int len)
{
    if(send_pkt(cmd, len) != len){
        /** write error */
        std::cerr << "write error\r\n";
        return -1;
    }

    if(wait_ack()){
        std::cerr << "wait ack error\r\n";
        return -2;
    }

    return len;
}

int PN532_THREAD :: receive_pkt(quint8 *cmd, int len, int timeout)
{
    int received_bytes_count = 0;
    int available_bytes_count = 0;
    const int expected_bytes_count = (int)len;
    int read_size = 0;
    int res;

    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_3);

    do{
        if(socket_available >= expected_bytes_count){
            res = in.readRawData((char *)cmd, expected_bytes_count);
            if(res <= 0){
                std::cerr << "read error 0\r\n";
                return -1;
            }
            socket_available -= expected_bytes_count;
            received_bytes_count = res;
        }else if(socket_available){
            res = in.readRawData((char *)cmd, socket_available);
            if(res <= 0){
                std::cerr << "read error 1\r\n";
                return -1;
            }
            socket_available = 0;
            received_bytes_count += res;
        }else{
            if(tcpSocket->waitForReadyRead(timeout) == 0){
                //std::cerr << "read timeout\r\n";
                if(received_bytes_count){
                    return received_bytes_count;
                }
                return -1;
            }

            available_bytes_count = tcpSocket->bytesAvailable();
            if(available_bytes_count == 0){
                std::cerr << "availabe = 0";
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
                std::cerr << "read error 2\r\n";
                return -3;
            }
            received_bytes_count += res;

        }
    }while(expected_bytes_count > received_bytes_count);

    return received_bytes_count;
}

void PN532_THREAD :: puthex(quint8 *buf, int len)
{
    const char hex_tab[]="0123456789ABCDEF";
    char buf_hex[300];

    buf_hex[3*len] = 0;
    for(int i=0; i<len; i++){
        buf_hex[3*i] = hex_tab[(buf[i]>>4)];
        buf_hex[3*i+1] = hex_tab[(buf[i]&0x0F)];
        buf_hex[3*i+2] = ' ';
    }
    std::cerr << buf_hex << "\r\n";
}
