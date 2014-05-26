#ifndef LOGGERWORKER_H
#define LOGGERWORKER_H

#include <QObject>
#include "waxrec.h"

class LoggerWorker : public QObject
{
    Q_OBJECT
public slots:
    void doWork(int, bool*);

signals:
    void gotPacket(WaxPacket*);
    void droppedPacket();

private:
    bool mKill;
};

#endif // LOGGERWORKER_H
