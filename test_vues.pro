#-------------------------------------------------
#
# Project created by QtCreator 2015-02-23T17:22:43
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_vues
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
        classeur.cpp \
        classeurs_treeview.cpp \
        classeurs_model.cpp \
        files_treeview.cpp \
        files_model.cpp \
        add_classeur_files.cpp

HEADERS  += mainwindow.h\
        classeur.h \
        classeurs_treeview.h \
        classeurs_model.h \
        files_treeview.h \
        files_model.h \
        add_classeur_files.h

FORMS    += mainwindow.ui \
        add_classeur_files.ui

include(../modeltest5/modeltest.pri)
