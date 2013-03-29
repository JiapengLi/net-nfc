#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <QDialog>
//#include <QTcpSocket>
#include "pn532.h"
#include "ui_tripplanner.h"

#ifndef max
#define max(a,b) (((a) (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

class QPushButton;

class TripPlanner : public QDialog, private Ui::TripPlanner
{
    Q_OBJECT

public:
    TripPlanner(QWidget *parent = 0);

private slots:
    void connectToServer();
    void sendRequest();
    void updateTableWidget();
    void stopSearch();
    void connectionClosedByServer();
    void error(QAbstractSocket::SocketError err);
    void addList(quint8 *buf, int len);



private:
    void closeConnection();
    /** PN532 */
//    void pn532_wake_up();
//    void pn532_send_pkt(quint8 *cmd, int len);
//    int pn532_receive_pkt(quint8 *cmd, int len, int timeout);

    QPushButton *searchButton;
    QPushButton *stopButton;
//    QTcpSocket tcpSocket;
    PN532_THREAD pn532;
    quint16 nextBlockSize;
    int socket_available;
};

#endif
