#ifndef ACCESSCONTROL_H
#define ACCESSCONTROL_H

#include <QObject>

class AccessControl : public QObject
{
    Q_OBJECT
public:
    explicit AccessControl(uint citac, char funkcija, uint zona, uint lokacija, QObject *parent = 0);

signals:
    void accessGranted();
    void accessDenied();

public slots:
    void cardRead(QByteArray code);

private:
    uint citac;
    char funkcija;
    uint zona;
    uint lokacija;
};

#endif // ACCESSCONTROL_H
