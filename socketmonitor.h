#ifndef SOCKETMONITOR_H
#define SOCKETMONITOR_H

#include <QDateTime>
#include <QObject>
#include <QTimer>

class SocketMonitor : public QObject
{
    Q_OBJECT
public:
    explicit SocketMonitor(QObject *parent = 0);

    QString getSocketName() const;
    void setSocketName(const QString &value);

signals:

public slots:
    void received(QByteArray data);
    void sent(QByteArray data);

private slots:
    void timerTimeout();

private:
    QDateTime dtReceived;
    QString socketName;
    QTimer timeoutTimer;
    QByteArray receivedBytes;
};

#endif // SOCKETMONITOR_H
