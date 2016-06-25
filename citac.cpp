#include "citac.h"

#include <QDebug>

Citac::Citac(QObject *parent) : QObject(parent)
{

}

void Citac::received(QByteArray data)
{
    qDebug()<<"Received"<<data;
}
