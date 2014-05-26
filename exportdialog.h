#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QList>
#include "device.h"

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = 0);
    void setDevices(QList<Device*> devices);
    ~ExportDialog();

public slots:
    void accept();

private slots:
    void on_browse_clicked();

private:
    Ui::ExportDialog *ui;
    QList<Device*> mDevices;
};

#endif // EXPORTDIALOG_H
