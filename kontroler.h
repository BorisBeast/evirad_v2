#ifndef KONTROLER_H
#define KONTROLER_H

#include <QObject>

class Kontroler : public QObject
{
    Q_OBJECT
public:
    explicit Kontroler(QString address, char tipBrave, QObject *parent = 0);

signals:
    void write(QByteArray data);

public slots:
    void received(QByteArray data);
    void openDoor();

private:
    QString address;
    char tipBrave;
};

#endif // KONTROLER_H
