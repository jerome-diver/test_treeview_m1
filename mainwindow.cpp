#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modeltest/modeltest.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSqlDatabase database =  QSqlDatabase::addDatabase("QPSQL");
    database.setDatabaseName("recorder");
    database.setHostName("localhost");
    if(!database.open(QString("admin"), QString("overover"))) {
        QString err = database.lastError().text();
        qDebug() << QString("La base de donnée ne s'est pas "
                            "connectée correctement:\n%1")
                            .arg(err); }
    classeurs_model = new Classeurs_Model;
    classeurs_model->setCol_FK("id_project");
    ui->classeurs->setModel(classeurs_model);
    ui->classeurs->setStyleSheet("selection-background-color: blue");
    ui->classeurs->hideColumn(2);
    ui->classeurs->hideColumn(3);
    ui->classeurs->hideColumn(4);
    ui->classeurs->setSortingEnabled(true);
 //   QObject::connect(ui->classeurs,
 //                    SIGNAL(clicked(QModelIndex)),
 //                    this,
 //                    SLOT(on_classeurs_clicked(QModelIndex)));
    QObject::connect(ui->classeurs,
                     SIGNAL(delete_classeur(QModelIndexList)),
                     this,
                     SLOT(on_classeurs_delete(QModelIndexList)));
    QObject::connect(ui->classeurs,
                     SIGNAL(add_classeur(QModelIndex)),
                     this,
                     SLOT(on_classeurs_add(QModelIndex)));
    QObject::connect(ui->classeurs,
                     SIGNAL(add_child_classeur(QModelIndex)),
                     this,
                     SLOT(on_classeurs_add_child(QModelIndex)));
    new ModelTest(classeurs_model, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


//********************  initialize:  classeurs  **************************************
void MainWindow::on_classeurs_clicked(QModelIndex index) {
    if(index.isValid()) {
        ui->files->reset();  // RAZ files of the tree table
        QModelIndex   index_Col0 = index.parent().sibling(index.row(),0);
        Classeur     *classeur   = classeurs_model->getClasseur(index_Col0);
        files_name               = classeur->getFiles_model();
        ui->files->setModel(files_name);
        ui->files->setColumnHidden(4,true);
    }
}

void MainWindow::on_classeurs_delete(QModelIndexList list_indexs) {
    qDebug(Q_FUNC_INFO);
    while(!list_indexs.isEmpty()) {
        QModelIndex idx = list_indexs.first();
        if(idx.column() == 0)
            classeurs_model->removeClasseur(idx);
        list_indexs.removeFirst(); }
}

void MainWindow::on_classeurs_add(QModelIndex index) {
    qDebug(Q_FUNC_INFO);
    idx_result = classeurs_model->sibling(index.row(),0,index);
    QString name_project = "test";
    Classeur *currentClasseur = classeurs_model->getClasseur(idx_result);
    add_classeur_files *dialog_add_classeur;
    dialog_add_classeur = new add_classeur_files(this, name_project,
                                                 classeurs_model, currentClasseur);
    int result = dialog_add_classeur->exec();
        if(result) {
            QVariant id_parent;
            int pos;
            if(idx_result.parent().isValid()) {
                QModelIndex oldIdxAtidP = classeurs_model->index(idx_result.row(), 3,
                                                                 idx_result.parent());
                id_parent = oldIdxAtidP.data();
                pos = idx_result.row() + 1; }
            else {
                id_parent = QVariant(0);
                pos = (idx_result.isValid()) ? idx_result.row() + 1 : 0; }
            QVector<QVariant> datas;
            datas << dialog_add_classeur->New_classeur.name
                  << dialog_add_classeur->New_classeur.comment
                  << QVariant("")
                  << id_parent;
            if(!classeurs_model->insertClasseur(datas, pos, idx_result.parent()))
                return; }
}

void MainWindow::on_classeurs_add_child(QModelIndex index) {
    qDebug(Q_FUNC_INFO);
    idx_result = classeurs_model->sibling(index.row(),0,index);
    QString name_project = "test";
    Classeur *currentClasseur = classeurs_model->getClasseur(idx_result);
    add_classeur_files *dialog_add_classeur;
    dialog_add_classeur = new add_classeur_files(this, name_project,
                                                 classeurs_model, currentClasseur);
    int result = dialog_add_classeur->exec();
        if(result) {
            QModelIndex oldParentAtId      = classeurs_model->index(idx_result.row(), 2,
                                                                    idx_result.parent());
            QVector<QVariant> datas;
            datas << dialog_add_classeur->New_classeur.name
                  << dialog_add_classeur->New_classeur.comment
                  << QVariant("")
                  << oldParentAtId.data();
            if(!classeurs_model->insertClasseur(datas, 0, idx_result))
                return;
        }
}

//********************              Files tree Actions        ***********************

void MainWindow::on_files_doubleClicked(const QModelIndex &index) {
    qDebug(Q_FUNC_INFO);
    int column = index.column();
    QString data = index.data().toString();
    switch (column) {
        case 0: { // ouverture du dossier dans le file browser par défaut de l'OS (pas de break)
            }
        case 1: {  // ouvrir le fichier avec une application par défaut de l'OS
            QString my_url;
            QModelIndex next;
            my_url = QString("%1").arg(index.data().toString());
            next = index.parent();
            while (next.isValid()) {
                my_url = QString("%1/%2").arg(next.data().toString()).arg(my_url);
                next = next.parent(); }
            my_url = QString("file://%1").arg(my_url);
            QDesktopServices::openUrl(QUrl(my_url));
            break; }
        case 3: {
            break; } }// edition du commentaire lié au fichier ou au répertoire
}
