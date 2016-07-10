#include "accesscontrol.h"

#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

AccessControl::AccessControl(uint citac, char funkcija, uint zona, uint lokacija, QObject *parent) : QObject(parent)
{
    this->citac = citac;
    this->funkcija = funkcija;
    this->zona = zona;
    this->lokacija = lokacija;
}

void AccessControl::cardRead(QByteArray code)
{
    QString strCode = QString(code);
    quint64 idKartice = 0;
    uint idRadnika = 0;
    bool aktivnaKartica = false;
    bool aktivanRadnik = false;
    uint grupa = 0;
    bool allowed = false;
    bool privremenaKartica = false;

    QDateTime sad = QDateTime::currentDateTime();

    QSqlQuery query;
    query.prepare("SELECT id, radnik, aktivna FROM kartica WHERE (kod=:code);");
    query.bindValue(":code", strCode);
    query.exec(); //TODO: provjeri da li je upit izvrsen???
    while(query.next())  //Ako je kartica u bazi
    {
        idKartice = query.value("id").toUInt();
        idRadnika = query.value("radnik").toUInt();
        aktivnaKartica = query.value("aktivna").toBool();

        if(aktivnaKartica) break;   //ako si nasao validnu karticu prestani da trazis
    }
    query.clear();

    if(!idKartice || (idKartice && !aktivnaKartica))   //ako kartice nema medju "regularnim" karticama ili je neaktivna, mozda je ima medju privremenim
    {
        query.prepare("SELECT id, radnik, grupa FROM kartica_privremena WHERE (kod=:code AND vrijeme_od<=:vrijeme AND vrijeme_do>=:vrijeme) ORDER BY id DESC LIMIT 1;");
        query.bindValue(":code", strCode);
        query.bindValue(":vrijeme", sad.toString("yyyy-MM-dd hh:mm:ss"));
        query.exec(); //TODO: provjeri da li je upit izvrsen???
        if(query.next())  //Ako je kartica u bazi
        {
            idKartice = query.value("id").toULongLong();
            idRadnika = query.value("radnik").toUInt();
            aktivnaKartica = true;
            grupa = query.value("grupa").toUInt();
            privremenaKartica = true;
        }
    }

    if(idKartice && idRadnika)
    {
        query.exec(QString("SELECT aktivan, grupa FROM radnik WHERE (id=") + QString::number(idRadnika) + QString(");")); //TODO: provjeri da li je upit izvrsen???
        if(query.next())  //Ako je radnik u bazi
        {
            aktivanRadnik = query.value("aktivan").toBool();
            grupa = query.value("grupa").toUInt();
        }
        else idRadnika=0;
        query.clear();
    }
    else if(grupa) aktivanRadnik=true;

    bool postojiPravo = false;
    bool imaZabranu = false;

    if(aktivanRadnik && aktivnaKartica)
    {
        query.prepare("SELECT i.dani_u_sedmici_flags, i.datum_pocetak, i.datum_kraj, i.prioritet, i.is_zabranjeno FROM `interval` i, pravo p WHERE p.zona=:zona AND p.grupa=:grupa AND p.interval=i.id ORDER BY i.prioritet DESC");
        query.bindValue(":zona", zona);
        query.bindValue(":grupa",grupa);
        query.exec(); //TODO: provjeri da li je upit izvrsen???
        while(query.next())  //pretraga je sortirana po prioritetu
        {
            postojiPravo = true; //postoji neko pravo, jos nije utvrdjeno da li postoji dozvola

            QString daniFlags = query.value("dani_u_sedmici_flags").toString();
            QDateTime datumPocetak = query.value("datum_pocetak").toDateTime();
            QDateTime datumKraj = query.value("datum_kraj").toDateTime();
            //uint prioritet = query.value("prioritet").toInt();
            bool isZabranjeno = query.value("is_zabranjeno").toBool();

            if(!daniFlags.isEmpty())   //interval sa ponavljanjem po danima
            {
                char sadDan=sad.date().dayOfWeek();
                if(daniFlags.size()<sadDan) continue;

                if(daniFlags.at(sadDan-1)=='1')  //pravo vazi za danasnji dan
                {
                    if( (sad.date() >= datumPocetak.date()) && (sad.date() <= datumKraj.date()) && (sad.time() >= datumPocetak.time()) && (sad.time() <= datumKraj.time()) )
                    {
                        if(isZabranjeno) imaZabranu = true;
                        else allowed=true;
                        break;
                    }
                }
            }
            else   //prosti od-do interval
            {
                if((sad>=datumPocetak) && (sad<=datumKraj))
                {
                    if(isZabranjeno) imaZabranu = true;
                    else allowed=true;
                    break;
                }
            }
        }
        query.clear();
    }

    unsigned int greska = allowed?0:( (idKartice?0:1) + (idRadnika?0:2) + (aktivnaKartica?0:4) + (aktivanRadnik?0:8) + (postojiPravo?0:16) + (!imaZabranu?32:64));  //32 ako nema pravo, 64 ako mu je zabranjeno
    qDebug()<<"Kartica:"<<code<<"   Dozvoljeno:"<<allowed<<"   Greska:"<<greska;

    query.prepare("INSERT INTO log_dogadjaja(citac, dogadjaj, kod_kartice, vrijeme, lokacija, greska, kartica, kartica_privremena, radnik) VALUES(:citac, :dogadjaj, :kod, NOW(), :lokacija, :greska, :kartica, :kartica_privremena, :radnik)");
    query.bindValue(":citac", citac);
    qDebug()<<funkcija;
    query.bindValue(":dogadjaj", QString(funkcija));
    query.bindValue(":kod", strCode);
    query.bindValue(":lokacija", lokacija);
    query.bindValue(":greska", greska);
    query.bindValue(":kartica", privremenaKartica?QVariant(QVariant::UInt):(idKartice?idKartice:QVariant(QVariant::UInt)));
    query.bindValue(":kartica_privremena", privremenaKartica?(idKartice?idKartice:QVariant(QVariant::UInt)):QVariant(QVariant::UInt));
    query.bindValue(":radnik", idRadnika?idRadnika:QVariant(QVariant::UInt));
    if(!query.exec()) qDebug()<<"Insert greska:"<<query.lastError().text();
    query.clear();

    if(allowed) emit accessGranted();
    else emit accessDenied();
}
