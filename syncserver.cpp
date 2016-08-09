#include "syncserver.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QVariant>
#include <QDebug>

SyncServer::SyncServer(QObject *parent) : QObject(parent)
{
    //this->lokacija = lokacija;

    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(500);
    connect(timeoutTimer,SIGNAL(timeout()),this,SLOT(timeoutTimerTimeout()));
}

void SyncServer::received(QByteArray data)
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

void SyncServer::timeoutTimerTimeout()   //nije primljen citav paket u ocekivanom periodu
{
    qDebug()<<this->objectName()<<"receive timeout";
    timeoutTimer->stop();
    receivedData.clear();
}

void SyncServer::parseData(QString data)    //TODO: napravi genericku funkciju koja ce bilo koju tabelu da pretvara u json objekat (morace svaki parametar da bude string) ?????
{
    qDebug()<<"Primljeno"<<data;

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject message = doc.object();

    QJsonObject response;

    if(message.value("date").isUndefined()) {qCritical()<<"date undefined!"; return;}
    if(message.value("location").isUndefined()) {qCritical()<<"location undefined!"; return;}

    uint lokacija = message.value("location").toInt();

    response.insert("date", message.value("date"));
    response.insert("location", message.value("location"));

    QSqlQuery query;
    query.exec("SELECT DATE_FORMAT(NOW(),'%Y-%m-%d %H:%i:%s') AS request_time");
    query.next();
    QString requestTime = query.value("request_time").toString();
    query.clear();

    response.insert("requestTime", QJsonValue(requestTime));
    QString strQuery = "UPDATE parametri SET vrijednost='" + requestTime + "' WHERE naziv='sync_time' AND lokacija=" + QString::number(lokacija);
    qDebug()<<strQuery;
    query.exec(strQuery);

    if(message.value("date").isNull())
    {
        QJsonObject insertObj;

        QJsonArray tabelaCitac;
        query.setForwardOnly(true);
        query.exec("SELECT id, naziv, funkcija, zona, lokacija, registracija_vremena, kontroler, socket, adresa FROM citac");
        qDebug()<<getLastExecutedQuery(query);
        while(query.next())
        {
            QJsonObject citac;
            citac.insert("id", QJsonValue((int)query.value("id").toUInt()) );
            citac.insert("naziv", QJsonValue(query.value("naziv").toString()) );
            citac.insert("funkcija", QJsonValue(query.value("funkcija").toString()) );
            citac.insert("zona", QJsonValue((int)query.value("zona").toUInt()) );
            citac.insert("lokacija", QJsonValue((int)query.value("lokacija").toUInt()) );
            citac.insert("registracija_vremena", QJsonValue(query.value("registracija_vremena").toBool()) );
            QVariant kontroler = query.value("kontroler");
            citac.insert("kontroler", kontroler.isNull()? QJsonValue(): QJsonValue((int)kontroler.toUInt()) );
            citac.insert("socket", QJsonValue((int)query.value("socket").toUInt()) );
            citac.insert("adresa", QJsonValue(query.value("adresa").toString()) );

            tabelaCitac.append(citac);
        }
        query.clear();
        insertObj.insert("citac", tabelaCitac);

        //TODO: ostale tabele

        response.insert("insert", insertObj);
    }
    else
    {
        //QDateTime date = QDateTime::fromString(message.value("date").toString(), "yyyy-MM-dd HH:mm:ss");
        QString date = message.value("date").toString();

        QHash<QString,QSet<quint64>*> insertHash;
        QHash<QString,QSet<quint64>*> deleteHash;

        query.setForwardOnly(true);
        query.prepare("SELECT tabela, edit_type, edit_id FROM sync_edit_info WHERE vrijeme>=:date AND vrijeme<:request_time");
        query.bindValue(":date", date);
        query.bindValue(":request_time",requestTime);
        query.exec();
        qDebug()<<getLastExecutedQuery(query);
        while(query.next())
        {
            char editType = query.value("edit_type").toString().at(0).toLatin1();
            QHash<QString,QSet<quint64>*>* hash;
            if(editType=='i' || editType=='u') hash=&insertHash;
            else hash=&deleteHash;

            QString tabela = query.value("tabela").toString();
            if(!hash->contains(tabela))hash->insert(tabela, new QSet<quint64>());

            hash->value(tabela)->insert(query.value("edit_id").toULongLong());


        }
        query.clear();

        QJsonObject insertObj;
        QJsonObject deleteObj;
        //QHash<quint64, QJsonObject> objekti;
        if(insertHash.contains("citac"))
        {
            //QSet<quint64> allIds;
            //if(insertHash.contains("citac")) allIds+=*(insertHash.value("citac"));
            //za delete id-eve ne mora da se radi SELECT

            QString ids; bool first=true;
            //foreach (quint64 id, allIds) {
            foreach (quint64 id, *(insertHash.value("citac"))) {
                if(!first) ids+=",";
                first=false;
                ids+=QString::number(id);
            }

            //objekti.clear();
            QJsonArray tabelaInsert;
            query.setForwardOnly(true);
            query.exec("SELECT id, naziv, funkcija, zona, lokacija, registracija_vremena, kontroler, socket, adresa FROM citac WHERE id IN (" + ids + ")");
            qDebug()<<getLastExecutedQuery(query);
            while(query.next())
            {
                QJsonObject citac;
                uint id = query.value("id").toUInt();
                citac.insert("id", QJsonValue((int)id) );
                citac.insert("naziv", QJsonValue(query.value("naziv").toString()) );
                citac.insert("funkcija", QJsonValue(query.value("funkcija").toString()) );
                citac.insert("zona", QJsonValue((int)query.value("zona").toUInt()) );
                citac.insert("lokacija", QJsonValue((int)query.value("lokacija").toUInt()) );
                citac.insert("registracija_vremena", QJsonValue(query.value("registracija_vremena").toBool()) );
                QVariant kontroler = query.value("kontroler");
                citac.insert("kontroler", kontroler.isNull()? QJsonValue(): QJsonValue((int)kontroler.toUInt()) );
                citac.insert("socket", QJsonValue((int)query.value("socket").toUInt()) );
                citac.insert("adresa", QJsonValue(query.value("adresa").toString()) );

                //objekti.insert(id, citac);
                tabelaInsert.append(citac);
            }
            query.clear();


            /*foreach (quint64 id, *(insertHash.value("citac"))) {
                if(objekti.contains(id)) tabelaInsert.append(objekti.value(id));
            }*/

            insertObj.insert("citac", tabelaInsert);
        }
        if(deleteHash.contains("citac"))
        {
            QJsonArray tabelaDelete;
            foreach (quint64 id, *(deleteHash.value("citac"))) {
                tabelaDelete.append(QJsonValue((qint64)id));
            }
            deleteObj.insert("citac", tabelaDelete);
        }

        //TODO: ostale tabele

        response.insert("insert", insertObj);
        response.insert("delete", deleteObj);
    }


    QJsonDocument docResponse(response);

    //qDebug()<<"Poslato"<<docResponse.toJson(QJsonDocument::Compact);

    emit write(docResponse.toJson(QJsonDocument::Compact));
}

QString SyncServer::getLastExecutedQuery(const QSqlQuery& query)
{
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext())
    {
        it.next();
        str.replace(it.key(),it.value().toString());
    }
    return str;
}
