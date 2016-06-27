#include "mainapp.h"

#include <QCoreApplication>
#include <QObject>
#include <QLocale>
#include <QSocketNotifier>
#include <QThread>

#ifdef Q_OS_LINUX
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#endif
#ifdef Q_OS_WIN
#include <windows.h>
#include <stdio.h>
#endif

MainApp *app;

#ifdef Q_OS_LINUX
int sigIntFd[2];
QSocketNotifier *snInt;

static void intSignalHandler(int unused)
{
    Q_UNUSED(unused);
    char a = 1;
    ::write(sigIntFd[0], &a, sizeof(a));
}

int setupUnixSignalHandler()
{
    struct sigaction sigint;

    sigint.sa_handler = intSignalHandler;
    sigemptyset(&sigint.sa_mask);
    sigint.sa_flags = 0;
    sigint.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &sigint, 0) > 0)
       return 1;

    return 0;
}
#endif
#ifdef Q_OS_WIN
BOOL CtrlHandler( DWORD fdwCtrlType )
{
    switch( fdwCtrlType )
    {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
      printf( "Ctrl-C event\n\n" );
      app->handleSigInt();
      return( TRUE );

    default:
      return FALSE;
    }
}
#endif
void TekstLoger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    //QByteArray localMsg = msg.toLocal8Bit();

    QDateTime dtSada = QDateTime::currentDateTime();
    QString time = dtSada.toString("yyyy-MM-dd HH:mm:ss.zzz    ");
    //QString thread = (QThread::currentThread()->objectName().length() > 0) ? QThread::currentThread()->objectName() + '\t': QString::number((unsigned long int)QThread::currentThreadId()) + '\t';
    QString msgType = "";
    bool terminate = false;
    bool info=false;
    FILE* out=NULL;
    switch (type) {
        case QtDebugMsg:
            msgType = "DEBUG    ";
            out=stdout;
            break;
        case QtWarningMsg:
            msgType = "WARN    ";
            out=stdout;
            break;
        case QtCriticalMsg:
            msgType = "ERROR    ";
            out=stderr;
            break;
        case QtFatalMsg:
            msgType = "FATAL    ";
            out=stderr;
            terminate = true;
            break;
        case QtInfoMsg:
            msgType = "INFO    ";
            out=stdout;
            info=true;    //info tip se ne salje na websocket
            break;
    }

    fprintf(out, "%s\n", msg.toLocal8Bit().data());  //posalji nepromijenjeno na konzolu

    QString message = msgType + time + msg;
    if(!info && app) app->sendWsMessage(message);   //posalji WebSocket-om sa tipom i vremenom

    if(terminate){
        abort();
    }
}

int main(int argc, char *argv[])
{
    QLocale locale(QLocale::SerboCroatian, QLocale::Montenegro);
    QLocale::setDefault(locale);

    QThread::currentThread()->setObjectName("MainThread");

    QCoreApplication a(argc, argv);

    qInstallMessageHandler(TekstLoger);

    app = new MainApp();

#ifdef Q_OS_LINUX
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigIntFd))
           qCritical("Couldn't create SIGINT socketpair");
    else
    {
        setupUnixSignalHandler();

        snInt = new QSocketNotifier(sigIntFd[1], QSocketNotifier::Read, app);
        QObject::connect(snInt, SIGNAL(activated(int)), app, SLOT(handleSigInt()));
    }
#endif
#ifdef Q_OS_WIN
    if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) ) qCritical("Could not set control handler");
#endif

    int ret = a.exec();

    delete app;
    app=0;
    //delete snInt;

    return ret;
}

