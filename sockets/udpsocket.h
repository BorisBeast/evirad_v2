#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QUdpSocket>
#include "socket.h"

class UdpSocket : public Socket
{
    Q_OBJECT
    Q_INTERFACES(Socket)
    Q_PROPERTY(int port READ getPort WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString address READ getAddress WRITE setAddress NOTIFY addressChanged)
public:
    Q_INVOKABLE explicit UdpSocket(QObject *parent = 0);
    
signals:
    void portChanged(int);
    void addressChanged(QString);

public slots:
    int getPort();
    void setPort(int);
    QString getAddress();
    void setAddress(QString);
    void fetchData();

//IODevice---------------------------------------
public Q_SLOTS:
    void read(QByteArray) Q_DECL_OVERRIDE;
    void write(QByteArray) Q_DECL_OVERRIDE;
//----------------------------------------------
protected:
    int port;
    QString address;

private:
    QUdpSocket* socket;
};

#endif // UDPSOCKET_H
