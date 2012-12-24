/* Copyright 2011 Research In Motion Limited.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QtGui>

#include "sqlite.h"
#include "sqlitebkp.h"
#include "sqlite3.h"

#include "log.h"
#include "pragmadialog.h"
#include "memorydialog.h"
#include "databasewidget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_log = new Log(this);
    m_pragmaDialog = new PragmaDialog(this);
    m_memoryDialog = new MemoryDialog(this);

    m_dbWidget = new DatabaseWidget();
    connect(m_dbWidget, SIGNAL(logMsg(QString)), m_log, SLOT(addMsg(QString)));
    connect(m_dbWidget, SIGNAL(active(QSharedPointer<SQLite>)), m_pragmaDialog, SLOT(active(QSharedPointer<SQLite>)));
    connect(m_dbWidget, SIGNAL(inactive()), m_pragmaDialog, SLOT(inactive()));

    connect(m_dbWidget, SIGNAL(active(QSharedPointer<SQLite>)), m_memoryDialog, SLOT(active(QSharedPointer<SQLite>)));
    connect(m_dbWidget, SIGNAL(inactive()), m_memoryDialog, SLOT(inactive()));

    setCentralWidget(m_dbWidget);

    createActions();
    createMenus();
    createToolBars();

    statusBar();

    QSettings settings("Research In Motion", "sbuilder");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();

    sqlite3_initialize();
}

MainWindow::~MainWindow()
{
    sqlite3_shutdown();
}

void MainWindow::about() {
    QMessageBox::information(
            this,
            tr("About sbuilder"),
            tr("Copyright (c) 2011 Research In Motion Limited."),
            QMessageBox::Ok);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings("Research In Motion", "sbuilder");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", m_recentFiles);

    event->accept();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        loadDatabase(action->data().toString());
    }
}

void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator files(m_recentFiles);
    while (files.hasNext()) {
        if (!QFile::exists(files.next())) {
            files.remove();
        }
    }

    for (int i = 0; i < MaxRecentFiles; ++i) {
        if (i < m_recentFiles.count()) {
            QString text = tr("&%1 %2").arg(i + 1).arg(m_recentFiles[i]);
            m_recentFileActions[i]->setText(text);
            m_recentFileActions[i]->setData(m_recentFiles[i]);
            m_recentFileActions[i]->setVisible(true);
        } else {
            m_recentFileActions[i]->setVisible(false);
        }
    }

    m_recentFileSeparator->setVisible(!m_recentFiles.isEmpty());
}

void MainWindow::openDatabase() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Database"), ".", tr("Database files (*.db);;All files (*.*)"));

    if (!fileName.isEmpty()) {
        loadDatabase(fileName);
    }
}

void MainWindow::saveAsDatabase() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Database"), ".", tr("Database files (*.db);;All files (*.*)"));

    if (!fileName.isEmpty()) {
        // Copy the current database, close it, and open the "saved as" file
        copyDatabase(fileName);

        m_dbWidget->closeDatabase();
        loadDatabase(fileName);
    }
}

void MainWindow::openMemoryDatabase() {
    loadDatabase("");
}

void MainWindow::copyDatabase(const QString &fileName)
{
    int rc;

    QSharedPointer<SQLite> pSrc = m_dbWidget->getDb();
    QSharedPointer<SQLite> pDest = SQLite::create(fileName, rc);

    if (!pSrc.isNull() && !pDest.isNull() && rc == 0) {
        QSharedPointer<SQLiteBkp> pBkp = SQLiteBkp::create(pSrc, pDest);
        if (pBkp) {
            pBkp->step(rc);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not copy to save file, reason: %1").arg(rc));
        }
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Could not create save file %1, reason: %2").arg(fileName).arg(pDest->getErrMsg()));
    }
}

void MainWindow::loadDatabase(const QString &fileName)
{
    QString openFile = fileName;
    if (!openFile.isEmpty()) {
        m_recentFiles.removeAll(openFile);
        m_recentFiles.prepend(openFile);
        updateRecentFileActions();
    } else {
        openFile = ":memory:";
    }

    m_dbWidget->openDatabase(openFile);

    setWindowTitle(tr("sbuilder - %1").arg(openFile));
    statusBar()->showMessage(tr("Opened database %1").arg(openFile), 2000);
}

void MainWindow::closeDatabase()
{
    setWindowTitle(tr("sbuilder"));
    m_dbWidget->closeDatabase();
}

void MainWindow::createActions() {

    m_newAction = new QAction(this);
    m_newAction->setText(tr("&New"));
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setIcon(QIcon(":/images/document-new.png"));
    m_newAction->setStatusTip(tr("Create a new, in memory, SQLite database"));
    connect(m_newAction, SIGNAL(triggered()), this, SLOT(openMemoryDatabase()));

    m_openAction = new QAction(this);
    m_openAction->setText(tr("&Open"));
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setIcon(QIcon(":/images/document-open.png"));
    m_openAction->setStatusTip(tr("Open an existing SQLite database"));
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(openDatabase()));

    m_saveAction = new QAction(this);
    m_saveAction->setText(tr("&Save"));
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip(tr("Save the database"));
    connect(m_saveAction, SIGNAL(triggered()), m_dbWidget, SIGNAL(commit()));

    m_saveAsAction = new QAction(this);
    m_saveAsAction->setText(tr("Save &As..."));
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setStatusTip(tr("Save this database under a new name"));
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsDatabase()));

    m_closeAction = new QAction(this);
    m_closeAction->setText(tr("Close"));
    m_closeAction->setShortcut(QKeySequence::Close);
    m_closeAction->setStatusTip(tr("Close the currently opened database"));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(closeDatabase()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        m_recentFileActions[i] = new QAction(this);
        m_recentFileActions[i]->setVisible(false);
        connect(m_recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    m_exitAction = new QAction(this);
    m_exitAction->setText(tr("E&xit"));
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Quits sbuilder"));
    connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));

    m_undoAction = new QAction(this);
    m_undoAction->setText(tr("&Undo"));
    m_undoAction->setShortcut(QKeySequence::Undo);
    connect(m_undoAction, SIGNAL(triggered()), m_dbWidget, SLOT(undo()));

    m_redoAction = new QAction(this);
    m_redoAction->setText(tr("&Redo"));
    m_redoAction->setShortcut(QKeySequence::Redo);
    connect(m_redoAction, SIGNAL(triggered()), m_dbWidget, SLOT(redo()));

    m_cutAction = new QAction(this);
    m_cutAction->setText(tr("Cu&t"));
    m_cutAction->setShortcut(QKeySequence::Cut);
    m_cutAction->setIcon(QIcon(":/images/edit-cut.png"));
    connect(m_cutAction, SIGNAL(triggered()), m_dbWidget, SLOT(cut()));

    m_copyAction = new QAction(this);
    m_copyAction->setText(tr("&Copy"));
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    connect(m_copyAction, SIGNAL(triggered()), m_dbWidget, SLOT(copy()));

    m_pasteAction = new QAction(this);
    m_pasteAction->setText(tr("&Paste"));
    m_pasteAction->setShortcut(QKeySequence::Paste);
    m_pasteAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(m_pasteAction, SIGNAL(triggered()), m_dbWidget, SLOT(paste()));

    m_selectAllAction = new QAction(this);
    m_selectAllAction->setText(tr("&Select All"));
    m_selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(m_selectAllAction, SIGNAL(triggered()), m_dbWidget, SLOT(selectAll()));

    m_runAction = new QAction(this);
    m_runAction->setText(tr("&Run"));
    // TODO: a better name
    m_runAction->setIcon(QIcon(":/images/media-playback-start.png"));
    m_runAction->setShortcut(QKeySequence(tr("F5")));
    connect(m_runAction, SIGNAL(triggered()), m_dbWidget, SLOT(execute()));

    m_commitAction = new QAction(this);
    m_commitAction->setText(tr("&Commit"));
    connect(m_commitAction, SIGNAL(triggered()), m_dbWidget, SIGNAL(commit()));

    m_rollbackAction = new QAction(this);
    m_rollbackAction->setText(tr("Roll&back"));
    connect(m_rollbackAction, SIGNAL(triggered()), m_dbWidget, SIGNAL(rollback()));

    m_logAction = new QAction(this);
    m_logAction->setText(tr("Show Log"));
    connect(m_logAction, SIGNAL(triggered()), m_log, SLOT(show()));

    m_pragmaAction = new QAction(this);
    m_pragmaAction->setText(tr("Show &Pragmas"));
    connect(m_pragmaAction, SIGNAL(triggered()), m_pragmaDialog, SLOT(show()));

    m_memoryAction = new QAction(this);
    m_memoryAction->setText(tr("Show &Memory Stats"));
    connect(m_memoryAction, SIGNAL(triggered()), m_memoryDialog, SLOT(show()));

    m_aboutAction = new QAction(this);
    m_aboutAction->setText(tr("&About"));
    m_aboutAction->setStatusTip(tr("About the wonderful maker of sbuilder"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus() {
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addAction(m_closeAction);
    m_recentFileSeparator = m_fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i) {
        m_fileMenu->addAction(m_recentFileActions[i]);
    }

    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_editMenu->addAction(m_undoAction);
    m_editMenu->addAction(m_redoAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_selectAllAction);

    m_runMenu = menuBar()->addMenu(tr("&Run"));
    m_runMenu->addAction(m_runAction);
    m_runMenu->addSeparator();
    m_runMenu->addAction(m_commitAction);
    m_runMenu->addAction(m_rollbackAction);

    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    m_windowMenu->addAction(m_logAction);
    m_windowMenu->addAction(m_pragmaAction);
    m_windowMenu->addAction(m_memoryAction);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBars() {
    m_fileToolBar = addToolBar(tr("File"));
    m_fileToolBar->addAction(m_newAction);
    m_fileToolBar->addAction(m_openAction);

    m_editToolBar = addToolBar(tr("Edit"));
    m_editToolBar->addAction(m_cutAction);
    m_editToolBar->addAction(m_copyAction);
    m_editToolBar->addAction(m_pasteAction);

    m_runToolBar = addToolBar(tr("Run"));
    m_runToolBar->addAction(m_runAction);
}
