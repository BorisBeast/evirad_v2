#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QUdpSocket>
#include "socket.h"

class UdpSocket : public Socket
{
    Q_OBJECT
    Q_INTERFACES(Socket)
    Q_PROPERTY(int receivePort READ getReceivePort WRITE setReceivePort NOTIFY receivePortChanged)
    Q_PROPERTY(int sendPort READ getSendPort WRITE setSendPort NOTIFY sendPortChanged)
    Q_PROPERTY(QString address READ getAddress WRITE setAddress NOTIFY addressChanged)
public:
    Q_INVOKABLE explicit UdpSocket(QObject *parent = 0);

signals:
    void receivePortChanged(int);
    void sendPortChanged(int);
    void addressChanged(QString);

public slots:
    int getReceivePort();
    void setReceivePort(int);
    int getSendPort() const;
    void setSendPort(int value);
    QString getAddress();
    void setAddress(QString);
    void fetchData();

//IODevice---------------------------------------
public Q_SLOTS:
    void read(QByteArray) Q_DECL_OVERRIDE;
    void write(QByteArray) Q_DECL_OVERRIDE;
//----------------------------------------------
protected:
    int receivePort;
    int sendPort;
    QString address;

private:
    QUdpSocket* socket;
};

#endif // UDPSOCKET_H
