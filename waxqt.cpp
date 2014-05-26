#include "waxqt.h"
#include "ui_waxqt.h"
#include "transmitterdialog.h"
#include "listdevicesdialog.h"
#include "exportdialog.h"
#include "loggerworker.h"

#include "waxrec.h"
#include "device.h"

#include <iostream>
#include <unistd.h>

#include <QSettings>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QErrorMessage>
#include <QFileDialog>
#include <QTextStream>
#include <QList>
#include <QIcon>

WaxQt::WaxQt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaxQt)
{
    ui->setupUi(this);

    QIcon addIcon = QIcon::fromTheme("list-add");
    ui->addButton->setIcon(addIcon);
    QIcon delIcon = QIcon::fromTheme("list-remove");
    ui->deleteButton->setIcon(delIcon);

    QStandardItemModel *model = new QStandardItemModel(0,4,this);
    model->setHeaderData(1, Qt::Horizontal, tr("ID"));
    model->setHeaderData(2, Qt::Horizontal, tr("Device name"));
    model->setHeaderData(3, Qt::Horizontal, tr("Recordings"));

    QHeaderView *hv = ui->tableView->horizontalHeader();
    hv->setStretchLastSection(true);

    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QItemSelectionModel *sm = ui->tableView->selectionModel();
    connect(sm, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(rowChanged(QModelIndex,QModelIndex)));

    fd = -1;
    isLogging = false;

    // Log thread
    LoggerWorker *lw = new LoggerWorker();
    lw->moveToThread(&logThread);
    connect(&logThread, SIGNAL(finished()), lw, SLOT(deleteLater()));
    connect(lw, SIGNAL(gotPacket(WaxPacket*)), this, SLOT(addLogEntry(WaxPacket*)), Qt::QueuedConnection);
    connect(lw, SIGNAL(droppedPacket()), this, SLOT(droppedPacket()));
    connect(this, SIGNAL(startLogging(int, bool*)), lw, SLOT(doWork(int, bool*)), Qt::QueuedConnection);
    logThread.start();

    // Show devices
    QSettings settings;
    int len = settings.beginReadArray("devices");
    for (int i = 0; i < len; i++) {
        addDevice(i);
    }
    settings.endArray();
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editDevice(QModelIndex)));

    // Setup graphs
    ui->graphX->xAxis->setTickStep(5000);
    ui->graphY->xAxis->setTickStep(5000);
    ui->graphZ->xAxis->setTickStep(5000);

    ui->graphX->xAxis->setRange(0,4000);
    ui->graphY->xAxis->setRange(0,4000);
    ui->graphZ->xAxis->setRange(0,4000);

    ui->graphX->yAxis->setRange(-30,30);
    ui->graphY->yAxis->setRange(-30,30);
    ui->graphZ->yAxis->setRange(-30,30);

    ui->graphX->setInteraction(QCP::iRangeDrag, true);
    ui->graphY->setInteraction(QCP::iRangeDrag, true);
    ui->graphZ->setInteraction(QCP::iRangeDrag, true);
}

WaxQt::~WaxQt()
{
    delete ui;
    mKill = true;
    emit stopLogging();
    logThread.exit(0);
    logThread.wait();
}

//////////////////////////////
//// HARDWARE INTERFACING ////
//////////////////////////////

bool WaxQt::sendcommand(QString cmd) {
    std::cout << cmd.toStdString() << std::endl;
    if (fd < 0) {
        QErrorMessage *msgdialog = new QErrorMessage(this);
        msgdialog->showMessage(tr("Could not send command - device not connected"));
        return false;
    }

    // Make sure device is ready
    WAX_sendcommand(fd, "AT\r\n");
    unsigned char c;
    for (;;) {
        read(fd, &c, 1);
        if (c == 'A') {
            break;
        }
    }
    std::cout << "Read done" << std::endl;

    // Send command
    WAX_sendcommand(fd, cmd.toStdString().c_str());

    // Update ui
    ui->statusBar->showMessage(tr("Command sent!"));
    return true;
}

void WaxQt::on_connectPath_clicked() {
    fd = WAX_openport(ui->deviceBox->currentText().toStdString().c_str(), 1, 0);
    if (fd > 0) {
        ui->deviceBox->setEnabled(false);
        ui->connectPath->setEnabled(false);
        ui->statusBar->showMessage(tr("Connected to device"));
    }
}

////////////////////
//// DEVICES ///////
////////////////////

void WaxQt::editDevice(QModelIndex index) {
    int devid =  ui->tableView->model()->index(index.row(), 1).data().toInt();
    TransmitterDialog *td = new TransmitterDialog(this, devid);
    connect(td, SIGNAL(commandCreated(QString)), this, SLOT(sendcommand(QString)));
    td->show();
}

void WaxQt::addDevice(int i) {
    QSettings settings;
    settings.beginReadArray("devices");
    settings.setArrayIndex(i);

    QStandardItemModel *model = (QStandardItemModel *)ui->tableView->model();
    QList<QStandardItem*> list;
    QStandardItem *recLabel = new QStandardItem();
    QStandardItem *checkItem = new QStandardItem();
    checkItem->setCheckable(true);
    list.append(checkItem);
    list.append(new QStandardItem(settings.value("id").toString()));
    list.append(new QStandardItem(settings.value("name").toString()));
    list.append(recLabel);
    model->appendRow(list);

    Device *d = new Device(ui->graphX, ui->graphY, ui->graphZ, recLabel);
    d->ID = settings.value("id").toInt();
    mDevices[settings.value("id").toInt()] = d;

    settings.endArray();
}

void WaxQt::on_addButton_clicked() {
    TransmitterDialog *td = new TransmitterDialog();
    connect(td, SIGNAL(commandCreated(QString)), this, SLOT(sendcommand(QString)));
    td->show();
}

void WaxQt::on_actionDevicesList_triggered() {
    ListDevicesDialog *ld = new ListDevicesDialog();
    connect(ld, SIGNAL(deviceSelected(int)), this, SLOT(addDevice(int)));
    ld->show();
}

void WaxQt::on_exportRecordings_clicked() {
    QList<Device*> exportDevices;
    QStandardItemModel *model = (QStandardItemModel *)ui->tableView->model();
    int rows = model->rowCount();
    bool selected = false;

    Device *d;
    for (int i = 0; i < rows; i++) {
        if (model->item(i, 0)->checkState()) {
            d = mDevices[model->item(i,1)->text().toInt()];
            exportDevices.append(d);
            selected = true;
        }
    }

    if (!selected) {
        QErrorMessage *em = new QErrorMessage(this);
        em->showMessage(tr("No export devices selected"));
        return;
    }

    ExportDialog *ed = new ExportDialog();
    ed->setDevices(exportDevices);
    ed->show();
}

///////////////
/// LOGGING ///
///////////////

void WaxQt::addLogEntry(WaxPacket *wp) {
    if (isLogging && mDevices.contains(wp->deviceId)) {
        if (!mDevices[wp->deviceId]->isRecording()) {
            mDevices[wp->deviceId]->startRecording();

            mDevices[wp->deviceId]->recordCountLabel->setText(QString::number(mDevices[wp->deviceId]->countRecordings()));
            ui->currentRec->setText(QString::number(mDevices[wp->deviceId]->countRecordings()-1));

            redrawAllGraphs();

            // Set this as the selected device
            QStandardItemModel *model = (QStandardItemModel *)ui->tableView->model();
            int rows = model->rowCount();
            for (int i = 0; i < rows; i++) {
                if (model->item(i, 1)->text().toInt() == wp->deviceId) {
                    ui->tableView->selectRow(i);
                }
            }
        }
        mDevices[wp->deviceId]->addPacket(wp);

        // TODO: Drawing
        if (false) {
            ui->graphX->replot();
            ui->graphY->replot();
            ui->graphZ->replot();
        }
    }
}

void WaxQt::on_startLogging_clicked() {
    if (fd < 0) {
        ui->statusBar->showMessage("Could not start logging. Connect to device first.");
        return;
    }

    isLogging = !isLogging;
    QHash<int, Device*>::const_iterator i = mDevices.constBegin();
    if (isLogging) { // Start logging
        // Check whether to show all graphs
        if (!ui->showAll->isChecked()) {
            clearGraphs();
        }
        ui->startLogging->setText("Stop logging");
        mKill = false;
        emit startLogging(fd, &mKill);
    } else { // Stop logging
        while (i != mDevices.constEnd()) {
            i.value()->stopRecording();
            ++i;
        }
        fitGraphs();
        ui->startLogging->setText("Start logging");
        mKill = true;
        emit stopLogging();
        redrawAllGraphs();
    }
}

void WaxQt::droppedPacket() {
    ui->droppedPackets->setText(QString::number(ui->droppedPackets->text().toInt()+1));
}

////////////////
//// GRAPHS ////
////////////////

void WaxQt::redrawAllGraphs() {
    int cur = ui->currentRec->text().toInt();
    int device = currentDevice();
    if (device == -1) {
        return;
    }

    if (ui->showAll->checkState() == Qt::Checked) {
        // Loop through all recordings
        for (int i = 0; i < mDevices[device]->countRecordings(); ++i) {
//            qDebug() << "DRAW RECORDING: " << i;

            QColor col = QColor::fromHsl(i*50 % 255, 200, 100);
            QPen pen(col);

//            qDebug() << col;

            if (i == ui->currentRec->text().toInt()) {
                pen.setWidth(3);
            }

            drawGraphs(device, i, pen);
        }

        // Draw current graph in bold
    } else {
        // Only show one recording
        drawGraphs(device, cur);
    }
}

void WaxQt::drawGraphs(int device, int recording, QPen pen) {
    if (!mDevices.contains(device)) {
        return;
    }
    if (recording == -1) {
        recording = mDevices[device]->countRecordings()-1;
    }

    QList<QCPGraph*> graphs = mDevices[device]->graphs(recording);

    if (graphs.empty()) {
        qDebug() << "EMPTY GRAPHS";
        return;
    }

//    qDebug() << "DRAWING: " << device << ", " << recording;
//    qDebug() << "RECORDINGS: " << mDevices[device]->countRecordings();
//    qDebug() << "GRAPH COUNT: " << graphs.length();
//    qDebug() << "GRAPHS: " << graphs;

    graphs[0]->setPen(pen);
    graphs[1]->setPen(pen);
    graphs[2]->setPen(pen);

    ui->graphX->addPlottable(graphs[0]);
    ui->graphY->addPlottable(graphs[1]);
    ui->graphZ->addPlottable(graphs[2]);

    ui->graphX->replot();
    ui->graphY->replot();
    ui->graphZ->replot();
}

void WaxQt::clearGraphs() {
    ui->graphX->clearPlottables();
    ui->graphY->clearPlottables();
    ui->graphZ->clearPlottables();

    ui->graphX->replot();
    ui->graphY->replot();
    ui->graphZ->replot();
}

void WaxQt::fitGraphs() {
    ui->graphX->rescaleAxes();
    ui->graphY->rescaleAxes();
    ui->graphZ->rescaleAxes();

    ui->graphX->replot();
    ui->graphY->replot();
    ui->graphZ->replot();
}

void WaxQt::on_fitGraphs_clicked() {
    fitGraphs();
}

void WaxQt::on_clearGraphs_clicked() {
    clearGraphs();
}

////////////////////////
//// USER INTERFACE ////
////////////////////////

void WaxQt::rowChanged(QModelIndex cur, QModelIndex prev) {
    (void)cur;
    (void)prev;
    clearGraphs();
    redrawAllGraphs();
}

void WaxQt::on_showAll_stateChanged(int state) {
    if (state == Qt::Checked) {
        // Deactivate controls
        ui->currentRec->setReadOnly(true);
        ui->nextRec->setDisabled(true);
        ui->prevRec->setDisabled(true);

        // Loop through all recordings and show graphs
        redrawAllGraphs();
    } else {
        // Activate controls
        ui->currentRec->setReadOnly(false);
        ui->nextRec->setDisabled(false);
        ui->prevRec->setDisabled(false);

        // Show graph of recording in currentRec
        redrawAllGraphs();
    }
}

void WaxQt::on_nextRec_clicked() {
    int cur = ui->currentRec->text().toInt();
    int device = currentDevice();

    if (cur+1 == mDevices[device]->countRecordings()) {
        return;
    }

    ui->currentRec->setText(QString::number(cur+1));
    redrawAllGraphs();
}

void WaxQt::on_prevRec_clicked() {
    int cur = ui->currentRec->text().toInt();

    if (cur-1 < 0) {
        return;
    }

    ui->currentRec->setText(QString::number(cur-1));
    redrawAllGraphs();
}

/////////////////
//// HELPERS ////
/////////////////
int WaxQt::currentDevice() {
    // Check if we have a valid index
    QModelIndex index = ui->tableView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return -1;
    }

    int device = ((QStandardItemModel *)ui->tableView->model())->item(index.row(), 1)->text().toInt();
    return device;
}
