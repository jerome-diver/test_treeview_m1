#include "files_treeview.h"

#include <QMessageBox>
#include <QAction>
#include <QMenu>

Files_treeView::Files_treeView(QWidget *parent) {
    this->setAcceptDrops(true);
    supprimer   = new QAction(tr("Supprimer"), this);
    QObject::connect(supprimer, &QAction::triggered, this, &Files_treeView::delete_row);
}


Files_treeView::~Files_treeView()
{
}

void Files_treeView::delete_row() {  // peut supprimer une ou plusieurs sélections et leurs fils
    int answer = QMessageBox::question(this,"SUPPRIMER",
                                       tr("voulez vous supprimer le(s) fichier(s)\n"
                                          "(ou répertoire(s)) que vous avez sélectionné\n"
                                          "de ce classeur ?"),
                                       tr("Oui"), tr("Non"));
    if (answer == 0) { // si la réponse est "oui" j'efface le fichier ou le dossier sélectionné...
        //emit delete_files(this->selectionModel()->selectedIndexes());
        while(!this->selectionModel()->selectedIndexes().isEmpty()) {
            QModelIndex idx = this->selectionModel()->selectedIndexes().first();
            this->model()->removeRow(idx.row(), idx.parent()); } }
}

void Files_treeView::setRoot(QStandardItem *the_root) {
    root = the_root;
    this->setColumnHidden(4,true);
}


void Files_treeView::mouseReleaseEvent ( QMouseEvent * event ) {
    if(event->button() == Qt::RightButton) {
        menu_middle = new QMenu(this);
        if (this->indexAt(event->pos()).isValid()) {
            menu_middle->addAction(supprimer);
            menu_middle->popup(this->viewport()->mapToGlobal(event->pos()));
        }
    }
    if(event->button() == Qt::MiddleButton)
    {
        QMessageBox *msgBox;
        int row, row_parent;
        QString parent_contain;
        QModelIndex my_index;
        my_index = this->indexAt( event->pos() );
        if (my_index.isValid()) {
            row = my_index.row();
            parent_contain = my_index.parent().data().toString();
            row_parent = my_index.parent().row();
            msgBox = new QMessageBox();
            msgBox->setWindowTitle("Hello");
            msgBox->setText(QString("You Clicked Middle Mouse Button\n"
                                    "On row :           %1\n"
                                    "of parent contain: %2\n"
                                    "parent is at row:  %3")
                            .arg(row)
                            .arg(parent_contain)
                            .arg(row_parent));
            msgBox->show(); }
    }
}
