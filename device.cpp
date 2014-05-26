#include "device.h"
#include <QPen>
#include <QColor>

Device::Device(QCustomPlot *xplot, QCustomPlot *yplot, QCustomPlot *zplot, QStandardItem *item)
{
    recordCountLabel = item;
    currentRec = -1;
    plots << xplot << yplot << zplot;
}

void Device::addPacket(WaxPacket *wp) {
    if (!recording || currentRec < 0 || currentRec >= recs.length()) {
        return;
    }

    recs[currentRec]->addPacket(wp);
}

void Device::startRecording() {
    recs << new Recording(plots);
    recording = true;
    ++currentRec;
}

void Device::stopRecording() {
    recording = false;
}

bool Device::isRecording() {
    return recording;
}

QList<Recording *> Device::recordings() {
    return recs;
}

int Device::countRecordings() {
    return recs.size();
}

QList<QCPGraph*> Device::graphs(int rec) {
    if (rec < 0) {
        rec = currentRec;
    }

    if (rec >= recs.size()) {
        qDebug() << "EMPTY LIST";
        return QList<QCPGraph*>();
    }

    return recs[rec]->graphs();
}

/**
 * Recordings
 */
Recording::Recording(QList<QCustomPlot*> plots) {
    grphs << new QCPGraph(plots[0]->xAxis,plots[0]->yAxis)
            << new QCPGraph(plots[1]->xAxis,plots[1]->yAxis)
            << new QCPGraph(plots[2]->xAxis,plots[2]->yAxis);
    mPackets = QList<WaxPacket>();

    samples = 0;
}

void Recording::addPacket(WaxPacket *wp) {
    int s;
    mPackets.append(*wp);
    for (s = 0; s < wp->sampleCount; ++s, ++samples) {
        grphs[0]->addData(samples, wp->samples[s].x / 256.0f * 9.82f);
        grphs[1]->addData(samples, wp->samples[s].y / 256.0f * 9.82f);
        grphs[2]->addData(samples, wp->samples[s].z / 256.0f * 9.82f);
    }
}

QList<WaxPacket> Recording::packets() {
    return mPackets;
}

QList<QCPGraph*> Recording::graphs() {
    return grphs;
}

QColor generateColor() {
    qreal hue;

    qreal lightness = 0.5;
    qreal saturation = 0.8;

    hue = static_cast<qreal>(qrand() % 25) / 25;
    return QColor::fromHslF(hue, saturation, lightness);
}
