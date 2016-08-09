#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QTimer>
#include "socket.h"

class TcpSocket : public Socket
{
    Q_OBJECT
    Q_INTERFACES(Socket)
    Q_PROPERTY(int port READ getPort WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString address READ getAddress WRITE setAddress NOTIFY addressChanged)
public:
    Q_INVOKABLE explicit TcpSocket(QObject *parent = 0);
    void setSocket(QTcpSocket* socket);

signals:
    void portChanged(int);
    void addressChanged(QString);

public slots:
    void connectToHost();
    void disconnect();

    int getPort();
    void setPort(int);
    QString getAddress();
    void setAddress(QString);

private slots:
    void fetchData();
    void handleError(QAbstractSocket::SocketError socketError);
    void connectTimerTimeout();
    void connected();
    void disconnected();

//Socket---------------------------------------
public Q_SLOTS:
    void read(QByteArray) Q_DECL_OVERRIDE;
    void write(QByteArray) Q_DECL_OVERRIDE;
//----------------------------------------------
protected:
    int port;
    QString address;

private:
    QTcpSocket* socket;
    QTimer* connectTimer;
    bool socketConnected;
    bool retryConnection;
};

#endif // TCPSOCKET_H
