#ifndef ACCESSCONTROL_H
#define ACCESSCONTROL_H

#include <QObject>

class AccessControl : public QObject
{
    Q_OBJECT
public:
    explicit AccessControl(QObject *parent = 0);

signals:

public slots:
};

#endif // ACCESSCONTROL_H