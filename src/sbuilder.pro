#-------------------------------------------------
#
# Project created by QtCreator 2011-03-29T21:44:49
#
#-------------------------------------------------

QT       += core gui

DEFINES += SQLITE_ENABLE_COLUMN_METADATA SQLITE_ENABLE_FTS3 SQLITE_ENABLE_FTS3_PARENTHESIS YYTRACKMAXSTACKDEPTH


TARGET = sbuilder
TEMPLATE = app

INCLUDEPATH += ./sqlite ./core

SOURCES += main.cpp\
        mainwindow.cpp \
    databasewidget.cpp \
    log.cpp \
    querywidget.cpp \
    resultwidget.cpp \
    explorewidget.cpp \
    queryhighlighter.cpp \
    resultwidgetitem.cpp \
    transactiondialog.cpp \
    sqlite/sqlite3.c \
    core/sqlite.cpp \
    core/sqlitebkp.cpp \
    core/sqlitestmt.cpp \
    core/resultset.cpp \
    core/sqliteinfo.cpp \
    explorewidgetitem.cpp \
    pragmadialog.cpp \
    core/sqliteio.cpp \
    blobviewerdialog.cpp \
    memorydialog.cpp

HEADERS  += mainwindow.h \
    databasewidget.h \
    log.h \
    querywidget.h \
    resultwidget.h \
    explorewidget.h \
    queryhighlighter.h \
    resultwidgetitem.h \
    transactiondialog.h \
    core/resultset.h \
    core/sqlitestmt.h \
    core/sqlitebkp.h \
    core/sqlite.h \
    core/sqliteinfo.h \
    explorewidgetitem.h \
    pragmadialog.h \
    core/sqliteio.h \
    blobviewerdialog.h \
    memorydialog.h

FORMS    += \
    transactiondialog.ui \
    pragmadialog.ui \
    blobviewerdialog.ui \
    memorydialog.ui

RESOURCES += \
    resources.qrc
