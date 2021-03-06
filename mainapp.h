#ifndef MAINAPP_H
#define MAINAPP_H

#include "kontroler.h"
#include "citac.h"
#include "socket.h"

#include <QObject>
#include <QSqlDatabase>
#include <QTimer>
#include <QtWebSockets/qwebsocket.h>
#include <QtWebSockets/qwebsocketserver.h>

class MainApp : public QObject
{
    Q_OBJECT
public:
    explicit MainApp(QObject *parent = 0);
    ~MainApp();

signals:

public slots:
    void handleSigInt();
    void sendWsMessage(QString msg);


private slots:
    void dbRefreshTimeout();
    void onNewWsConnection();
    void processWsMessage(QString message);
    void webSocketDisconnected();
    void wsServerClosed();

    void quitApp(int returnCode);

private:
    QSqlDatabase db;
    QTimer* dbRefreshTimer;
    QWebSocketServer* webSocketServer;
    QList<QWebSocket*> clients;
    int wsPort;

    int lokacija;
    QHash<uint,Socket*> sockets;
    QHash<uint,Kontroler*> kontroleri;
    QHash<uint,Citac*> citaci;
};

#endif // MAINAPP_H
