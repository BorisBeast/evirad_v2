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

int UdpSocket::getPort()
{
    return port;
}

void UdpSocket::setPort(int p)
{
    if(port != p){
        if(socket->state() != QAbstractSocket::UnconnectedState) socket->close();
        if(!socket->bind(p)){
            qCritical() << objectName() << " greska podizanja udp servera na portu " << p;
            return;
        }
        port = p;
        Q_EMIT portChanged(p);
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
    socket->writeDatagram(data, (QHostAddress)address, port);
    Q_EMIT sent(data);
}

