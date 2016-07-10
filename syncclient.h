#ifndef SYNCCLIENT_H
#define SYNCCLIENT_H

#include <QObject>
#include <QTimer>

class SyncClient : public QObject
{
    Q_OBJECT
public:
    explicit SyncClient(uint lokacija, uint syncPeriod, QObject *parent = 0);

signals:
    void write(QByteArray data);

public slots:
    void received(QByteArray data);

private slots:
    void timeoutTimerTimeout();
    void syncTimerTimeout();

private:
    void parseData(QString data);

    uint lokacija;
    uint syncPeriod;
    QTimer* syncTimer;
    QTimer* timeoutTimer;
    QByteArray receivedData;
};

#endif // SYNCCLIENT_H
