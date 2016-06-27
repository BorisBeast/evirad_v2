#include "socketmonitor.h"
#include <QDebug>

SocketMonitor::SocketMonitor(QObject *parent) : QObject(parent)
{
    timeoutTimer.setSingleShot(true);
    timeoutTimer.setInterval(20);
    connect(&timeoutTimer,SIGNAL(timeout()),this,SLOT(timerTimeout()));
}

void SocketMonitor::received(QByteArray data)
{
    //TODO: dodaj vrijeme prijema
    receivedBytes.append(data);
    timeoutTimer.start();
}

void SocketMonitor::sent(QByteArray data)
{
    qDebug()<<socketName+" sent:"<<data;
}

void SocketMonitor::timerTimeout()
{
    qDebug()<<socketName+" received:"<<receivedBytes;
    receivedBytes.clear();
}

QString SocketMonitor::getSocketName() const
{
    return socketName;
}

void SocketMonitor::setSocketName(const QString &value)
{
    socketName = value;
}
