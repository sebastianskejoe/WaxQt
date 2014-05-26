#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QWidget>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QItemDelegate>

class CheckboxDelegate : public QItemDelegate
{
public:
    CheckboxDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

private:
    int checkboxCol = 0;
};

#endif // CHECKBOXDELEGATE_H
