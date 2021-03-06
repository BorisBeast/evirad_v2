#include "mainapp.h"
#include "processmanager.h"
#include "udpsocket.h"
#include "portserial.h"
#include "socketmonitor.h"
#include "accesscontrol.h"
#include "syncserver.h"
#include "syncclient.h"
#include "tcpsocket.h"
#include "syncsocketserver.h"

#include <QDebug>
#include <QCoreApplication>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QMetaMethod>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

MainApp::MainApp(QObject *parent) : QObject(parent)
{
    setObjectName(QStringLiteral("Evirad"));
    qDebug() << "Library path: " << QCoreApplication::libraryPaths();

    QString key_db_host="DB/Host";
    QString key_db_name="DB/Name";
    QString key_db_user="DB/User";
    QString key_db_password="DB/Password";
    QString key_system_lokacija="System/Lokacija";
    QString key_system_syncperiod="System/SyncPeriod";

    QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    if(!settings.contains(key_db_host)) settings.setValue(key_db_host, QString("localhost"));
    if(!settings.contains(key_db_name)) settings.setValue(key_db_name, QString("evirad_v2"));
    if(!settings.contains(key_db_user)) settings.setValue(key_db_user, QString("root"));
    if(!settings.contains(key_db_password)) settings.setValue(key_db_password, QString("sifrasifra"));
    if(!settings.contains(key_system_lokacija)) settings.setValue(key_system_lokacija, 1);
    if(!settings.contains(key_system_syncperiod)) settings.setValue(key_system_syncperiod, 10);

    QString dbHost = settings.value(key_db_host).toString();
    QString dbName = settings.value(key_db_name).toString();
    QString dbUser = settings.value(key_db_user).toString();
    QString dbPassword = settings.value(key_db_password).toString();
    lokacija = settings.value(key_system_lokacija).toInt();
    uint syncPeriod = settings.value(key_system_syncperiod).toInt();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(dbHost);
    db.setDatabaseName(dbName);
    db.setUserName(dbUser);
    db.setPassword(dbPassword);
    if(!db.open())
    {
        qDebug()<<"Database error:"<<db.lastError().text();

        QTimer* exitTimer = new QTimer(this);
        exitTimer->setSingleShot(true);
        QObject::connect(exitTimer, &QTimer::timeout, [=] () { QCoreApplication::exit(1); });   //mora iz timera da se pozove QCoreApplication::exit() jer u ovom trenu jos uvijek ne radi event loop!
        exitTimer->start(1);

        return;
    }

    dbRefreshTimer=new QTimer(this);
    connect(dbRefreshTimer, SIGNAL(timeout()), this, SLOT(dbRefreshTimeout()));
    dbRefreshTimer->start(600000);

    wsPort = 1234;
    webSocketServer= new QWebSocketServer(this->objectName()+" WebSocket Server", QWebSocketServer::NonSecureMode, this);
    if (webSocketServer->listen(QHostAddress::Any, wsPort)) {
        qDebug() << "WebSocketServer listening on port" << wsPort;
        connect(webSocketServer, SIGNAL(newConnection()), this, SLOT(onNewWsConnection()));
        connect(webSocketServer, SIGNAL(closed()), this, SLOT(wsServerClosed()));
    }
    else {}//TODO: probaj ponovo kasnije ?

    ProcessManager* shell = new ProcessManager(this);
    shell->setObjectName("shell");


    QSqlQuery query;
    query.prepare("SELECT id, naziv, tip, funkcija, parametri FROM socket WHERE lokacija = :lokacija");
    query.bindValue(":lokacija", lokacija);
    query.exec();
    while(query.next())
    {
        uint id = query.value("id").toUInt();
        QString naziv = query.value("naziv").toString();
        char tip = query.value("tip").toString().at(0).toLatin1();
        char funkcija = query.value("funkcija").isNull()?0:query.value("funkcija").toString().at(0).toLatin1();
        QJsonDocument doc = QJsonDocument::fromJson(query.value("parametri").toString().toUtf8());
        QJsonObject parametri = doc.object();

        Socket* socket = NULL;

        if(tip=='u')
        {
            UdpSocket* udpSocket = new UdpSocket(this);
            udpSocket->setObjectName(naziv.replace(" ","_"));
            if(!parametri["address"].isUndefined())udpSocket->setAddress(parametri["address"].toString());
            if(!parametri["port"].isUndefined())udpSocket->setReceivePort(parametri["port"].toInt());
            if(!parametri["sendPort"].isUndefined())udpSocket->setSendPort(parametri["sendPort"].toInt());
            socket = udpSocket;
        }
        else if(tip=='s')
        {
            PortSerial* serialSocket = new PortSerial(this);
            serialSocket->setObjectName(naziv.replace(" ","_"));
            serialSocket->setAddress(parametri["address"].toString());
            serialSocket->setBaudRate(parametri["baud"].toInt());
            serialSocket->start();
            socket = serialSocket;
        }
        else if(tip=='t')
        {
            TcpSocket* tcpSocket = new TcpSocket(this);
            tcpSocket->setObjectName(naziv.replace(" ","_"));
            tcpSocket->setAddress(parametri["address"].toString());
            tcpSocket->setPort(parametri["port"].toInt());
            tcpSocket->connectToHost();
            socket = tcpSocket;
        }
        else if(tip=='T')
        {
            if(funkcija=='S')
            {
                SyncSocketServer* server = new SyncSocketServer(parametri["port"].toInt(),this);
                server->setObjectName(naziv.replace(" ","_"));
            }
        }

        if(funkcija=='s')
        {
            SyncClient* syncClient = new SyncClient(lokacija,syncPeriod,this);
            syncClient->setObjectName("sync_client");
            connect(socket, SIGNAL(received(QByteArray)), syncClient, SLOT(received(QByteArray)));
            connect(syncClient, SIGNAL(write(QByteArray)), socket, SLOT(write(QByteArray)));
        }

        if(socket!=NULL)
        {
            SocketMonitor* socMon = new SocketMonitor(this);
            socMon->setSocketName(socket->objectName());
            connect(socket,SIGNAL(received(QByteArray)),socMon,SLOT(received(QByteArray)));
            connect(socket,SIGNAL(sent(QByteArray)),socMon,SLOT(sent(QByteArray)));
            sockets.insert(id,socket);
        }
    }
    query.clear();


    query.prepare("SELECT id, naziv, socket, adresa, tip_brave, trajanje_otvaranja, ulazi_maska FROM kontroler WHERE lokacija = :lokacija");
    query.bindValue(":lokacija", lokacija);
    query.exec();
    while(query.next())
    {
        uint id = query.value("id").toUInt();
        QString naziv = query.value("naziv").toString();
        uint socket = query.value("socket").toUInt();
        QString adresa = query.value("adresa").toString();
        char tipBrave = query.value("tip_brave").toString().at(0).toLatin1();
        Kontroler* kontroler = new Kontroler(adresa, tipBrave, this);
        kontroler->setObjectName(naziv.replace(" ","_"));

        connect(sockets[socket],SIGNAL(received(QByteArray)),kontroler,SLOT(received(QByteArray)));
        connect(kontroler,SIGNAL(write(QByteArray)),sockets[socket],SLOT(write(QByteArray)));

        kontroleri.insert(id,kontroler);
    }
    query.clear();

    query.prepare("SELECT id, naziv, funkcija, zona, kontroler, socket, adresa FROM citac WHERE lokacija = :lokacija");
    query.bindValue(":lokacija", lokacija);
    query.exec();
    while(query.next())
    {
        uint id = query.value("id").toUInt();
        QString naziv = query.value("naziv").toString();
        char funkcija = query.value("funkcija").toString().at(0).toLatin1();
        uint zona = query.value("zona").toUInt();
        uint kontroler = query.value("kontroler").toUInt();
        uint socket = query.value("socket").toUInt();
        QString adresa = query.value("adresa").toString();

        Citac* citac;
        if(funkcija=='u' || funkcija=='i')
        {
            citac = new Citac(adresa, this);
            citac->setObjectName(naziv.replace(" ","_"));

            connect(sockets[socket],SIGNAL(received(QByteArray)),citac,SLOT(received(QByteArray)));
            connect(citac,SIGNAL(write(QByteArray)),sockets[socket],SLOT(write(QByteArray)));

            if(kontroler!=0)
            {
                AccessControl* ac = new AccessControl(id,funkcija,zona,lokacija,this);
                connect(citac,SIGNAL(cardRead(QByteArray)),ac,SLOT(cardRead(QByteArray)));
                connect(ac,SIGNAL(accessDenied()),citac,SLOT(signalDenied()));
                connect(ac,SIGNAL(accessGranted()),citac,SLOT(signalGranted()));
                connect(ac,SIGNAL(accessGranted()),kontroleri[kontroler],SLOT(openDoor()));
            }
        }
        //TODO: funkcija=='t'

        citaci.insert(id,citac);
    }
    query.clear();
}

MainApp::~MainApp()
{
    qInstallMessageHandler(0);

    QString connection = db.connectionName();
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(connection);

    if(webSocketServer) webSocketServer->close();
    qDeleteAll(clients.begin(), clients.end());
}

void MainApp::handleSigInt()  //na SIGINT (CTRL+C) izadji regularno
{
    qDebug()<<"\n\nTerminating...";
    QCoreApplication::exit();
}

void MainApp::dbRefreshTimeout()
{
    QSqlQuery refresh;
    if(!refresh.exec(QString("SELECT NOW();")))
        qDebug()<<"DB error (refresh)"<<refresh.lastError().text();
    refresh.clear();
}

void MainApp::sendWsMessage(QString msg)
{
    foreach (QWebSocket* socket, clients) {
        if(socket->state()==QAbstractSocket::ConnectedState)
            socket->sendTextMessage(msg);
    }
}

void MainApp::onNewWsConnection()
{
    QWebSocket *socket = webSocketServer->nextPendingConnection();

    connect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(processWsMessage(QString)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(webSocketDisconnected()));

    clients << socket;
}

void MainApp::processWsMessage(QString message)
{
    qDebug() << "WS message received:" << message;

    QStringList words;
    QString word="";
    //bool quote = false;
    bool escape = false;

    int pos = 0;
    QRegExp rx("[^\\\\](\\\\r)"); while ((pos = rx.indexIn(message)) != -1) {message.replace(pos+1, 2, "\r");}
    rx = QRegExp("[^\\\\](\\\\n)"); while ((pos = rx.indexIn(message)) != -1) {message.replace(pos+1, 2, "\n");}
    rx = QRegExp("[^\\\\](\\\\t)"); while ((pos = rx.indexIn(message)) != -1) {message.replace(pos+1, 2, "\t");}
    //message.replace("\\\\","\\");

    for(int i=0; i<message.length(); i++)    //TODO: dodaj { } za definisanje niza
    {
        QChar ch = message.at(i);
        if(ch=='\\') escape=!escape;

        if(ch==' ' && !escape){ if(word!="") words<<word; word="";}
        else if(!escape || ch==' ') word+=ch;

        if(ch!='\\') escape=false;
    }
    if(word!="") words<<word;


    //TODO: napravi da mogu da se dinamicki konstruisu objekti i konektuju signali !!!!


    QObject* obj=0;
    if(words.length()==0) qDebug()<<"Format: objectName methodName params";
    else if(words[0]=="this") obj=this;
    else {obj = this->findChild<QObject*>(words[0]); if(obj==0) qDebug()<<"No such object!!";}
    if(obj==0)
    {
        QList<QObject*> objects = this->findChildren<QObject*>();
        QStringList names;
        names<<"this"<<"shell";
        foreach (QObject* o, objects) {
            if(o->objectName()!="" && o->objectName()!="shell") names << o->objectName();
        }
        qDebug()<<"Available objects:"<<names.join(" ");
    }
    else
    {
        if(words.length()==1) //samo ime objekta
        {
            const QMetaObject* metaObject = obj->metaObject();
            QStringList methods;

            methods << "prop";

            for(int i = 0/*metaObject->methodOffset()*/; i < metaObject->methodCount(); ++i)
            {
                QMetaMethod method =  metaObject->method(i);
                methods << QString::fromLatin1(method.methodSignature());
            }

            qDebug()<<"Available methods:"<<methods.join(" ");
        }
        else
        {
            int wordsLength = words.length();
            const QMetaObject* metaObject = obj->metaObject();

            if(words.at(1)=="prop")
            {
                if(wordsLength==2) //samo "imeObjekta prop"
                {
                    QStringList properties;
                    for (int i = 0; i < metaObject->propertyCount(); i++){
                        properties.append(metaObject->property(i).name());
                    }
                    qDebug()<<"Available properties:"<<properties.join(" ");
                }
                else
                {
                    if(wordsLength==3) //imeObjekta prop imeProperty
                        qDebug()<<"Property value:"<<obj->property(words.at(2).toLatin1());
                    else
                    {
                        if( obj->setProperty(words.at(2).toLatin1(), words.at(3)) ) qDebug()<<"Property set";
                        else qDebug()<<"Property not set";
                    }
                }
            }

            int methodIndex = metaObject->indexOfMethod(((QString)words[1]).toLatin1().data());
            QMetaMethod method = metaObject->method(methodIndex);
            int paramCount = method.parameterCount();

            QVariant* vars[9];
            for(int i=0; i<9; i++) vars[i]=0;

            QGenericArgument ga[9];

            if(wordsLength < paramCount + 2) qDebug()<<"Wrong param count!";
            for(int i=0; i<paramCount; i++)
            {
                QMetaType::Type type = (QMetaType::Type) method.parameterType(i);
                QVariant* v;   //moraju da budu pokazivaci jer u protivnom c++ brise promjenljivu na kraju svakog prolaza pa QGenericArgument (koji cuva pokazivac sa v->data() ) ne bude validan pri pozivu metoda !!!!

                if(i<wordsLength-2)
                {
                    if(type == QMetaType::Char) v=new QVariant(((QString)words[i+2]).at(0).toLatin1()); //TODO: ovo je hack, vidi postoji li bolji (genericki) nacin za ovo
                    else v=new QVariant(words[i+2]);
                }
                else
                    v=new QVariant("");

                vars[i] = v;

                if( !v->convert( type ) ){
                    qDebug() << "Unable to convert string to" << (QVariant::Type)type;
                    for(int i=0; i<9; i++) delete vars[i];
                    return;
                }

                if(v->isValid()){
                    ga[i] = QGenericArgument(v->typeName(), v->data());
                }
                else qDebug() << "QVariant variable is invalid" << *v;
            }

            method.invoke(obj, ga[0], ga[1], ga[2], ga[3], ga[4], ga[5], ga[6], ga[7], ga[8]);

            for(int i=0; i<9; i++) delete vars[i];
        }
    }
}

void MainApp::webSocketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    qDebug() << "socketDisconnected:" << client;
    if (client) {
        clients.removeAll(client);
        client->deleteLater();
    }
}

void MainApp::wsServerClosed()
{
    qInfo() << "Server closed!!";
}

void MainApp::quitApp(int returnCode)
{
    qDebug()<<"Exiting with return code"<<returnCode;
    QCoreApplication::exit(returnCode);
}
