#ifndef TRANSMITTERDIALOG_H
#define TRANSMITTERDIALOG_H

#include <QDialog>
#include "ui_newtransmitter.h"
#include "waxqt.h"

class TransmitterDialog : public QDialog, public Ui::NewTransmitter
{
    Q_OBJECT
public:
    explicit TransmitterDialog(WaxQt *parent = 0, int deviceid = -1);

signals:
    void commandCreated(QString);

public slots:
    void accept();

private slots:
    void initializeDevice();

private:
    Ui::NewTransmitter *ui;
    int saveIndex;
    bool save;
};

#endif // TRANSMITTERDIALOG_H
