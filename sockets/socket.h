#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QDebug>
#include <QDateTime>

class Socket: public QObject
{
Q_OBJECT
public:
    QDateTime dtRead, dtWrite;
    quint64 countReceived, countSent;
signals:
    void received(QByteArray);
    void sent(QByteArray);
public slots:
    virtual void read(QByteArray) = 0;
    virtual void write(QByteArray) = 0;
};
Q_DECLARE_INTERFACE(Socket, "evirad.Socket")

#endif // SOCKET_H
