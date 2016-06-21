#include "processmanager.h"
#include <QDebug>

ProcessManager::ProcessManager(QObject *parent) : QObject(parent)
{
    process=NULL;
}

ProcessManager::~ProcessManager()
{
    delete process;
}

void ProcessManager::runProcess(QString path, QString args)
{
    if(process)
    {
        disconnect(process, SIGNAL(started()), this, SLOT(processStarted()));
        disconnect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
        disconnect(process, SIGNAL(finished(int)), this, SLOT(processFinished(int)));
        delete process;
    }
    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    connect(process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int)), this, SLOT(processFinished(int)));

    QStringList arguments = args.split(" ",QString::SkipEmptyParts);
    process->start(path, arguments);
}

void ProcessManager::abort()
{
    process->kill();
}

void ProcessManager::processStarted()
{
    qDebug() << "processStarted()"<<"pid="<<process->processId()<<"arguments:"<<process->arguments();
}

void ProcessManager::processError(QProcess::ProcessError error)
{
    qDebug() << "processError()" << error << process->errorString() << "arguments"<<process->arguments()<<"output:"<<process->readAll();
}

void ProcessManager::processFinished(int exitCode)
{
    qDebug() << "processFinished()" << exitCode << process->readAll();
}
