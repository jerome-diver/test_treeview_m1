#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardItem>
#include <QAction>

#include "classeurs_model.h"
#include "classeur.h"
#include "files_model.h"
#include "add_classeur_files.h"

namespace Ui {
class MainWindow;
}

struct datas {
    int       id;
    QString   name;
    QString   ref;
    QString   comment;
    int       location_id;
    QString   location_name;
    QString   dir_name;
    QString   dir_comment;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    datas  Projects;

private slots:
    void on_files_doubleClicked(const QModelIndex &index);
    void on_classeurs_clicked(QModelIndex index) ;
    void on_classeurs_add(QModelIndex index);
    void on_classeurs_add_child(QModelIndex index);
    void on_classeurs_delete(QModelIndexList list_indexs);

private:
    Ui::MainWindow *ui;
    Classeurs_Model          *classeurs_model;
    Files_Model              *files_name;
    QModelIndex               idx_result;
};

#endif // MAINWINDOW_H
