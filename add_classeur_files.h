#ifndef ADD_CLASSEUR_FILES_H
#define ADD_CLASSEUR_FILES_H

#include <QDialog>
#include <QStandardItemModel>
#include <classeurs_model.h>

namespace Ui {
class add_classeur_files;
}

class add_classeur_files : public QDialog
{
    Q_OBJECT
struct new_classeur {
    QVariant      name;
    QVariant      comment;
};


public:
    explicit add_classeur_files(QWidget *parent = 0,
                                QString name = "",
                                Classeurs_Model *classeurs_model = NULL,
                                Classeur *classeurParent = NULL);
//explicit add_classeur_files(QWidget *parent = 0,
//                            QString name = "",
//                            Classeurs_Model *classeurs_model = NULL);
    ~add_classeur_files();
    new_classeur   New_classeur;

private slots:
    void on_pushButton_clicked();
    void on_textEdit_comment_textChanged();
    void on_lineEdi_name_textChanged(const QString &arg1);

private:
    Ui::add_classeur_files    *ui_add_classeur_files;
    Classeurs_Model           *model_classeur;
  //  QStandardItemModel        *model_classeur;
    QModelIndex                last_parent;
    QHash<QString, QString>    classeur_e;
};

#endif // ADD_CLASSEUR_FILES_H
