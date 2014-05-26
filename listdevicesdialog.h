#ifndef LISTDEVICESDIALOG_H
#define LISTDEVICESDIALOG_H

#include <QWidget>
#include "ui_listdevices.h"

class ListDevicesDialog : public QDialog, public Ui::ListDevicesDialog
{
    Q_OBJECT
public:
    explicit ListDevicesDialog(QDialog *parent = 0);

signals:
    void deviceSelected(int);

public slots:
    void accept();

private:
    Ui::ListDevicesDialog *ui;
};

#endif // LISTDEVICESDIALOG_H
