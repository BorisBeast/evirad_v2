#include <QtGlobal>
#include <QThread>
#ifdef Q_OS_LINUX
#define PORTSERIAL_TTY
#endif
#ifdef Q_OS_WIN
#define PORTSERIAL_COM
#endif

#ifdef PORTSERIAL_TTY
    #include <termios.h> /* POSIX terminal control definitions */
    #include <fcntl.h>   /* File control definitions */
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <linux/serial.h>
#endif
#ifdef PORTSERIAL_COM
    #include <windows.h>
#endif

//------------------------------------------- TTY -------------------------------------------
#ifndef PORTSERIAL_H
#define PORTSERIAL_H

#include "socket.h"

class PortSerial : public Socket
{
    class PortThread: public QThread
    {
    public:
        PortSerial* port;
        virtual void run()
        {
            port->run();
        }
    };

    Q_OBJECT
    Q_INTERFACES(Socket)
    Q_PROPERTY(QString address READ getAddress WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int baudRate READ getBaudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(int parity READ getParity WRITE setParity NOTIFY parityChanged)
    Q_PROPERTY(int stopBits READ getStopBits WRITE setStopBits NOTIFY stopBitsChanged)
    Q_PROPERTY(int byteSize READ getByteSize WRITE setByteSize NOTIFY byteSizeChanged)

public:
    enum State {Unknown = 0, Closed, Closing, Opening, Opened};
    Q_INVOKABLE PortSerial(QObject *parent = 0);

public:
    QString getAddress();
    int getBaudRate();
    int getParity();
    int getStopBits();
    int getByteSize();
    State getState();
    void tryStart();
    static QStringList getComList();
public Q_SLOTS:
    void setAddress(QString);
    void setBaudRate(int);
    void setParity(int);
    void setStopBits(int);
    void setByteSize(int);
Q_SIGNALS:
    void byteSizeChanged(int);
    void stopBitsChanged(int);
    void baudRateChanged(int);
    void addressChanged(QString);
    void parityChanged(int);

public Q_SLOTS:
    //reimplemented IODevice
    virtual void write(QByteArray);
    virtual void read(QByteArray);
    //------------------------------
    virtual void run();
    virtual void open();
    virtual void close();
    virtual void work();
    virtual void start();

protected:
    State m_state;
    QString m_address;
    int m_baudRate;
    int m_parity;
    int m_stopBits;
    int m_byteSize;
    bool m_skipOpen;

private:
    PortThread portThread;


#ifdef    PORTSERIAL_TTY
    int com; /* File descriptor for the port */

private:
    static void registerComport( QStringList& comList, QStringList& comList8250, const QString& dir);
    static QString getDriver(const QString& tty);
    static void probeSerial8250Comports(QStringList& comList, QStringList& comList8250);
#endif
#ifdef    PORTSERIAL_COM
    HANDLE hCom;
    OVERLAPPED o_rx, o_tx, o_wt;
    DCB dcb;
    BOOL fSuccess;
    DWORD poslato;
    COMMTIMEOUTS timeouts;
#endif
};


#endif // PORTSERIAL_H

