#ifndef WAXQT_H
#define WAXQT_H

#include <QMainWindow>
#include "waxrec.h"
#include <QHash>
#include <QVector>
#include <QThread>
#include "device.h"

namespace Ui {
class WaxQt;
}

class WaxQt : public QMainWindow
{
    Q_OBJECT

public:
    explicit WaxQt(QWidget *parent = 0);
    ~WaxQt();
    bool mKill;

signals:
    void startLogging(int, bool *);
    void stopLogging();

private slots:
    void on_addButton_clicked();
    void on_actionDevicesList_triggered();

    void on_connectPath_clicked();
    void on_startLogging_clicked();
    void on_exportRecordings_clicked();
    void on_fitGraphs_clicked();
    void on_clearGraphs_clicked();
    void on_showAll_stateChanged(int);

    void on_nextRec_clicked();
    void on_prevRec_clicked();

    void editDevice(QModelIndex index);

    void addDevice(int);
    void addLogEntry(WaxPacket*);
    bool sendcommand(QString cmd);
    void droppedPacket();

    void rowChanged(QModelIndex, QModelIndex);

private:
    Ui::WaxQt *ui;
    int fd;
    //QHash<int, QVector<WaxPacket> > packets;
    QHash<int, Device*> mDevices;
    //QHash<int, int> graphIds;
    QThread logThread;
    bool isLogging;

    void drawGraphs(int device, int recording = -1, QPen = QPen());
    void redrawAllGraphs();
    void clearGraphs();
    void fitGraphs();

    int currentDevice();
};

#endif // WAXQT_H
