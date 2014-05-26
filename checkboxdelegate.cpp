#include "checkboxdelegate.h"

#include <QItemDelegate>
#include <QCheckBox>

CheckboxDelegate::CheckboxDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *CheckboxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    if (index.isValid() && index.column() == checkboxCol) {
        QCheckBox *editor = new QCheckBox(parent);
        editor->installEventFilter(const_cast<CheckboxDelegate*>(this));
        return editor;
    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void CheckboxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    if (index.isValid() && index.column() == checkboxCol) {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();

        QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
        if(value == "Y")
            checkBox->setCheckState(Qt::Checked);
        else
            checkBox->setCheckState(Qt::Unchecked);
    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

void CheckboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    if (index.isValid() && index.column() == checkboxCol) {
        QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
        QString value;
        if (checkBox->checkState() == Qt::Checked)
            value = "Y";
        else
            value = "N";

        model->setData(index, value);
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

void CheckboxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    if (index.isValid() && index.column() == checkboxCol)
        editor->setGeometry(option.rect);
    else
        QItemDelegate::updateEditorGeometry(editor, option, index);
}
