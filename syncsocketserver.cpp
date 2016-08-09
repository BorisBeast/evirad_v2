#include "socketmonitor.h"
#include "syncserver.h"
#include "syncsocketserver.h"
#include "tcpsocket.h"

#include <QDebug>

SyncSocketServer::SyncSocketServer(int port, QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    //qDebug() << this->objectName()<<"Listening:" << ;
    if(!server->listen(QHostAddress::Any, port)){
        qCritical() << objectName() << " greska podizanja udp servera na portu " << port;
        return;
    }
    else qDebug() << this->objectName()<<"Listening on port"<<port;
}

void SyncSocketServer::newConnection()
{
    while (server->hasPendingConnections())
    {
        QTcpSocket *socket = server->nextPendingConnection();
        TcpSocket* tcpSocket = new TcpSocket(socket);
        tcpSocket->setSocket(socket);
        tcpSocket->setObjectName("tcp_socket_" + QHostAddress(socket->peerAddress().toIPv4Address()).toString());
        SyncServer* syncServer = new SyncServer(socket);
        syncServer->setObjectName("sync_server_" + QHostAddress(socket->peerAddress().toIPv4Address()).toString());
        connect(tcpSocket, SIGNAL(received(QByteArray)), syncServer, SLOT(received(QByteArray)));
        connect(syncServer, SIGNAL(write(QByteArray)), tcpSocket, SLOT(write(QByteArray)));

        SocketMonitor* socMon = new SocketMonitor(socket);
        socMon->setSocketName(tcpSocket->objectName());
        connect(tcpSocket,SIGNAL(received(QByteArray)),socMon,SLOT(received(QByteArray)));
        connect(tcpSocket,SIGNAL(sent(QByteArray)),socMon,SLOT(sent(QByteArray)));

        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    }
}

void SyncSocketServer::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}
