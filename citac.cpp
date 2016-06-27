#include "citac.h"

#include <QDebug>

Citac::Citac(QString address, QObject *parent) : QObject(parent)
{
    this->address = address;
    stanjePrijema=0;
}

void Citac::received(QByteArray data)
{
    for(int i=0; i<data.length(); i++)
    {
        char ch = data[i];
        if(ch=='~') { stanjePrijema=1; adrScanIndex=0; continue; }
        switch(stanjePrijema)
        {
            default: stanjePrijema=0; break;
            case 1:  //adresa
                if(ch==address[adrScanIndex].toLatin1())
                {
                    ++adrScanIndex;
                    if(adrScanIndex>=address.length()) ++stanjePrijema;
                }
                else stanjePrijema=0;  //nije poruka za ovaj citac
                break;
            case 2:  //komanda
                switch(ch)
                {
                    case 'k': ++stanjePrijema; kartica.clear(); break;  //kartica
                    default: stanjePrijema=0; break;
                }
                break;
            case 3:  //kartica
                if(ch=='\r' || ch=='\n'){qDebug()<<objectName()<<kartica; emit cardRead(kartica); stanjePrijema=0; break;}

                kartica+=ch;
                break;
        }
    }
}

void Citac::signalGranted()
{
    QString command = "~" + address + "s\r\n";
    emit write(command.toLatin1());
}

void Citac::signalDenied()
{
    QString command = "~" + address + "z\r\n";
    emit write(command.toLatin1());
}
