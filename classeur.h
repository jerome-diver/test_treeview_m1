#ifndef CLASSEUR_H
#define CLASSEUR_H

#include <QObject>
#include <QSqlDatabase>
#include <QStandardItem>
#include <QStandardItemModel>
#include "files_model.h"


class Classeur
{

public:
    explicit Classeur(const QVector<QVariant> &data = QVector<QVariant>(5),
                      Classeur *parent = 0);
    ~Classeur();
    Classeur          *parent();
    Classeur          *child(int number);
    QList<Classeur *>  childs();
    int                childCount() const;
    QVariant           data(int column) const;
    bool               insertChildren(int position,
                                      int count);
    bool               removeChildren(int position,
                                      int count);
    int                childNumber() const;
    bool               setData(int column,
                               const QVariant &value);
    bool               hasChildren();
    int                get_id_MAX();
    Files_Model       *getFiles_model();
    QStandardItem     *getFiles_root();
    int                getID_tree();
    int                getID_tree_parent();
    int                getID_classeur();
    QVariant           getName();
    QVariant           getComment();
    bool               isRoot();
    void               razId_tree_Max();

private:
    QList<Classeur*>      childsClasseur;
    QVector<QVariant>     classeurData;
    Classeur             *parentClasseur;
    QStandardItem        *files_root;
    Files_Model          *files_model;
    static int            id_tree_MAX;

signals:

public slots:
};

#endif // CLASSEUR_H

