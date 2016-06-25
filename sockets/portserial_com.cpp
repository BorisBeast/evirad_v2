#include "portserial.h"

#include <QThread>

PortSerial::PortSerial(QObject *parent)
{
    setParent(parent);
    m_address = "COM1";
    m_baudRate = 9600;
    m_parity = 0;

    m_byteSize = 8;
    m_stopBits = 0;
    m_skipOpen = false;
//    QThread *thread;
//    thread = new QThread();
//    this->moveToThread(thread);
//    connect(thread, SIGNAL(started()), this, SLOT(run()));
////    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
////    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
////    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    thread->start();

}

void PortSerial::write(QByteArray data)
{
    unsigned long written;
    /*if(WriteFile(hCom, data.data(), (DWORD)data.size(), &written, NULL)){
        dtWrite = QDateTime::currentDateTime();
        countSent += data.length();
        Q_EMIT sent(data);
    }
    else{
        qCritical() << objectName() << "predato:" << data.length() << "poslato:" << written;
    }*/
    WriteFile(hCom, data.data(), (DWORD)data.size(), &written, &o_tx);
}
void PortSerial::open()
{
    m_state = PortSerial::Opening;

    hCom = CreateFile((LPCTSTR) m_address.data(),
                                        GENERIC_READ | GENERIC_WRITE,
                                        0,    // must be opened with exclusive-access
                                        NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                                        FILE_FLAG_OVERLAPPED,    //overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                                        );
    /*hCom = CreateFile((LPCTSTR) m_address.data(),
                                            GENERIC_READ | GENERIC_WRITE,
                                            0,    // must be opened with exclusive-access
                                            NULL, // no security attributes
                                            OPEN_EXISTING, // must use OPEN_EXISTING
                                            0,    //non-overlapped I/O
                                            NULL  // hTemplate must be NULL for comm devices
                                            );*/

    if (hCom == INVALID_HANDLE_VALUE)
    {
       qWarning("%s", ("CreateFile greska - port ne postoji"));
       m_state = PortSerial::Closed;
       return;
    }

#define  INPUT_BUFFER_LEN    256
#define  OUTPUT_BUFFER_LEN   512
    if ( !SetupComm(hCom,INPUT_BUFFER_LEN,OUTPUT_BUFFER_LEN) )
    {
        qWarning("%s", "SetupComm greska ");
        return;
    }

    fSuccess = GetCommState(hCom, &dcb);

    if (!fSuccess)
    {
       qWarning("%s", "GetCommState greska ");
       return;
    }

    dcb.BaudRate = m_baudRate;     // set the baud rate
    dcb.ByteSize = m_byteSize;             // data size, xmit, and rcv
    dcb.Parity = m_parity;        // no parity bit
    dcb.StopBits = m_stopBits;    // one stop bit
    //   dcb.BaudRate = 9600;     // set the baud rate
    //   dcb.ByteSize = 8;             // data size, xmit, and rcv
    //   dcb.Parity = NOPARITY;        // no parity bit
    //   dcb.StopBits = ONESTOPBIT;    // one stop bit
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    //dcb.fDtrControl = DTR_CONTROL_DISABLE;
    //dcb.fRtsControl = RTS_CONTROL_ENABLE;

    fSuccess = SetCommState(hCom, &dcb);

    if (!fSuccess)
    {
       qWarning("%s", "SetCommState greska ");
       return;
    }
    fSuccess = SetCommMask(hCom, EV_RXCHAR|EV_CTS|EV_DSR);
    if (!fSuccess)
    {
        qWarning("%s", "SetCommMask greska ");
        return;
    }

    /*timeouts.ReadIntervalTimeout = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;*/
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    fSuccess =SetCommTimeouts(hCom, &timeouts);
    if (!fSuccess) {
        qWarning("%s", "SetCommTimeouts greska ");
        return;
    }

    o_rx.hEvent = CreateEvent(
            NULL,   // default security attributes
            FALSE,  // auto reset event
            FALSE,  // not signaled
            NULL    // no name
    );
    // Initialize the rest of the OVERLAPPED structure to zero.
    o_rx.Internal = 0;
    o_rx.InternalHigh = 0;
    o_rx.Offset = 0;
    o_rx.OffsetHigh = 0;


//    assert(o_rx.hEvent);

    o_tx.hEvent = CreateEvent(
            NULL,   // default security attributes
            FALSE,  // auto reset event
            FALSE,  // not signaled
            NULL    // no name
    );
    // Initialize the rest of the OVERLAPPED structure to zero.
    o_tx.Internal = 0;
    o_tx.InternalHigh = 0;
    o_tx.Offset = 0;
    o_tx.OffsetHigh = 0;


//    assert(o_tx.hEvent);

    o_wt.hEvent = CreateEvent(
            NULL,   // default security attributes
            FALSE,  // auto reset event
            FALSE,  // not signaled
            NULL    // no name
    );
    // Initialize the rest of the OVERLAPPED structure to zero.
    o_wt.Internal = 0;
    o_wt.InternalHigh = 0;
    o_wt.Offset = 0;
    o_wt.OffsetHigh = 0;


//    assert(o_wt.hEvent);
    m_state = PortSerial::Opened;
}
void PortSerial::close()
{
    m_state=Closing;
    CloseHandle(hCom);
    if(hCom!=0)
        Sleep(200);   //TODO: zasto je ovo neophodno????? (nece da odmah ponovo otvori port ako ovoga nema)
    hCom=0;
}
void PortSerial::work()
{
    QByteArray rxb;
    DWORD dwBajtova;
    DWORD dwEvtMask;
    //BOOL result;
    char comrxb[1];
    while (m_state==Opened)
    {
        if (WaitCommEvent(hCom, &dwEvtMask, NULL))
        {
            if (dwEvtMask & EV_RXCHAR)
            {
                    dwBajtova = 1;
                    while (dwBajtova){
                            ReadFile(hCom, comrxb, 1, &dwBajtova, &o_rx);
                            if(dwBajtova){
                                rxb.append(comrxb, 1);
                                read(rxb);
                                rxb.clear();
                            }
                    }
            }
            if (dwEvtMask & EV_CTS)
            {
                DWORD lpModemStat;
                GetCommModemStatus(hCom, &lpModemStat);
                if(lpModemStat & MS_CTS_ON) {;}
                //TODO:
            }
            if (dwEvtMask & EV_DSR)
            {
                DWORD lpModemStat;
                GetCommModemStatus(hCom, &lpModemStat);
                if(lpModemStat & MS_DSR_ON) {;}
                //TODO:
            }
        }
        else {
            DWORD last_error = GetLastError();
            if(last_error == ERROR_OPERATION_ABORTED)  // disconnected ?
            {
                if(m_state == Opened) {
                    m_state = Opening;
                    qCritical() << objectName() <<  " vratio 0 podataka => zatvoren port";
                }
            }
        }
    }
}

QStringList PortSerial::getComList()
{
    QStringList ports;

    //What will be the return value from this function (assume the worst)
    //BOOL bSuccess = FALSE;

    HKEY hSERIALCOMM;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &hSERIALCOMM) == ERROR_SUCCESS)
    {
        //Enumerate all the values underneath HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM
        DWORD dwIndex = 0;
        DWORD dwType;
        DWORD dwValueNameSize = 1024;
        DWORD dwDataSize = 1024;
        TCHAR szValueName[1024];
        BYTE byValue[1024];
        LONG nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
        while (nEnum == ERROR_SUCCESS)
        {
          //If the value is of the correct type, then add it to the array
          if (dwType == REG_SZ)
          {
            TCHAR* szPort = reinterpret_cast<TCHAR*>(byValue);
            char port[1024];
            wcstombs(port, szPort, 1023);
            ports.append(port);
          }

          //Prepare for the next time around
          dwValueNameSize = 1024;
          dwDataSize = 1024;
          ++dwIndex;
          nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
        }

        //Close the registry key now that we are finished with it
        RegCloseKey(hSERIALCOMM);
    }

    return ports;
}
