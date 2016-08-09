#ifndef SYNCSERVER_H
#define SYNCSERVER_H

#include <QObject>
#include <QTimer>
#include <QSqlQuery>

class SyncServer : public QObject
{
    Q_OBJECT
public:
    explicit SyncServer(QObject *parent = 0);

signals:
    void write(QByteArray data);

public slots:
    void received(QByteArray data);

private slots:
    void timeoutTimerTimeout();

private:
    void parseData(QString data);
    QString getLastExecutedQuery(const QSqlQuery &query);

    //uint lokacija;
    QTimer* timeoutTimer;
    QByteArray receivedData;
};

#endif // SYNCSERVER_H
