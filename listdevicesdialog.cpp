#include "listdevicesdialog.h"
#include "ui_listdevices.h"
#include <QSettings>
#include <QListWidgetItem>
#include <QDialogButtonBox>
#include <QPushButton>

ListDevicesDialog::ListDevicesDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::ListDevicesDialog)
{
    ui->setupUi(this);

    QPushButton *addButton = new QPushButton(tr("Add"));
    ui->buttonBox->addButton(addButton, QDialogButtonBox::AcceptRole);

    QSettings settings;
    QString name;
    QListWidgetItem *item;
    int i, id;
    int len = settings.beginReadArray("devices");
    for (i = 0; i < len; i++) {
        settings.setArrayIndex(i);
        name = settings.value("name").toString();
        id = settings.value("id").toInt();
        item = new QListWidgetItem(QString("%1 (id: %2)").arg(name).arg(id), ui->listWidget);
        item->setData(Qt::UserRole, i);
    }
    settings.endArray();
}

void ListDevicesDialog::accept() {
    emit deviceSelected(ui->listWidget->currentItem()->data(Qt::UserRole).toInt());
    QDialog::accept();
}
