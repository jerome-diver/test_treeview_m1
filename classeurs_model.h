
#ifndef CLASSEURS_MODEL_H
#define CLASSEURS_MODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QFlags>
#include <QMultiHash>
#include "classeur.h"

enum order {
    ASC, DESC
};

struct database_classeurs_fields {
    QString        col_FK;  // ref to type of object link (id_project, id_structure, etc...)
    int            id_FK;   // id of the ref object linked
    QStringList    fields;  // name of fields names columns ofr classeur table
    QString        table;   // name of classeur table
    QString        linker_table; // name of linker table
};

class Classeurs_Model : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit Classeurs_Model(QObject *parent = 0);
    ~Classeurs_Model();
    QModelIndex    parent(const QModelIndex &child = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex    index(int                row,
                         int                column,
                         const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex    indexOf(Classeur *classeur);
    Qt::ItemFlags  flags(const QModelIndex &index = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant       data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool           setData(const QModelIndex &index,
                           const QVariant &value,
                           int role = Qt::EditRole) Q_DECL_OVERRIDE;
    int            columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int            rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant       headerData(int section,
                              Qt::Orientation orientation = Qt::Horizontal,
                              int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool           setHeaderData(int section,
                                 Qt::Orientation orientation,
                                 const QVariant &value,
                                 int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool           insertRows(int position,
                              int rows,
                              const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool           removeRows(int position,
                              int rows,
                              const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void           setModel(const QString         &colFK,
                            const int             &idFK,
                            const QStringList     &order_list,
                            const QStringList     &fields,
                            const QString         &table  = "Classeurs",
                            const QString         &fTable = "ClasseurRef",
                            const order           &type = ASC);
    void           setCol_FK(const QString &name);
    void           setTable(const QString &name);
    void           setLinkerTable(const QString &name);
    void           setFieldsNames(const QVector<QString> &fields);
    Classeur      *getClasseur(const QModelIndex &index = QModelIndex()) const;
    QModelIndex    getRootIndex();
    Classeur      *getRootClasseur();
    Files_Model   *getCurrent_Files_model(QModelIndex &index) const;
    bool           insertClasseur(const QVector<QVariant> &datas, int position,
                                  const QModelIndex     &parent = QModelIndex());
    bool           removeClasseur(const QModelIndex &index);
    bool           recordDatasToDB(int id_FK);
    //  int            totalRowCount(const QModelIndex &parent) const;

protected:
    Classeur                          *root_classeur;
    QMultiHash <int,              // id_parent =>
           QHash <int,         // id =>
                  QVector<QVariant> > >  db_Classeurs;  // [name, comment]
    QString                            query_str;
    database_classeurs_fields          db_coordonates;
    void           find_Childs(int id_parent,
                               Classeur *parent);

    bool           extractDatas();
    bool           deleteIntoDB(int id_classeur);
    bool           insertIntoDB(const QVector<QVariant> &fields);
    bool           updateDB(const QVector<QVariant> &fields);
    void           cleanDatasRange();
    bool           recordFiles(Classeur *parent);
    bool           recordFilesFromModel(Files_Model *files_model,
                                        QModelIndex parent,
                                        int id_classeur) ;

protected slots:

signals:


};



#endif // CLASSEURS_MODEL_H

