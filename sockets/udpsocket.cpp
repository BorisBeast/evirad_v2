#include "udpsocket.h"
#include <QDebug>

UdpSocket::UdpSocket(QObject *parent)
{
    setParent(parent);
    socket = new QUdpSocket(this);

    connect(socket, &QUdpSocket::readyRead, this, &UdpSocket::fetchData);
    //connect(socket, &QUdpSocket::disconnected, [&](){ qCritical() << this->objectName() << " port zatvoren";});
    //connect(socket, &QUdpSocket::connected, [&](){ qDebug() << this->objectName() << " port otvoren";});
    connect(socket, &QUdpSocket::stateChanged, [&](QAbstractSocket::SocketState state){ qDebug() << this->objectName() << " SocketState: " << state;});
//    connect(this, &UdpSocket::error, [&](QAbstractSocket::SocketError error){ qDebug() << this->objectName() << " SocketError: " << error;});

    sendPort = 0;
    receivePort = 0;
}

void UdpSocket::fetchData()
{
    while (socket->hasPendingDatagrams()) {
        QHostAddress senderIp;
        quint16 senderPort;
        QByteArray rxb;
        rxb.resize(socket->pendingDatagramSize());
        if (socket->readDatagram(rxb.data(), rxb.size(), &senderIp, &senderPort) == -1) continue;
        read(rxb);
    }
}

int UdpSocket::getReceivePort()
{
    return receivePort;
}

void UdpSocket::setReceivePort(int p)
{
    if(receivePort != p){
        if(socket->state() != QAbstractSocket::UnconnectedState) socket->close();
        if(!socket->bind(p)){
            qCritical() << objectName() << " greska podizanja udp servera na portu " << p;
            return;
        }
        receivePort = p;
        Q_EMIT receivePortChanged(p);
    }
}

QString UdpSocket::getAddress()
{
    return address;
}

void UdpSocket::setAddress(QString a)
{
    if(address != a){
        address = a;
        Q_EMIT addressChanged(a);
    }
}


void UdpSocket::read(QByteArray data)
{
    Q_EMIT received(data);
}

void UdpSocket::write(QByteArray data)
{
    if(!sendPort) setSendPort(receivePort);

    socket->writeDatagram(data, (QHostAddress)address, sendPort);
    Q_EMIT sent(data);
}

int UdpSocket::getSendPort() const
{
    return sendPort;
}

void UdpSocket::setSendPort(int p)
{
    if(sendPort != p){
        sendPort = p;
        Q_EMIT sendPortChanged(p);
    }
}

