#include "portserial.h"

#include <QThread>

PortSerial::PortSerial(QObject *parent)
{
    setParent(parent);
    m_address = "/dev/ttyUSB0";
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
    quint16 written = ::write(com, data.data(), data.length());
    if(written == data.length()){
        dtWrite = QDateTime::currentDateTime();
        countSent += data.length();
        Q_EMIT sent(data);
    }
    else{
        qCritical() << objectName() << "predato:" << data.length() << "poslato:" << written;
    }
}
void PortSerial::open()
{
    if(m_state == Opened) return;
    m_state = Opening;

    com = ::open(m_address.toLatin1(), O_RDWR | O_NOCTTY | O_NDELAY);
    //ovaj dio je za pseudo terminal
    char cpts[20];
    if(!ptsname_r(com, cpts, 20)){
        setAddress(cpts);
        qCritical()<< objectName() << " = pseudo terminal: " << cpts;
        grantpt(com);
        unlockpt(com);
    }
    //-----------------------------
    if (com == -1)
    {
        // Could not open the port.
        qCritical() << objectName() <<  " Ne mogu da otvorim port " << m_address;
        m_state = Closed;
    }
    else
    {
        fcntl(com, F_SETFL, 0);

        struct termios options;
        tcgetattr(com, &options);
        switch(m_baudRate){
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        case 38400:
            cfsetispeed(&options, B38400);
            cfsetospeed(&options, B38400);
            break;
        case 19200:
            cfsetispeed(&options, B19200);
            cfsetospeed(&options, B19200);
            break;
        case 9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case 4800:
            cfsetispeed(&options, B4800);
            cfsetospeed(&options, B4800);
            break;
        case 2400:
            cfsetispeed(&options, B2400);
            cfsetospeed(&options, B2400);
            break;
        case 1200:
            cfsetispeed(&options, B1200);
            cfsetospeed(&options, B1200);
            break;
        }

        options.c_cflag |= (CLOCAL | CREAD);
        options.c_oflag &= ~OPOST;

        if(m_parity == 0){
            options.c_cflag &= ~PARENB;
        }
        else {
            options.c_cflag |= PARENB;
            if(m_parity == 1)     options.c_cflag &= ~PARODD;
            else                options.c_cflag |= PARODD;
        }
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;

// EVEN (8E1):
//options.c_cflag |= PARENB;
//options.c_cflag &= ~PARODD;
//options.c_cflag &= ~CSTOPB;
//options.c_cflag &= ~CSIZE;
//options.c_cflag |= CS8;

        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNBRK);

        options.c_cflag &= ~CRTSCTS;
//                options.c_cflag |= CRTSCTS;
        //tcsetattr(com, TCSANOW, &options);

        options.c_cc[VINTR]    = 0;     /* Ctrl-c */
        options.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
        options.c_cc[VERASE]   = 0;     /* del */
        options.c_cc[VKILL]    = 0;     /* @ */
        options.c_cc[VEOF]     = 0;     /* Ctrl-d */
        options.c_cc[VTIME]    = 0;     /* inter-character timer unused */
        options.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
        options.c_cc[VSWTC]    = 0;     /* '\0' */
        options.c_cc[VSTART]   = 0;     /* Ctrl-q */
        options.c_cc[VSTOP]    = 0;     /* Ctrl-s */
        options.c_cc[VSUSP]    = 0;     /* Ctrl-z */
        options.c_cc[VEOL]     = 0;     /* '\0' */
        options.c_cc[VREPRINT] = 0;     /* Ctrl-r */
        options.c_cc[VDISCARD] = 0;     /* Ctrl-u */
        options.c_cc[VWERASE]  = 0;     /* Ctrl-w */
        options.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
        options.c_cc[VEOL2]    = 0;     /* '\0' */

//        cfmakeraw(&options);

        tcsetattr(com, TCSAFLUSH, &options);
        m_state = Opened;
    }
}
void PortSerial::close()
{
    m_state=Closing;
    ::close(com);
    com = 0;
}
void PortSerial::work()
{
    char comrxb[1];
    QByteArray rxb;
    while(m_state == Opened){
        fd_set read_fds, write_fds, except_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        FD_SET(com, &read_fds);

        // Set timeout to 100 miliseconds
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        // Wait for input to become ready or until the time out; the first parameter is
        // 1 more than the largest file descriptor in any of the sets
        if (select(com + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
        {
            int bytesRead = ::read(com, comrxb, 1);
            if(bytesRead == 1) {
                rxb.append(comrxb, 1);
                read(rxb);
                rxb.clear();
            }
            else if(bytesRead == 0){
                if(m_state == Opened) m_state = Opening;
                qCritical() << objectName() <<  " vratio 0 podataka => zatvoren port";
            }
        }
    }
}

QStringList PortSerial::getComList()
{
    int n;
    struct dirent **namelist;
    QStringList comList;
    QStringList comList8250;
    const char* sysdir = "/sys/class/tty/";

    // Scan through /sys/class/tty - it contains all tty-devices in the system
    n = scandir(sysdir, &namelist, NULL, NULL);
    if (n < 0)
        perror("scandir");
    else {
        while (n--) {
            if (strcmp(namelist[n]->d_name,"..") && strcmp(namelist[n]->d_name,".")) {

                // Construct full absolute file path
                QString devicedir = sysdir;
                devicedir += namelist[n]->d_name;

                // Register the device
                registerComport(comList, comList8250, devicedir);
            }
            free(namelist[n]);
        }
        free(namelist);
    }

    // Only non-serial8250 has been added to comList without any further testing
    // serial8250-devices must be probe to check for validity
    probeSerial8250Comports(comList, comList8250);

    // Return the lsit of detected comports
    return comList;
}


void PortSerial::registerComport(QStringList &comList, QStringList &comList8250, const QString &dir)
{
    // Get the driver the device is using
    QString driver = getDriver(dir);

    // Skip devices without a driver
    if (driver.size() > 0) {
        QString devfile = "/dev/" + QString(basename(dir.toLatin1()));

        // Put serial8250-devices in a seperate list
        if (driver == "serial8250") {
            comList8250.append(devfile);
        } else
            comList.append(devfile);
    }
}

QString PortSerial::getDriver(const QString &tty)
{
    struct stat st;
    QString devicedir = tty;

    // Append '/device' to the tty-path
    devicedir += "/device";

    // Stat the devicedir and handle it if it is a symlink
    if (lstat(devicedir.toLatin1(), &st)==0 && S_ISLNK(st.st_mode)) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Append '/driver' and return basename of the target
        devicedir += "/driver";

        if (readlink(devicedir.toLatin1(), buffer, sizeof(buffer)) > 0)
            return basename(buffer);
    }
    return "";
}

void PortSerial::probeSerial8250Comports(QStringList &comList, QStringList &comList8250)
{
    struct serial_struct serinfo;

    // Iterate over all serial8250-devices
    foreach (QString port, comList8250) {
        int fd = ::open(port.toLatin1(), O_RDWR | O_NONBLOCK | O_NOCTTY);

        if (fd >= 0) {
            // Get serial_info
            if (ioctl(fd, TIOCGSERIAL, &serinfo)==0) {
                // If device type is not PORT_UNKNOWN we accept the port
                if (serinfo.type != PORT_UNKNOWN)
                    comList.append(port);
            }
            ::close(fd);
        }
    }
}
