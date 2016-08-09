#include "tcpsocket.h"

#include <QDebug>
#include <QHostAddress>

TcpSocket::TcpSocket(QObject *parent)
{
    setParent(parent);
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &TcpSocket::fetchData);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    socketConnected = false;
    retryConnection = false;

    connectTimer = new QTimer(this);
    connectTimer->setSingleShot(true);
    connectTimer->setInterval(10 * 1000); //svakih 10 sekundi pokusa da se konektuje na server
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(connectTimerTimeout()));
}

void TcpSocket::setSocket(QTcpSocket *socket)
{
    delete this->socket;
    this->socket = socket;
    socketConnected = true;
    connect(socket, &QTcpSocket::readyRead, this, &TcpSocket::fetchData);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void TcpSocket::connectToHost()
{
    retryConnection = true;
    qDebug()<<this->objectName() + " connecting";
    socket->connectToHost((QHostAddress)address, port);
}

void TcpSocket::disconnect()
{
    retryConnection = false;
    socket->abort();
}

int TcpSocket::getPort()
{
    return port;
}

void TcpSocket::setPort(int p)
{
    if(port != p){
        port = p;
        Q_EMIT portChanged(p);
    }
}

QString TcpSocket::getAddress()
{
    return address;
}

void TcpSocket::setAddress(QString a)
{
    if(address != a){
        address = a;
        Q_EMIT addressChanged(a);
    }
}

void TcpSocket::fetchData()
{
    while(socket->bytesAvailable())
        read(socket->readAll());
}

void TcpSocket::handleError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    /*switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            break;
        case QAbstractSocket::ConnectionRefusedError:
            break;
        default:
            QMessageBox::information(this, tr("Fortune Client"),
                                     tr("The following error occurred: %1.")
                                     .arg(tcpSocket->errorString()));
        }*/
    qDebug()<<this->objectName()<<"Error:"<<socket->errorString();

    if(!socketConnected && retryConnection)
    {
        qDebug()<<this->objectName() + " will retry connection";
        connectTimer->start();
    }
}

void TcpSocket::connectTimerTimeout()
{
    connectToHost();
}

void TcpSocket::connected()
{
    qDebug()<<this->objectName() + " connected";
    socketConnected = true;
}

void TcpSocket::disconnected()
{
    qDebug()<<this->objectName() + " disconnected";
    socketConnected = false;
    if(retryConnection)
    {
        qDebug()<<this->objectName() + " will retry connection";
        connectTimer->start();
    }
}

void TcpSocket::read(QByteArray data)
{
    Q_EMIT received(data);
}

void TcpSocket::write(QByteArray data)
{
    if(socketConnected)
    {
        socket->write(data);
        Q_EMIT sent(data);
    }
    else qDebug()<<this->objectName() + " not connected!";
}
