#ifndef KONTROLER_H
#define KONTROLER_H

#include <QObject>

class Kontroler : public QObject
{
    Q_OBJECT
public:
    explicit Kontroler(QObject *parent = 0);

signals:

public slots:
};

#endif // KONTROLER_H