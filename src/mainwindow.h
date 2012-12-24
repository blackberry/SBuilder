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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QToolBar;

class DatabaseWidget;
class PragmaDialog;
class MemoryDialog;
class Log;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

private slots:
    void about();
    void openDatabase();
    void openMemoryDatabase();
    void openRecentFile();
    void closeDatabase();
    void saveAsDatabase();

private:

    static const int MaxRecentFiles = 4;

    void createActions();
    void createMenus();
    void createToolBars();

    void updateRecentFileActions();
    void loadDatabase(const QString &fileName);
    void copyDatabase(const QString &fileName);

    DatabaseWidget *m_dbWidget;
    Log *m_log;
    PragmaDialog *m_pragmaDialog;
    MemoryDialog *m_memoryDialog;
    QStringList m_recentFiles;

    QMenu *m_fileMenu;
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_closeAction;
    QAction *m_recentFileSeparator;
    QAction *m_recentFileActions[MaxRecentFiles];
    QAction *m_exitAction;

    QMenu *m_editMenu;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_selectAllAction;

    QMenu *m_runMenu;
    QAction *m_runAction;
    QAction *m_commitAction;
    QAction *m_rollbackAction;

    QMenu *m_windowMenu;
    QAction *m_logAction;
    QAction *m_pragmaAction;
    QAction *m_memoryAction;

    QMenu *m_helpMenu;
    QAction *m_aboutAction;

    QToolBar *m_fileToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_runToolBar;
};

#endif // MAINWINDOW_H
