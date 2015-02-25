
#include "classeurs_model.h"
#include "classeurs_treeview.h"
#include "classeur.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>

Classeurs_Model::Classeurs_Model(QObject *parent):
    QAbstractItemModel(parent) {

  //  user_controler *db;
  //  db->getInstance();
  //  db->initdb();
    db_coordonates.col_FK = "";
    db_coordonates.fields << "nom"     << "comment"
                          << "tree_id" << "tree_id_parent"
                          << "id";
    db_coordonates.id_FK = 0;
    db_coordonates.table = "Classeurs";
    db_coordonates.linker_table = "ClasseurRef";
    QVector<QVariant> headers;
    headers << tr("Classeurs") << tr("Description") << tr("id") << tr("id parent") << "ID";
    root_classeur = new Classeur(headers);
 //   for(int i(0); i < headers.count(); i++)
 //       setHeaderData(i, Qt::Horizontal, headers[i]);
}

Classeurs_Model::~Classeurs_Model() {

}

QModelIndex Classeurs_Model::parent(const QModelIndex &index) const {
    if(!index.isValid())
        return QModelIndex();
    Classeur *childClasseur = getClasseur(index);
    Classeur *parentClasseur = childClasseur->parent();
    if(parentClasseur == root_classeur)
        return QModelIndex();
    return createIndex(parentClasseur->childNumber(), 0, parentClasseur);
}

QModelIndex Classeurs_Model::index(int row,
                                   int column,
                                   const QModelIndex &parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    Classeur *parentClasseur = getClasseur(parent);
    Classeur *childClasseur = parentClasseur->child(row);
    if (childClasseur)
        return createIndex(row, column, childClasseur);
    else
        return QModelIndex();

}

QModelIndex Classeurs_Model::indexOf(Classeur *classeur) {
    return createIndex(0,0,classeur->parent());
}

Qt::ItemFlags  Classeurs_Model::flags(const QModelIndex &index) const {
    if(!index.isValid())
        return 0;
    return Qt::ItemIsEditable |
           Qt::ItemIsEnabled |
           Qt::ItemIsSelectable |
           QAbstractItemModel::flags(index);
}

QVariant Classeurs_Model::data(const QModelIndex &index,
                               int role) const {
    if(index.isValid() &&
            (role == Qt::DisplayRole ||
             role == Qt::EditRole)) {
        Classeur *classeur = getClasseur(index);
        return classeur->data(index.column()); }
    return QVariant();
}

bool Classeurs_Model::setData(const QModelIndex &index,
                              const QVariant &value,
                              int role) {
    if (role != Qt::EditRole)
            return false;
    Classeur *classeur = getClasseur(index);
    bool result = classeur->setData(index.column(), value);
    if (result) {
        QVector<int> role; role << Qt::EditRole;
        emit dataChanged(index, index, role); }
    return result;
}

int Classeurs_Model::columnCount(const QModelIndex &parent) const {
    return 4;
}

int Classeurs_Model::rowCount(const QModelIndex &parent) const {
    Classeur *parentClasseur = getClasseur(parent);
    return parentClasseur->childCount();
}

QVariant Classeurs_Model::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root_classeur->data(section);
    return QVariant();
}

bool Classeurs_Model::setHeaderData(int section,
                                    Qt::Orientation orientation,
                                    const QVariant &value,
                                    int role) {
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;
    bool result = root_classeur->setData(section, value);
    if (result)
        emit headerDataChanged(orientation, section, section);
    return result;
}

bool Classeurs_Model::insertRows(int position,
                                 int rows,
                                 const QModelIndex &parent) {
    Classeur *parentClasseur = getClasseur(parent);
    bool success;
    beginInsertRows(parent, position, position + rows - 1);
    success = parentClasseur->insertChildren(position, rows);
    endInsertRows();
    return success;
}

bool Classeurs_Model::insertClasseur(const QVector<QVariant> &datas,
                                     int position,
                                     const QModelIndex       &parent) {
    bool success, result;
    Classeur *parentClasseur = getClasseur(parent);
    beginInsertRows(parent, position, position);
    success = parentClasseur->insertChildren(position, 1);
    endInsertRows();
    if(success) {
        for(int column(0); column < 4; ++column) {
            if(column != 2) {
                result = parentClasseur->child(position)
                                       ->setData(column, datas[column]);
                if (!result)
                    return false; } } }
    if (success && result) {
        QVector<QVariant> db_datas = datas;
        db_datas[2] = parentClasseur->child(position)->getID_tree();
        QVector<QVariant> part3;
        part3 << db_datas[0] << db_datas[1];
        QHash<int, QVector<QVariant> > part2;
        part2.insert(db_datas[2].toInt(), part3);
        db_Classeurs.insert(db_datas[3].toInt(), part2);
        return true; }
    return false;
}

bool Classeurs_Model::removeClasseur(const QModelIndex &index) {
    bool success, result;
    Classeur *parentClasseur = getClasseur(index.parent());
    Classeur *classeur       = getClasseur(index);
    while(classeur->hasChildren()) {
        QModelIndex child = this->index(0,0,index);
        removeClasseur(child); }
    int id_parent = classeur->getID_tree_parent();
    int id = classeur->getID_tree();
    int id_classeur = classeur->getID_classeur();
    QVariant name = classeur->getName();
    QVariant comment = classeur->getComment();
    QHash<int, QVector<QVariant> > level2;
    QVector<QVariant> level3;
    level3 << name << comment;
    if(id_classeur != 0) {
        if(!deleteIntoDB(id_classeur))
            return false;
        level3 << id_classeur; }
    level2.insert(id, level3);
    db_Classeurs.remove(id_parent, level2);
    beginRemoveRows(index.parent(), index.row(), index.row());
    success = parentClasseur->removeChildren(index.row(), 1);
    endRemoveRows();
    if(success) {
        cleanDatasRange(); // redefine datas to be clear and well organized
        return true; }
    return false;
}

bool Classeurs_Model::removeRows(int position,
                                 int rows,
                                 const QModelIndex &parent) {
    Classeur *parentClasseur = getClasseur(parent);
    bool success = true;
    beginRemoveRows(parent, position, position + rows - 1);
    success = parentClasseur->removeChildren(position, rows);
    endRemoveRows();
    return success;
}
//int Classeurs_Model::totalRowCount(const QModelIndex &parent) const {
//}

void Classeurs_Model::setModel(const QString         &colFK,
                               const int             &idFK,
                               const QStringList     &order_list,
                               const QStringList     &fields,
                               const QString         &table,
                               const QString         &fTable,
                               const order           &type) {

    QString field_list;
    db_coordonates.col_FK       = colFK;
    db_coordonates.id_FK        = idFK;
    if(fields[0] != "")
        db_coordonates.fields       = fields;
    db_coordonates.table        = table;
    db_coordonates.linker_table = fTable;
    foreach(QString field, db_coordonates.fields)
        field_list += (!field_list.isEmpty()) ?
                    QString(", c.%1").arg(field) :
                    QString(" c.%1").arg(field);
    query_str = QString("SELECT %1 FROM \"%2\" c, \"%3\" r "
                        "WHERE c.id = r.id_classeur "
                        "AND r.%4 = %5 ;")
            .arg(field_list).arg(table)
            .arg(fTable).arg(colFK).arg(idFK);
    QString list_order;
    foreach(QVariant field, order_list)
        list_order += (!list_order.isEmpty()) ?
                    QString("ORDER BY (%1").arg(field.toString()) :
                    QString(", %2").arg(field.toString());
    list_order += ") ";
    switch (type) {
        case ASC:  list_order += "ASC "; break;
        case DESC: list_order += "DESC"; break; }
    query_str.insert(-1, list_order);
    extractDatas();
}


void Classeurs_Model::setCol_FK(const QString &name) {
    db_coordonates.col_FK = name;
}

void Classeurs_Model::setTable(const QString &name) {
    db_coordonates.table = name;
}

void Classeurs_Model::setLinkerTable(const QString &name) {
    db_coordonates.table = name;
}

void Classeurs_Model::setFieldsNames(const QVector<QString> &fields) {
    db_coordonates.fields.clear();
    foreach(QString field, fields)
        db_coordonates.fields << field;
}

bool Classeurs_Model::extractDatas() {
    QSqlQuery query;
    if (!query_str.isEmpty()) {
        query.prepare(query_str);
        query.exec();
        int c_name      = query.record().indexOf(db_coordonates.fields[0]);
        int c_comment   = query.record().indexOf(db_coordonates.fields[1]);
        int c_id        = query.record().indexOf(db_coordonates.fields[2]);
        int c_id_parent = query.record().indexOf(db_coordonates.fields[3]);
        int c_id_DB     = query.record().indexOf(db_coordonates.fields[4]);
        db_Classeurs.clear();
        while(query.next()) {
            QVector<QVariant> values;
            values << query.value(c_name)
                   << query.value(c_comment)
                   << query.value(c_id_DB);
            QHash<int, QVector<QVariant>> level2;
            level2.insert(query.value(c_id).toInt(), values);
            db_Classeurs.insert(query.value(c_id_parent).toInt(), level2); }
        find_Childs(0, root_classeur); }
}

void Classeurs_Model::find_Childs(int id_parent,
                                  Classeur *parent) {
    QList<QHash<int, QVector<QVariant>>>
          cols_id = db_Classeurs.values(id_parent);
    for(int c_id(0); c_id < cols_id.size(); ++ c_id) {
        QHash<int, QVector<QVariant>> col_id = cols_id[c_id];
        foreach(int id, col_id.keys()) {
            QVector<QVariant> row;
            row.clear();
            row << col_id.value(id)[0] << col_id.value(id)[1]
                << QVariant(id)        << QVariant(id_parent)
                << col_id.value(id)[2];
 //       QModelIndex idx = this->indexOf(parent);
 //       beginInsertRows(idx, parent->childCount(), parent->childCount());
            parent->insertChildren(parent->childCount(), 1);
 //       endInsertRows();
            for (int column = 0; column < 5; ++column)
                parent->child(parent->childCount() - 1)->setData(column, row[column]);
            if(!db_Classeurs.values(id).isEmpty())
                find_Childs(id, parent->child(parent->childCount() - 1)); } }
}

bool Classeurs_Model::recordDatasToDB(int id_FK) {
    db_coordonates.id_FK = id_FK;
    bool success(false);
    int new_ID;
    foreach(int id_parent, db_Classeurs.keys()) {
        QList<QHash<int, QVector<QVariant>>>
              parent_values = db_Classeurs.values(id_parent);
        for(int c_idP(0); c_idP < parent_values.size(); ++c_idP) {
            QHash<int, QVector<QVariant>> parent_value = parent_values[c_idP];
            foreach(int id, parent_value.keys()) {
                QVector<QVariant> datas;
                datas << parent_value[id][0]
                      << parent_value[id][1]
                      << QVariant(id)
                      << QVariant(id_parent);
                if(parent_value[id][2].isValid()) {
                    datas << parent_value[id][2];
                    success = updateDB(datas); }
                else
                    success = insertIntoDB(datas); } } }
    if(success)
        if(root_classeur->hasChildren())
            success = recordFiles(root_classeur);
    return success;
}

bool Classeurs_Model::recordFiles(Classeur *parent) {
    bool success(false);
    int rec_id_classeur(0);
    if(parent->hasChildren()) {
        foreach(Classeur *classeur, parent->childs()) {
            rec_id_classeur = classeur->getID_classeur();
            Files_Model *current_files_model = classeur->getFiles_model();
     //       success = this->recordFilesFromModel(current_files_model,
     //                                      current_files_model->index(0,0),
     //                                      rec_id_classeur);
            if(success && classeur->hasChildren())
                success = recordFiles(classeur); } }
    return success;
}

bool recordFilesFromModel(Files_Model *files_model,
                          QModelIndex parent,
                          int id_classeur) {
    QSqlQuery query;
    for(int i(0); i < files_model->rowCount(parent); i++) {
        QModelIndex Files_index = files_model->index(i, 0, parent);
        QRegExp tri(".*(\\.\\w*$)"); // sépare l'extention finale
        QString full_name = files_model->index(i, 4, parent).data().toString();
        QString name      = files_model->index(i, 1, parent).data().toString(); //(nom entier)
        QString extname   = full_name;
        extname.replace(tri, "\\1");
        QString directory;
        QString str_url = "file:" + full_name;
        QUrl url(str_url);
        QFileInfo file_url = url.toLocalFile();
        if(file_url.isDir()) {
            directory = file_url.absoluteFilePath();
            name = "";
            extname =""; }
        else
            directory = file_url.absolutePath();
        QString comment   = files_model->index(i, 3, parent).data().toString();
      //**** Search if File exist allready find id *****
        int rec_id_file(0);
        query.prepare("SELECT id FROM \"Files\" "
                      "WHERE name = :name "
                      "AND directory = :directory ;" );
        query.bindValue(":name", name);
        query.bindValue(":directory", directory);
        if(!query.exec())
            return false;
        while(query.next())
            rec_id_file = query.value(0).toInt();
        if (rec_id_file == 0 ) {  // if not exist
           //**** INSERT DATA SQL TABLE "Files" if not exist *************
            query.prepare("INSERT INTO \"Files\" "
                          "       ( name,  extname,  directory,  comment) "
                          "VALUES (:name, :extname, :directory, :comment) "
                          "RETURNING id ;");
            query.bindValue(":name", name);
            query.bindValue(":directory", directory);
            query.bindValue(":extname", extname);
            query.bindValue(":comment", comment);
            if(!query.exec())
                return false;
            while(query.next())
                rec_id_file = query.value(0).toInt(); }
      //**** INSERT Link: Classeur<->Files in "FileRef" ***********
        query.prepare("INSERT INTO \"FileRef\" "
                      "       ( id_classeur,  id_file) "
                      "VALUES (:id_classeur, :id_file) ;");
        query.bindValue(":id_classeur", id_classeur);
        query.bindValue(":id_file", rec_id_file);
        if(!query.exec())
            return false;
        if(files_model->hasChildren(Files_index))
            if(!recordFilesFromModel(files_model, Files_index, id_classeur))
                qDebug() << QObject::tr("Erreur")
                         << QObject::tr("Erreur lors de la recherche de fichiers"); }
    query.finish();
    query.clear();
    return true;
}

bool Classeurs_Model::deleteIntoDB(int id_classeur) {
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare(QString("DELETE FROM \"%1\" "
                          "WHERE id = :id_classeur ;")
                  .arg(db_coordonates.table));
    query.bindValue(":id_classeur",id_classeur);
    if(!query.exec())
        return false;
    QSqlDatabase::database().commit();
    query.clear();
    return true;
}

bool Classeurs_Model::insertIntoDB(const QVector<QVariant> &fields) {
    QSqlQuery query;
    query.prepare(QString("INSERT INTO \"%1\" "
                          "       ( %2,    %3,       %4,       %5) "
                          "VALUES ( :name, :comment, :tree_id, :tree_id_p) "
                          "RETURNING id ;").arg(db_coordonates.table)
                          .arg(db_coordonates.table)
                          .arg(db_coordonates.fields[0])
                          .arg(db_coordonates.fields[1])
                          .arg(db_coordonates.fields[2])
                          .arg(db_coordonates.fields[3]));
    query.bindValue(":name",      fields[0].toString());
    query.bindValue(":comment",   fields[1].toString());
    query.bindValue(":tree_id",   fields[2].toInt());
    query.bindValue(":tree_id_p", fields[3].toInt());
    if(!query.exec())
        return false;
    int new_ID;
    while(query.next())
        new_ID = query.value(0).toInt();
    // insert le nouvel ID dans db_Classeurs pour l'enregistrement des fichiers et leur lien
    int c_id = fields[2].toInt();
    int c_id_parent = fields[3].toInt();
   // QMultiHash<int, QHash<int, QVector<QVariant>>>::iterator
   //         i = db_Classeurs.find(c_id_parent);
   // while (i != db_Classeurs.end() && i.key() == c_id_parent) {
   //     if( i.value().contains(c_id) )
   //         *i[c_id] << QVariant(new_ID);
   //     ++i;
    //}
    query.clear();
    query.prepare(QString("INSERT INTO \":%1\" "
                          "       ( id_classeur, %2) "
                          "VALUES (:id_classeur, :id_FK) ;")
                  .arg(db_coordonates.linker_table)
                  .arg(db_coordonates.col_FK));
    query.bindValue(":id_classeur", new_ID);
    query.bindValue(":id_FK", db_coordonates.id_FK);
    if(!query.exec())
        return false;
    query.clear();
    return true;
}

bool Classeurs_Model::updateDB(const QVector<QVariant> &fields) {
    QSqlQuery query;
    query.prepare(QString("UPDATE \"%1\" "
                  "SET %2 = :nom, %3 = :comment, "
                  "%4 = :tree_id, %5 = :tree_id_parent "
                  "WHERE id = :id_classeur")
                  .arg(db_coordonates.table)
                  .arg(db_coordonates.fields[0])
                  .arg(db_coordonates.fields[1])
                  .arg(db_coordonates.fields[2])
                  .arg(db_coordonates.fields[3]));
    query.bindValue(":nom",           fields[0]);
    query.bindValue(":comment",       fields[1]);
    query.bindValue(":tree_id",       fields[2]);
    query.bindValue("tree_id_parent", fields[3]);
    query.bindValue("id_classeur",    fields[4]);
    if(!query.exec())
        return false;
    query.clear();
    return true;
}

void Classeurs_Model::cleanDatasRange() { // datas arrived delete by one
    QMap<int, QVector<QVariant>> datas; // format data
    foreach(int id_parent, db_Classeurs.keys()) {
        QList<QHash<int, QVector<QVariant>>>
              parent_values = db_Classeurs.values(id_parent);
        for(int c_idP(0); c_idP < parent_values.size(); ++c_idP) {
            QHash<int, QVector<QVariant>> parent_value = parent_values[c_idP];
            foreach(int id, parent_value.keys()) {
                QVector<QVariant> content;
                content << QVariant(id_parent)
                        << parent_value[id][0]
                        << parent_value[id][1];
                datas.insert(id, content); } } }
    QMap<int, QVector<QVariant>>::iterator id; // remove break count
    bool finish(false);
    while(!finish) {
        int i(0);
        for(id = datas.begin(); id != datas.end(); ++id) {
            i++;
            if(id.key() != i) {  // if a break inside the count
                datas.insert(i, id.value());
                datas.remove(id.key());
                finish = false;
                break; }
            finish = true; } }
    db_Classeurs.clear();
    foreach(int id, datas.keys()) { // rewrite db
        QHash<int, QVector<QVariant>> level2;
        QVector<QVariant> level3;
        level3  << datas[id][1]  // nom
                << datas[id][2];  // comment
        if(datas[id].count() == 4)
            level3 << datas[id][3]; // id_classeur (if exist)
        level2.insert(id, level3);
        db_Classeurs.insert(datas[id][0].toInt(), level2); }
}

Files_Model* Classeurs_Model::getCurrent_Files_model(QModelIndex &index) const {
    if(!index.isValid())
        return NULL;
    Classeur *classeur = reinterpret_cast<Classeur*>(index.internalPointer());
    return classeur->getFiles_model();
}

Classeur* Classeurs_Model::getClasseur(const QModelIndex &index) const {
    if(!index.isValid())
        return root_classeur;
    Classeur *classeur = reinterpret_cast<Classeur*>(index.internalPointer());
    return classeur;
}

Classeur* Classeurs_Model::getRootClasseur() {
    return root_classeur;
}

QModelIndex Classeurs_Model::getRootIndex() {
    return this->indexOf(root_classeur);
}
