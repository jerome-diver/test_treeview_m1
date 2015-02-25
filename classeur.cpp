#include "classeur.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>


Classeur::Classeur(const QVector<QVariant> &data,
                   Classeur *parent)
{
    parentClasseur = parent;
    classeurData   = data;
    if(data.count() == 5) {
        if(classeurData[2] == 0)
            classeurData[2] = id_tree_MAX + 1;
        if(classeurData[2].toInt() > id_tree_MAX)
            id_tree_MAX = classeurData[2].toInt();
        files_model = new Files_Model();
        files_root  = files_model->invisibleRootItem();
        files_model->setColumnCount(4); }
}

Classeur::~Classeur()
{
    qDeleteAll(childsClasseur);
 //   qDeleteAll(Classeur_Info.files_model);
 //   qDeleteAll(Classeur_Info.files_root);
}

int Classeur::id_tree_MAX = 0;

Classeur *Classeur::child(int number) {
    return childsClasseur.value(number);
}

QList<Classeur*> Classeur::childs() {
    return childsClasseur;
}

int Classeur::childNumber() const {
    if(parentClasseur)
        return parentClasseur->childsClasseur.indexOf(
                    const_cast<Classeur*>(this));
    return 0;
}

bool Classeur::hasChildren() {
    if(childsClasseur.isEmpty())
        return false;
    return true;
}

bool Classeur::insertChildren(int position,
                              int count) {
    if(position < 0 ||
       position > childsClasseur.size())
        return false;
    for(int row = 0; row < count; ++row) {
        id_tree_MAX++;
        QVector<QVariant> datas(5);
        datas[2] = id_tree_MAX;
        Classeur *classeur = new Classeur(datas, this);
        childsClasseur.insert(position, classeur);
    }
    return true;
}

bool Classeur::removeChildren(int position,
                              int count) {
    if (position < 0 ||
        position + count > childsClasseur.size())
        return false;
    for(int row(0); row < count; ++row)
        delete childsClasseur.takeAt(position);
    return true;
}

int Classeur::childCount() const {
    if(!childsClasseur.isEmpty())
        return childsClasseur.count();
    return 0;
}

QVariant Classeur::data(int column) const {
    return classeurData.value(column);
}

bool Classeur::setData(int column,
                       const QVariant &value) {
    if(column < 0 ||
       column > classeurData.size() - 1)
        return false;
    classeurData[column] = value;
    return true;
}

Classeur* Classeur::parent() {
    return parentClasseur;
}

Files_Model* Classeur::getFiles_model() {
    return files_model;
}

QStandardItem* Classeur::getFiles_root() {
    return files_root;
}

int Classeur::getID_tree() {
    return classeurData[2].toInt();
}

int Classeur::getID_tree_parent() {
    return classeurData[3].toInt();
}

int Classeur::getID_classeur() {
    if(!classeurData[4].isNull())
        return classeurData[4].toInt();
    return 0;
}

int Classeur::get_id_MAX() {
    return id_tree_MAX;
}

QVariant Classeur::getName() {
    return classeurData[0];
}

QVariant Classeur::getComment() {
    return classeurData[1];
}

bool Classeur::isRoot() {
    if(parentClasseur)
        return false;
    return true;
}

void Classeur::razId_tree_Max() {
    id_tree_MAX = 0;
}
