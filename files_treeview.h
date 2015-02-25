
#ifndef FILES_TREEVIEW_H
#define FILES_TREEVIEW_H

#include <QtGui>
#include <QObject>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMenu>
#include <QAction>

#include "files_model.h"
#include "classeur.h"

class Files_treeView : public QTreeView
{
    Q_OBJECT

public:
    Files_treeView(QWidget *parent);
    ~Files_treeView();
    void setRoot(QStandardItem *the_root);

private slots:
    void mouseReleaseEvent ( QMouseEvent * event );

signals:
    void delete_files(QModelIndexList list_indexs);

private:
    QMenu    *menu_middle;
    QMenu    *menu_right;
    QAction  *supprimer;
    QStandardItem *root;
    void delete_row();
};



#endif // FILS_TREEVIEW_H

