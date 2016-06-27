#include "kontroler.h"

Kontroler::Kontroler(QString address, char tipBrave, QObject *parent) : QObject(parent)
{
    this->address = address;
    this->tipBrave = tipBrave;
}

void Kontroler::received(QByteArray data)
{
    Q_UNUSED(data);
    //TODO:
}

void Kontroler::openDoor()
{
    QString command = "~" + address + tipBrave + "\r\n";
    emit write(command.toLatin1());
}

//TODO: citanje ulaza
