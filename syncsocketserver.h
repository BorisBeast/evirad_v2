#ifndef SYNCSOCKETSERVER_H
#define SYNCSOCKETSERVER_H

#include <QObject>
#include <QTcpServer>

class SyncSocketServer : public QObject
{

    Q_OBJECT
public:
    explicit SyncSocketServer(int port, QObject *parent = 0);

signals:

public slots:

private slots:
    void newConnection();
    void disconnected();

private:
    QTcpServer *server;
};

#endif // SYNCSOCKETSERVER_H
