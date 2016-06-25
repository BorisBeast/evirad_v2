#ifndef CITAC_H
#define CITAC_H

#include <QObject>

class Citac : public QObject
{
    Q_OBJECT
public:
    explicit Citac(QObject *parent = 0);

signals:

public slots:
    void received(QByteArray data);

};

#endif // CITAC_H
