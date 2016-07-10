#include "syncclient.h"

SyncClient::SyncClient(uint lokacija, uint syncPeriod, QObject *parent) : QObject(parent)
{
    this->lokacija = lokacija;
    this->syncPeriod = syncPeriod;

    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(500);
    connect(timeoutTimer,SIGNAL(timeout()),this,SLOT(timeoutTimerTimeout()));

    syncTimer = new QTimer(this);
    syncTimer->setSingleShot(true);
    syncTimer->setInterval(syncPeriod);
    connect(syncTimer,SIGNAL(timeout()),this,SLOT(syncTimerTimeout()));
}

void SyncClient::received(QByteArray data)
{
    receivedData.append(data);
    //qDebug()<<this->objectName()<<data;


    QString strData = QString::fromUtf8(receivedData);

    uint braceCount = 0;
    bool escape = false;
    bool quote = false;

    for(int i=0; i<strData.length(); i++)
    {
        QChar ch = strData[i];

        if(ch=='\\') escape=!escape;
        if(ch=='"' && !escape) quote=!quote;
        if(ch!='\\') escape=false;

        if(ch=='{' && !quote) ++braceCount;   //zagrade su dozvoljene unutar json stringa
        else if(ch=='}' && !quote && braceCount)
        {
            --braceCount;
            if(braceCount==0)   //kraj paketa je kada sve viticaste zagrade budu zatvorene
            {
                timeoutTimer->stop();
                parseData(strData.left(i+1));  //ako nije stigao do kraja primljenog stringa, odnosno ako dodju dva paketa zaredom (ne bi trebalo ikad da se desi, ali za svaki slucaj)

                receivedData.clear();
                if(i!=strData.length()-1) //za slucaj da nije stigao do kraja paketa
                    receivedData=strData.right(strData.length()-1-i).toUtf8();
            }
        }
    }

    if(braceCount!=0) timeoutTimer->start();   //ako nije primljen citav paket startuj timer
}

void SyncClient::timeoutTimerTimeout()
{
    qDebug()<<this->objectName()<<"receive timeout";
    timeoutTimer->stop();
    receivedData.clear();
}

void SyncClient::syncTimerTimeout()
{

}

void SyncClient::parseData(QString data)
{

}
