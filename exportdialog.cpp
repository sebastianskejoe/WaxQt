#include "exportdialog.h"
#include "ui_exportdialog.h"
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include "waxrec.h"
#include <QErrorMessage>
#include <iostream>

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    QSettings settings;
    ui->directory->setText(settings.value("lastExportDirectory", ".").toString());
    ui->path->setText(settings.value("lastExportPath", "%d-%r.tsv").toString());
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::setDevices(QList<Device *> devices) {
    mDevices = devices;
}

void ExportDialog::on_browse_clicked() {
    QFileDialog *fd = new QFileDialog();
    fd->setFileMode(QFileDialog::Directory);
    fd->setOption(QFileDialog::ShowDirsOnly, true);
    if (fd->exec()) {
        ui->directory->setText(QString(fd->selectedFiles().first().data()));
    }
}

void ExportDialog::accept() {
    QList<Device*>::const_iterator di = mDevices.cbegin();
    std::cout << mDevices.size() << std::endl;
    while (di != mDevices.cend()) {
        std::cout << (*di)->ID << std::endl;
        QList<Recording*>::const_iterator ri = (*di)->recordings().cbegin();
        int rec = 1;
        while (ri != (*di)->recordings().cend()) {
            // Construct path
            QString path = QString("%1/%2").arg(ui->directory->text(), ui->path->text());
            path.replace(QString("%d"), QString::number((*di)->ID));
            path.replace(QString("%r"), QString::number(rec));

            // Write to file
            QFile file(path);
            if (file.exists()) {
                // Do something
            }

            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QErrorMessage *errmsg = new QErrorMessage(this);
                errmsg->showMessage("File couldn't be opened for output");
                ++ri;
                continue;
            }

            QTextStream out(&file);
            out << "Sample ID\tAcceleration X\tAcceleration Y\tAcceleration Z\n";
            QList<WaxPacket>::const_iterator pi = (*ri)->packets().cbegin();
            while (pi != (*ri)->packets().cend()) {
                for (int s = 0; s < (*pi).sampleCount; ++s) {
                    out << (*pi).samples[s].sampleIndex << "\t"
                        << (*pi).samples[s].x / 256.0f * 9.82f << "\t"
                        << (*pi).samples[s].y / 256.0f * 9.82f << "\t"
                        << (*pi).samples[s].z / 256.0f * 9.82f << "\n";
                }
                ++pi;
            }

            file.close();

            ++ri;
            ++rec;
        }
        ++di;
    }
    QSettings settings;
    settings.setValue("lastExportDirectory", ui->directory->text());
    settings.setValue("lastExportPath", ui->path->text());

    QDialog::accept();
}
