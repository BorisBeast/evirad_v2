#include "syncclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QDebug>

SyncClient::SyncClient(uint lokacija, uint syncPeriod, QObject *parent) : QObject(parent)
{
    this->lokacija = lokacija;
    this->syncPeriod = syncPeriod;

    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(500);
    connect(timeoutTimer,SIGNAL(timeout()),this,SLOT(timeoutTimerTimeout()));

    syncTimer = new QTimer(this);
    syncTimer->setSingleShot(false);  //TODO: ovo treba da bude true, a onda treba dodati timeout za zahtjev
    syncTimer->setInterval(syncPeriod * 1000);
    connect(syncTimer,SIGNAL(timeout()),this,SLOT(syncTimerTimeout()));
    syncTimer->start();
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
    QVariant syncTime(QVariant::String);
    QSqlQuery query;
    QString strQuery = "SELECT vrijednost FROM parametri WHERE naziv='sync_time' AND lokacija=" + QString::number(this->lokacija);
    qDebug()<<strQuery;
    query.exec(strQuery);
    if(query.next())
    {
        syncTime = query.value("vrijednost");
    }

    QJsonObject request;
    request.insert("date", syncTime.isNull()?QJsonValue():QJsonValue(syncTime.toString()));
    request.insert("location", QJsonValue((int)this->lokacija) );

    QJsonDocument docRequest(request);

    emit write(docRequest.toJson(QJsonDocument::Compact));  //posalji zahtjev
}

void SyncClient::parseData(QString data)
{
    qDebug()<<"Primljeno"<<data;

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject message = doc.object();

    QSqlQuery query;
    query.exec("SET FOREIGN_KEY_CHECKS=0;");       //TODO: makni ovo !!!!


    //TODO: dodaj transakciju da bi u slucaju greske rollback

    //TODO: redosled tabela za insert i delete    (insert treba da ima obrnut redosled od delete)

    if(!message.value("insert").isUndefined() && !message.value("insert").toObject().value("citac").isUndefined())
    {
        QString strQuery = "INSERT INTO citac(id, naziv, funkcija, zona, lokacija, registracija_vremena, kontroler, socket, adresa) VALUES ";

        uint i=0;
        QJsonArray insertArray = message.value("insert").toObject().value("citac").toArray();
        foreach (QJsonValue val, insertArray) {
            QJsonObject insertObj = val.toObject();
            if(i++!=0) strQuery+=",";
            strQuery+=QString("(%1,'%2','%3',%4,%5,%6,%7,%8,%9)")
                    .arg(insertObj["id"].toInt())
                    .arg(insertObj["naziv"].toString())
                    .arg(insertObj["funkcija"].toString())
                    .arg(insertObj["zona"].toInt())
                    .arg(insertObj["lokacija"].toInt())
                    .arg(insertObj["registracija_vremena"].toBool())
                    .arg(insertObj["kontroler"].isNull()?QString("NULL"):QString::number(insertObj["kontroler"].toInt()))
                    .arg(insertObj["socket"].toInt())
                    .arg(insertObj["adresa"].toString());
        }

        strQuery += " ON DUPLICATE KEY UPDATE naziv=VALUES(naziv), funkcija=VALUES(funkcija), zona=VALUES(zona), lokacija=VALUES(lokacija), registracija_vremena=VALUES(registracija_vremena), kontroler=VALUES(kontroler), socket=VALUES(socket), adresa=VALUES(adresa);";
        qDebug()<<strQuery;
        query.exec(strQuery);
        query.clear();
    }
    //TODO: insert ostale tabele

    if(!message.value("delete").isUndefined() && !message.value("delete").toObject().value("citac").isUndefined())
    {
        QString strQuery = "DELETE FROM citac WHERE id IN (";

        uint i=0;
        QJsonArray deleteArray = message.value("delete").toObject().value("citac").toArray();
        foreach (QJsonValue val, deleteArray) {
            if(i++!=0) strQuery+=",";
            strQuery += QString::number(val.toInt());
        }

        strQuery += ")";
        qDebug()<<strQuery;
        query.exec(strQuery);
        query.clear();
    }
    //TODO: delete ostale tabele

    QString date = message.value("requestTime").toString();
    QString strQuery = "UPDATE parametri SET vrijednost='" + date + "' WHERE naziv='sync_time' AND lokacija=" + QString::number(this->lokacija);
    qDebug()<<strQuery;
    query.exec(strQuery);
}
