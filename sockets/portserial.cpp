#include "portserial.h"

#include <QThread>

void PortSerial::read(QByteArray data)
{
    dtRead = QDateTime::currentDateTime();
    countReceived += data.length();
    Q_EMIT received(data);
}



void PortSerial::run()
{
    open:
    if(!m_skipOpen)
    {
        close();
        qWarning() << objectName() << " opening";
        open();
    }
    if(m_state == Opened){
        qWarning() << objectName() << " reading @" << this->m_baudRate<<"...";
        work();
        qWarning() << objectName() << " stopped";
        m_skipOpen=false;
    }
    if(m_state != Closing){
        sleep(5);
        goto open;
    }
}

QString PortSerial::getAddress()
{
    return m_address;
}
void PortSerial::setAddress(QString address)
{
    if(m_address.compare(address)){
        m_address = address;
        Q_EMIT addressChanged(address);
    }
}
int PortSerial::getStopBits()
{
    return m_stopBits;
}
void PortSerial::setStopBits(int stopBits)
{
    if(m_stopBits != stopBits){
        m_stopBits = stopBits;
        Q_EMIT stopBitsChanged(stopBits);
    }
}
int PortSerial::getByteSize()
{
    return m_byteSize;
}

PortSerial::State PortSerial::getState()
{
    return m_state;
}


void PortSerial::start()
{
    portThread.port = this;
    portThread.start();
}

void PortSerial::tryStart()  //prvo proba da otvori port pa tek onda pokrene thread
{
    m_skipOpen=false;
    close();
    open();
    if(m_state==Opened) {m_skipOpen = true; start();}
}

void PortSerial::setByteSize(int byteSize)
{
    if(byteSize != byteSize){
        m_byteSize = byteSize;
        Q_EMIT byteSizeChanged(byteSize);
    }
}

int PortSerial::getBaudRate()
{
    return m_baudRate;
}
void PortSerial::setBaudRate(int baudRate)
{
    if(m_baudRate != baudRate){
        m_baudRate = baudRate;
        Q_EMIT baudRateChanged(baudRate);
    }
}

int PortSerial::getParity()
{
    return m_parity;
}
void PortSerial::setParity(int parity)
{
    if(m_parity != parity){
        m_parity = parity;
        Q_EMIT parityChanged(parity);
    }
}
