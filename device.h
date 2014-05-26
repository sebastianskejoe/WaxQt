#ifndef DEVICE_H
#define DEVICE_H

#include "qcustomplot.h"
#include "waxrec.h"

#include <QColor>

class Recording
{
public:
    Recording(QList<QCustomPlot*>);
    void addPacket(WaxPacket *);
    QList<WaxPacket> packets();
    QList<QCPGraph*> graphs();

private:
    QList<WaxPacket> mPackets;
    QList<QCPGraph*> grphs;
    int samples;
};

class Device
{
public:
    Device(QCustomPlot *xplot, QCustomPlot *yplot, QCustomPlot *zplot, QStandardItem *item);
    void addPacket(WaxPacket *wp);
    void startRecording();
    void stopRecording();
    bool isRecording();
    QList<Recording*> recordings();
    int countRecordings();
    QList<QCPGraph*> graphs(int rec = -1);
    QStandardItem *recordCountLabel;
    int ID;

private:
    bool recording;
    int currentRec;
    QList<Recording*> recs;
    QList<QCustomPlot*> plots;
};

QColor generateColor();

#endif // DEVICE_H
