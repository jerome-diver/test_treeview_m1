
#ifndef CLASSEURS_TREEVIEW_H
#define CLASSEURS_TREEVIEW_H
#include "classeur.h"
#include "classeurs_model.h"

#include <QtGui>
#include <QObject>
#include <QTreeView>
#include <QStandardItem>
#include <QMenu>


struct infos_generales_classeurs {
    QList<int>                id;
    Classeurs_Model          *classeurs_model;
};  // structure de log des classeurs à insérer dans "Structures"


class Classeurs_treeView : public QTreeView
{
    Q_OBJECT

public:
    explicit Classeurs_treeView(QWidget *parent);
    ~Classeurs_treeView();
    infos_generales_classeurs Classeurs;
    Classeurs_Model          *getModel();

protected:
    QMenu    *menu_middle;
    QMenu    *menu_right;
    QAction  *supprimer;
    QAction  *add;
    QAction  *addChild;
    int       next_id;
    void      delete_row();
    void      add_row();
    void      add_row_child();
    int       setLimit(QModelIndex index);

protected slots:
    void mouseReleaseEvent ( QMouseEvent * event );
    void mousePressEvent( QMouseEvent * event );

signals:
    void clicked(QModelIndex);
    void delete_classeur(QModelIndexList list_indexs);
    void add_classeur(QModelIndex index);
    void add_child_classeur(QModelIndex index);

};


#endif // CLASSEURS_TREEVIEW_H

