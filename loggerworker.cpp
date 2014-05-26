#include "loggerworker.h"
#include <iostream>
#include <QDebug>
#include <termios.h>

void LoggerWorker::doWork(int fd, bool *kill)
{
    const int BUFFER_SIZE = 1024;
    static char buffer[BUFFER_SIZE];
    WaxPacket *wp;

    tcflush(fd, TCIOFLUSH);
    qDebug() << "FLUSHED";
    while (!*kill) {
        int len = WAX_slipread(fd, buffer, BUFFER_SIZE);
        wp = WAX_parseWaxPacket(buffer, len, WAX_TicksNow());
        if (wp == 0) {
            emit droppedPacket();
            continue;
        }
        emit gotPacket(wp);
    }
}
