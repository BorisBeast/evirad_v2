#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>
#include <QProcess>

class ProcessManager : public QObject
{
    Q_OBJECT
public:
    explicit ProcessManager(QObject *parent = 0);
    ~ProcessManager();

signals:
    void finished(QString output);

public slots:
    void runProcess(QString path, QString args="");
    void abort();

private slots:
    void processStarted();
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode);

private:
    QProcess* process;
};

#endif // PROCESSMANAGER_H
