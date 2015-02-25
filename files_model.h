
#ifndef FILES_MODEL_H
#define FILES_MODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QFileInfo>



class Files_Model : public QStandardItemModel
{
public:
    Files_Model();
    ~Files_Model();
    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDropActions() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    bool dropMimeData(const QMimeData *data,
                            Qt::DropAction action,
                            int row, int column,
                            const QModelIndex &parent);
    bool add_files(const QUrl &url, const QModelIndex &parent);
    bool add_rows(const QStringList &name,
                  const QStringList &directory,
                  const QStringList &comment);

protected:
    QString file_size_form(float size);
    float dir_size(const QString &path);
    float file_dir_form(const QString &path, QStandardItem *item_parent);
    void populate_row(QFileInfo path, QList<QStandardItem*> item, QString size);
    QList<QStandardItem *> prepareRow(const QString &one,   const QString &two,
                                      const QString &three, const QString &four, const QString &five);
    QString Check_doubles_for(const QFileInfo &file_info);
    QModelIndex search(QString text, int column, const QModelIndex &parent);
};



#endif // FILES_MODEL_H

