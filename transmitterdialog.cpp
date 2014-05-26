#include "transmitterdialog.h"
#include "ui_newtransmitter.h"
#include "waxqt.h"

#include <QSettings>
#include <QErrorMessage>
#include <QPushButton>

TransmitterDialog::TransmitterDialog(WaxQt *parent, int deviceid) :
    QDialog(parent),
    ui(new Ui::NewTransmitter)
{
    ui->setupUi(this);


    QPushButton *init = new QPushButton(tr("Initialize device"));
    ui->buttonBox->addButton(init, QDialogButtonBox::ResetRole);
    connect(init, SIGNAL(clicked()), this, SLOT(initializeDevice()));

    // Set up combobox
    ui->freqEdit->addItem(tr("12.5 Hz, 2 g, low power"),  215);
    ui->freqEdit->addItem(tr("25 Hz, 2 g, low power"),    216);
    ui->freqEdit->addItem(tr("50 Hz, 2 g, low power"),    217);
    ui->freqEdit->addItem(tr("100 Hz, 2 g, low power"),   218);
    ui->freqEdit->addItem(tr("200 Hz, 2 g, low power"),   219);

    ui->freqEdit->insertSeparator(99);

    ui->freqEdit->addItem(tr("12.5 Hz, 4 g, low power"),  151);
    ui->freqEdit->addItem(tr("25 Hz, 4 g, low power"),    152);
    ui->freqEdit->addItem(tr("50 Hz, 4 g, low power"),    153);
    ui->freqEdit->addItem(tr("100 Hz, 4 g, low power"),   154);
    ui->freqEdit->addItem(tr("200 Hz, 4 g, low power"),   155);

    ui->freqEdit->insertSeparator(99);

    ui->freqEdit->addItem(tr("12.5 Hz, 8 g, low power"),  87);
    ui->freqEdit->addItem(tr("25 Hz, 8 g, low power"),    88);
    ui->freqEdit->addItem(tr("50 Hz, 8 g, low power"),    89);
    ui->freqEdit->addItem(tr("100 Hz, 8 g, low power"),   90);
    ui->freqEdit->addItem(tr("200 Hz, 8 g, low power"),   91);

    ui->freqEdit->insertSeparator(99);

    ui->freqEdit->addItem(tr("12.5 Hz, 16 g, low power"), 23);
    ui->freqEdit->addItem(tr("25 Hz, 16 g, low power"),   24);
    ui->freqEdit->addItem(tr("50 Hz, 16 g, low power"),   25);
    ui->freqEdit->addItem(tr("100 Hz, 16 g, low power"),  26);
    ui->freqEdit->addItem(tr("200 Hz, 16 g, low power"),  27);

    ui->freqEdit->insertSeparator(99);

    ui->freqEdit->addItem(tr("12.5 Hz, 2 g, normal power"),  199);
    ui->freqEdit->addItem(tr("25 Hz, 2 g, normal power"),    200);
    ui->freqEdit->addItem(tr("50 Hz, 2 g, normal power"),    201);
    ui->freqEdit->addItem(tr("100 Hz, 2 g, normal power"),   202);
    ui->freqEdit->addItem(tr("200 Hz, 2 g, normal power"),   203);

    ui->freqEdit->insertSeparator(99);

    ui->freqEdit->addItem(tr("12.5 Hz, 4 g, normal power"),  135);
    ui->freqEdit->addItem(tr("25 Hz, 4 g, normal power"),    136);
    ui->freqEdit->addItem(tr("50 Hz, 4 g, normal power"),    137);
    ui->freqEdit->addItem(tr("100 Hz, 4 g, normal power"),   138);
    ui->freqEdit->addItem(tr("200 Hz, 4 g, normal power"),   139);

    ui->freqEdit->insertSeparator(99);

    ui->freqEdit->addItem(tr("12.5 Hz, 8 g, normal power"),  71);
    ui->freqEdit->addItem(tr("25 Hz, 8 g, normal power"),    72);
    ui->freqEdit->addItem(tr("50 Hz, 8 g, normal power"),    73);
    ui->freqEdit->addItem(tr("100 Hz, 8 g, normal power"),   74);
    ui->freqEdit->addItem(tr("200 Hz, 8 g, normal power"),   75);

    ui->freqEdit->insertSeparator(99);

    ui->freqEdit->addItem(tr("12.5 Hz, 16 g, normal power"), 7);
    ui->freqEdit->addItem(tr("25 Hz, 16 g, normal power"),   8);
    ui->freqEdit->addItem(tr("50 Hz, 16 g, normal power"),   9);
    ui->freqEdit->addItem(tr("100 Hz, 16 g, normal power"),  10);
    ui->freqEdit->addItem(tr("200 Hz, 16 g, normal power"),  11);

    QSettings settings;
    int len = settings.beginReadArray("devices");
    if (deviceid == -1) {
        ui->deviceIdEdit->setValue(len+1);
        saveIndex = len;
        save = false;
    } else {
        for (int i = 0; i < len; i++) {
            settings.setArrayIndex(i);
            if (settings.value("id").toInt() == deviceid) {
                saveIndex = i;
                save = true;
                ui->deviceIdEdit->setValue(deviceid);
                ui->deviceNameEdit->setText(settings.value("name").toString());
                ui->actThresEdit->setValue(settings.value("actThreshold").toInt());
                ui->inactThresEdit->setValue(settings.value("inactThreshold").toInt());
                ui->channelEdit->setValue(settings.value("channel").toInt());
                ui->actTimeEdit->setValue(settings.value("actTime").toInt());
                ui->tranint->setValue(settings.value("tranInt").toInt());

                int index = ui->freqEdit->findData(settings.value("frequency"));
                ui->freqEdit->setCurrentIndex(index);
                break;
            }
        }
    }
    settings.endArray();
}

void TransmitterDialog::accept() {
    QSettings settings;
    int len = settings.beginReadArray("devices");
    int i;
    if (!save) {
        for (i = 0; i < len; i++) {
            settings.setArrayIndex(i);
            if (settings.value("id").toInt() == ui->deviceIdEdit->value()) {
                QErrorMessage *msgdialog = new QErrorMessage(this);
                msgdialog->showMessage(tr("Device ID already used."));
                return;
            }
        }
    }
    settings.endArray();

    // No clashing ids - save device
    settings.beginWriteArray("devices");
    settings.setArrayIndex(saveIndex);
    settings.setValue("name", ui->deviceNameEdit->text());
    settings.setValue("id", ui->deviceIdEdit->value());
    settings.setValue("actThreshold", ui->actThresEdit->value());
    settings.setValue("inactThreshold", ui->inactThresEdit->value());
    settings.setValue("actTime", ui->actTimeEdit->value());
    settings.setValue("frequency", ui->freqEdit->currentData().toInt());
    settings.setValue("tranInt", ui->tranint->value());
    settings.endArray();

    QDialog::accept();
}

void TransmitterDialog::initializeDevice() {
    QString command = QString("DEVICE=%1\r\nACTTHRES=%2\r\nINACTTHRES=%3\r\nACTTIME=%4\r\nTRANINT=%5\r\nFREQ=%6\r\n")
            .arg(ui->deviceIdEdit->value())
            .arg(ui->actThresEdit->value())
            .arg(ui->inactThresEdit->value())
            .arg(ui->actTimeEdit->value())
            .arg(ui->tranint->value())
            .arg(ui->freqEdit->currentData().toInt());

    emit commandCreated(command);
}
