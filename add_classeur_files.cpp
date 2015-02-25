#include "add_classeur_files.h"
#include "ui_add_classeur_files.h"

#include <QMessageBox>
#include <QMouseEvent>

add_classeur_files::add_classeur_files(QWidget *parent,
                                       QString name,
                                       Classeurs_Model *classeurs_model,
                                       Classeur *classeurParent) :
    QDialog(parent),
    ui_add_classeur_files(new Ui::add_classeur_files)
{
    ui_add_classeur_files->setupUi(this);
    ui_add_classeur_files->label_name->setStyleSheet("color: orange");
    QString c_name, c_comment;
    if(!classeurParent->isRoot()) {
        c_name = classeurParent->getName().toString();
        c_comment = classeurParent->getComment().toString(); }
    else {
        c_name = tr("pas de classeur parent");
        c_comment = ""; }
    ui_add_classeur_files->parent_name->setText(c_name);
    ui_add_classeur_files->parent_comment->setText(c_comment);
    model_classeur = classeurs_model;
    ui_add_classeur_files->label_name
            ->setText(tr("Nom du classeur pour \"%1\" :")
                      .arg(name));
}


add_classeur_files::~add_classeur_files()
{
    delete ui_add_classeur_files;
}

void add_classeur_files::on_lineEdi_name_textChanged(const QString &arg1) {
    New_classeur.name = QVariant(ui_add_classeur_files->lineEdi_name->text());
    if(!New_classeur.name.toString().isEmpty())
        ui_add_classeur_files->label_name->setStyleSheet("color:green");
    else
        ui_add_classeur_files->label_name->setStyleSheet("color: orange");

}

void add_classeur_files::on_textEdit_comment_textChanged() {
    New_classeur.comment = QVariant(ui_add_classeur_files
                                    ->textEdit_comment->toPlainText());
    if(!New_classeur.comment.toString().isEmpty())
        ui_add_classeur_files->label_comment->setStyleSheet("color:green");
    else
        ui_add_classeur_files->label_comment->setStyleSheet("color: white");
}

void add_classeur_files::on_pushButton_clicked() {
    if(New_classeur.name.toString().isEmpty()) {
        ui_add_classeur_files->label_name->setStyleSheet("color: red");
        QMessageBox::warning(this,tr("ATTENTION"),
                             tr("Vous n'avez pas nommé le Classeur\n"
                                "Il faut que vous donniez un nom au classeur, \n"
                                "sans quoi il ne pourra pas être créé."));
    }
    else
        accept();
}

