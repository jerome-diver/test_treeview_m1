#include "files_model.h"

#include <QtGui>
#include <QMimeData>
#include <QMessageBox>
#include <QStandardItem>


Files_Model::Files_Model() {
    QStringList headers_files;
    headers_files << tr("Répertoire") << tr("Fichier")
                  << tr("Poids") << tr("Description") << tr("full");
    this->setColumnCount(4);
    this->setHorizontalHeaderLabels(headers_files);
}

Files_Model::~Files_Model()
{

}

Qt::DropActions Files_Model::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags Files_Model::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsDropEnabled | Qt::ItemIsEnabled;
    if (index.column() == 3)
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractItemModel::flags(index);
}

QMimeData *Files_Model::mimeData(const QModelIndexList &indexes) const {
     QMimeData *mimeData = new QMimeData();
     QByteArray encodedData;
     QDataStream stream(&encodedData, QIODevice::WriteOnly);
     foreach (QModelIndex index, indexes) {
         if (index.isValid()) {
             QString text = data(index, Qt::DisplayRole).toString();
             stream << text; } }
     mimeData->setData("application/vnd.text.list", encodedData);
     return mimeData;
}

bool Files_Model::dropMimeData(const QMimeData *data,
                               Qt::DropAction action,
                               int row, int column,
                               const QModelIndex &parent) {
     QStringList formatList;
     formatList = data->formats();
     if (action == Qt::IgnoreAction)
         return true;
     if (!data->hasFormat("text/uri-list"))
         return false;
     if (column > 0)        // interdit de coller ailleurs que dans la première colonne
         return false;
     if (parent.isValid())  // interdit de coller à l'intérieur d'un arbre
         return false;
     QList<QUrl> urlList;
     QList<QStandardItem*> item;
     urlList = data->urls(); // retrieve list of urls
     foreach(QUrl url, urlList)  {  // pour chaque entrée d'une sélection multiple
         if(!add_files(url, parent))
             return false; }  // ajoute les fichiers correspondants si aucun double
     return true;
}

QStringList Files_Model::mimeTypes() const {
    QStringList qstrList;
    qstrList.append("text/uri-list");    // list of accepted mime types for drop
    return qstrList;
}

bool Files_Model::add_files(const QUrl &url, const QModelIndex &parent) {
    QFileInfo file_info(url.toLocalFile());
    QString find_double = Check_doubles_for(file_info);
    if (!find_double.isEmpty())  // accès interdit aux doublons
            return false;
    if(url.toLocalFile() != "") { // traite chaque entrée pour créer les lignes du TreeView
        QString rsize;
        if (file_info.isDir()) {  // traite un répertoire et son contenu
            rsize = file_size_form(
                        file_dir_form(file_info.absoluteFilePath(),
                                      this->itemFromIndex(parent) ) ); }
        else {      // traite un fichier
            rsize = file_size_form(file_info.size());
            QList<QStandardItem*> items_line = prepareRow("","","","","");
            this->appendRow(items_line);
            populate_row(file_info, items_line, rsize);
            items_line[0]->setText(file_info.absolutePath()); } }
}

bool Files_Model::add_rows(const QStringList &name,
                           const QStringList &directory,
                           const QStringList &comment) {
    QString                        full, rsize;
    QStandardItem                 *rep;
    QList<QStandardItem*>          new_row;
    QHash<QString, QStandardItem*> dir_items;
    for(int i(0); i < name.count(); i++ ) {
        full = directory[i] + "/" + name[i];
        QString set_url = "file:" + full;
        QUrl url(set_url);
        QFileInfo file_info(url.toLocalFile());
        if(file_info.isDir())
            rsize = file_size_form(dir_size(file_info.absoluteFilePath()));
        else
            rsize = file_size_form(file_info.size());
        new_row.clear();
        new_row.append(new QStandardItem(name[i]));    // column 1
        new_row.append(new QStandardItem(rsize));      // column 2
        new_row.append(new QStandardItem(comment[i])); // column 3
        new_row.append(new QStandardItem(full));       // column 4
        if (file_info.isDir()) {  // directory
            QString absolute_dir = file_info.absoluteFilePath();
            QString relative_dir = file_info.canonicalPath() + "/";
            if(dir_items.contains(relative_dir)) {
                QRegExp regex("\\/.*\\/([\\w\\s_()&@-]*\\/)$");
                QString short_dir = absolute_dir;
                short_dir.replace(regex, "\\1");
                rep = new QStandardItem(short_dir);
                new_row.insert(0, rep);               // column 0
                dir_items[relative_dir]->appendRow(new_row); }
            else {
                rep = new QStandardItem(absolute_dir);
                new_row.insert(0, rep);               // column 0
                this->appendRow(new_row); }
            dir_items.insert( absolute_dir, rep); }
        else {       // file
            QString absolute_dir = file_info.absolutePath() + "/";
            if(dir_items.contains(absolute_dir)) {
                rep = new QStandardItem("");
                new_row.insert(0, rep);
                dir_items[absolute_dir]->appendRow(new_row); }
            else {
                rep = new QStandardItem(absolute_dir);
                new_row.insert(0, rep);
                this->appendRow(new_row); } } }
}

QModelIndex Files_Model::search(QString text,
                                int column,
                                const QModelIndex &parent) {
    QModelIndex target_index, target_zero, target_tilt;
    QString target_data;
    for(int i; i < this->rowCount(parent); i++) {
        target_index  = this->index(i ,column , parent);
        target_zero   = this->index(i, 0, parent);
        if(target_tilt.data().toString() == text)
            break;
        else if(target_index.data().toString() == text) {
            target_tilt = target_index;
            break; }
        else if(this->hasChildren(target_zero)) {
            target_tilt = search(text, column, target_zero);
        } }
    if(target_tilt.isValid()) {
        return target_tilt; }
}

QString Files_Model::Check_doubles_for(const QFileInfo &file_info) {
    QString file_find;
    QMessageBox msgbox;
    QString search_file = file_info.absoluteFilePath();
    QModelIndex idx_find;
    if (this->hasChildren()) {
        QModelIndex start = this->index(-1,0);
        idx_find = search(search_file, 4, start);
        if(idx_find.isValid()) {
            file_find = idx_find.data().toString();
            msgbox.setText(QString("Ce fichier existe déjà dans vôtre liste :\n%1")
                             .arg(file_find));
            msgbox.exec(); } }
    return file_find;
}


QString Files_Model::file_size_form(float size) {
    QString rendu_size;
    if (size > 1073741824) rendu_size = QString()
            .number(size/1073741824, 'f', 2) + " Go";
    if (size < 1073741824 && size > 1048576) rendu_size = QString()
            .number(size/1048576, 'f', 2) + " Mo";
    if (size < 1048576 && size > 1024) rendu_size = QString()
            .number(size/1024, 'f', 2) + " Ko";
    if (size < 1024)  rendu_size = QString("%1 o").arg(size);
    return rendu_size;
}

// ajoute une ligne par entrée contenu dans le répertoire et renvoit le poids total
// efface aussi de la liste les fichiers/répertoires qui existent déjà dans celui ajouté (à faire)
float Files_Model::file_dir_form(const QString &path,
                                 QStandardItem *item_parent) { // (parent ==> colonne 0)
    if (this->hasChildren()) {  // ici, si je trouve un contenu en double, je l'efface...
        QModelIndex find_double;
        QString search_double;
        search_double = QFileInfo(path).absoluteFilePath();
        find_double = search(search_double, 4, this->index(-1,0));
        if (find_double.isValid()) {
            QModelIndex to_remove = this->sibling(find_double.row(), 0, find_double.parent());
            this->removeRow(find_double.row(), to_remove.parent()); } }
    QDir dir(path);  // path est obligatoirement un répertoire
    float total_size(0), size_file(0);
    QString rendu_size;
    QList<QStandardItem*> item = prepareRow("","","","","");
    QList<QStandardItem*> item_file;
    if (item_parent == NULL) // ++ root à la première entrée
        this->appendRow(item);
    else {                    // ++ child par la suite
        item_parent->appendRow(item);
    } // donne l'id_parent (row number du parent)
    populate_row(QFileInfo(path), item, "--");  // écrit dans la ligne ajoutée
    foreach (QString file, dir.entryList(QDir::Files)) { // trouve chaque fichier
        size_file = QFileInfo(dir, file).size();  // poids du fichier
        total_size += size_file;  // ++ au poids dans ce répertoire uniquement
        rendu_size = file_size_form(size_file);
        item_file = prepareRow("","","","","");
        item.first()->appendRow(item_file);  // insère une ligne
        populate_row(QFileInfo(dir, file), item_file, rendu_size); }
    item[2]->setText(file_size_form(total_size)); // écrit le poids du répertoire
    foreach(QString subdir, dir.entryList(QDir::Dirs |   //  pour chaque sous-répertoire
                                          QDir::NoDotAndDotDot)) {  // incrémente cette même fonction
        populate_row(QFileInfo(path), item, "--");
        total_size += file_dir_form(path + QDir::separator() + subdir, item.first());
        item[2]->setText(file_size_form(total_size)); }
    return total_size;
}

float Files_Model::dir_size(const QString &path) {
    QDir dir(path);
    float total_size(0), size_file(0);
    QString rendu_size;
    foreach (QString file, dir.entryList(QDir::Files)) // trouve chaque fichier
        total_size += QFileInfo(dir, file).size();  // total ++ poids du fichier
    foreach(QString subdir, dir.entryList(QDir::Dirs |   //  pour chaque sous-répertoire
                                          QDir::NoDotAndDotDot))  // incrémente cette même fonction
        total_size += dir_size(path + QDir::separator() + subdir);
    return total_size;
}

QList<QStandardItem *> Files_Model::prepareRow(const QString &one,
                                               const QString &two,
                                               const QString &three,
                                               const QString &four,
                                               const QString &five)   {
     QList<QStandardItem *> rowItems;
     rowItems << new QStandardItem(one);
     rowItems << new QStandardItem(two);
     rowItems << new QStandardItem(three);
     rowItems << new QStandardItem(four);
     rowItems << new QStandardItem(five);
     return rowItems;
}

void Files_Model::populate_row(QFileInfo path,     // écrit dans la ligne
                                QList<QStandardItem*> items,
                                QString size) {
    if (path.isDir()) {
        if (!this->indexFromItem(items[0]).parent().isValid())
            items[0]->setText( path.absoluteFilePath() );
        else
            items[0]->setText( path.fileName() ); }
    else {
        items[1]->setText( path.fileName() );
        items[2]->setText(size); }
    items[4]->setText(path.absoluteFilePath());
}

