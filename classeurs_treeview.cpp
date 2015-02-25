#include "classeurs_treeview.h"
#include "classeur.h"
#include <QMessageBox>

Classeurs_treeView::Classeurs_treeView(QWidget *parent) {
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    supprimer   = new QAction(tr("Supprimer"), this);
    add         = new QAction(tr("Ajoute un classeur"), this);
    addChild    = new QAction(tr("Ajoute un classeur \"enfant\""), this);
    QObject::connect(supprimer, &QAction::triggered,
                     this, &Classeurs_treeView::delete_row);
    QObject::connect(add, &QAction::triggered,
                     this, &Classeurs_treeView::add_row);
    QObject::connect(addChild, &QAction::triggered,
                     this, &Classeurs_treeView::add_row_child);
}

Classeurs_treeView::~Classeurs_treeView() {

}

//****** Methods "get"  ****************************************************
Classeurs_Model *Classeurs_treeView::getModel() {
    return Classeurs.classeurs_model;
}

//****** Actions Classeur **************************************************
void Classeurs_treeView::delete_row() {  // supprimer une sélection ou plus
    int answer = QMessageBox::question(this,"SUPPRIMER",   // et leurs fils
                            tr("Voulez vous supprimer le(s) classeur(s) ?"),
                            tr("Oui"), tr("Non"));
    if (answer == 0) // si "oui" j'envois le signal pour effacer
        emit delete_classeur(this->selectionModel()->selectedIndexes());
}

void Classeurs_treeView::add_row() {
    emit add_classeur(this->currentIndex());
}

void Classeurs_treeView::add_row_child() {
    emit add_child_classeur(this->currentIndex());
}

//****** Event mouse click *************************************************
void Classeurs_treeView::mouseReleaseEvent ( QMouseEvent * event ) {
    if(event->button() == Qt::RightButton) {
        menu_middle = new QMenu(this);
        if (this->indexAt(event->pos()).isValid()) {
            menu_middle->addAction(supprimer);
            menu_middle->popup(this->viewport()->mapToGlobal(event->pos()));
            menu_middle->addAction(add);
            menu_middle->popup(this->viewport()->mapToGlobal(event->pos()));
            menu_middle->addAction(addChild);
            menu_middle->popup(this->viewport()->mapToGlobal(event->pos())); }
        else {
            menu_middle->addAction(add);
            menu_middle->popup(this->viewport()->mapToGlobal(event->pos())); } }
}

void Classeurs_treeView::mousePressEvent( QMouseEvent * event ) {
    if(event->button() == Qt::LeftButton) {
        QModelIndex idx = this->indexAt(event->pos());
        int cursor_pos_X = event->pos().rx();
        if (idx.isValid()) {
            emit clicked(this->currentIndex());
            int limit_L = setLimit(idx);
            if(cursor_pos_X >= limit_L &&
               cursor_pos_X <= limit_L + 21)
                this->setExpanded(idx, !(this->isExpanded(idx)));
            this->setCurrentIndex(idx); } }
}

int Classeurs_treeView::setLimit(QModelIndex index) {
    int limit(0);
    if(index.parent().isValid()) {
        limit += 21 + setLimit(index.parent());
    }
    return limit;
}
