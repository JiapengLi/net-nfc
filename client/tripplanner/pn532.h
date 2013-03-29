#ifndef PN532_H
#define PN532_H

#include <QThread>
#include <QDialog>
#include <QTcpSocket>
#include <QtCore>

#include "pn532_cmd.h"

#define PN532_SAM_NORMAL_MODE               (0x01)
#define PN532_SAM_VIRTUAL_CARD              (0x02)
#define PN532_SAM_WIRED_CARD                (0x03)
#define PN532_SAM_DUAL_CARD                 (0x04)

#define PN532_BRTY_ISO14443A                0x00
#define PN532_BRTY_ISO14443B                0x03
#define PN532_BRTY_212KBPS                  0x01
#define PN532_BRTY_424KBPS                  0x02
#define PN532_BRTY_JEWEL                    0x04

class PN532_THREAD : public QThread
{
    Q_OBJECT

public:
    typedef enum{
        WORK_MODE_IDLE,
        WORK_MODE_CONNECT_TO_HOST,
        WORK_MODE_BEGIN,
        WORK_MODE_READ_MIFARE_M1FS50,
        WORK_MODE_STOP,
    }workMode_t;

    PN532_THREAD();
    void wake_up();
    int send_pkt(quint8 *cmd, int len);
    int send_pkt_wait_ack(quint8 *cmd, int len);
    int receive_pkt(quint8 *cmd, int len, int timeout);
    int wait_ack();
    int frame_analyse(uint8_t *buf, int pkt_len);
    int frame_check(uint8_t *buf, int len);

    int SAMConfiguration(quint8 *buf, uint8_t mode=PN532_SAM_NORMAL_MODE, \
                        uint8_t timeout=20, uint8_t irq=0);
    int InListPassiveTarget(quint8 *buf, uint8_t MaxTg = 1, uint8_t BrTy = PN532_BRTY_ISO14443A, \
                            uint8_t *InitiatorData = NULL, uint8_t len=0);
    int GetFirmwareVersion(quint8 *buf);

    int setWorkMode(workMode_t mode);

    QTcpSocket *tcpSocket;

    void puthex(quint8 *buf, int len);

    void semBufProducer(quint8 data);
    quint8 semBufConsumer();
    void writeSemBuf(quint8 *buf, int len);
    void readSemBuf(quint8 *buf, int len);

public slots:
    void connectToServer();
    void connectionClosedByServer();
    void begin();
    void closeConnection();
    void error(QAbstractSocket::SocketError err);
signals:
    void getFrame(quint8 *, int);
    void getId(quint8 *, int);
    void beginSig();

protected:
    void run();

private:
//    QTcpSocket tcpSocket;
    int socket_available;
    static const uint8_t ack[6];
    static const int BufferSize;
    workMode_t work_mode;
    quint8 buf[1024];

    quint8 semBuffer[1024];
    QSemaphore *freeSpace;
    QSemaphore *usedSpace;
    int consumerCnt;
    int producerCnt;
};


#endif // PN532_H
