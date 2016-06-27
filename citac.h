#ifndef CITAC_H
#define CITAC_H

#include <QObject>

class Citac : public QObject
{
    Q_OBJECT
public:
    explicit Citac(QString address, QObject *parent = 0);

signals:
    void cardRead(QByteArray code);
    void write(QByteArray data);

public slots:
    void received(QByteArray data);
    void signalGranted();
    void signalDenied();

private:
    QString address;
    int adrScanIndex;
    int stanjePrijema;
    QByteArray kartica;
};

#endif // CITAC_H
