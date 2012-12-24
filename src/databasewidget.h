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

#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include "sqlite.h"

class QVBoxLayout;
class QHBoxLayout;
class QTreeWidgetItem;

class QueryWidget;
class ExploreWidget;
class ResultWidget;

class DatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    DatabaseWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~DatabaseWidget();
    QSharedPointer<SQLite> getDb();

signals:
    void active(QSharedPointer<SQLite> pDb);
    void inactive();

    void logMsg(const QString &msg);


    void commit();
    void rollback();

public slots:
    bool openDatabase(const QString &fileName);
    bool closeDatabase();
    void execute();
    void cut();
    void undo();
    void redo();
    void copy();
    void paste();
    void selectAll();

protected slots:
    void dumpTable(QTreeWidgetItem*,int);

private:

    // Checks to see what a user wants to do
    // about any ongoing transaction.  Returns
    // true if a transaction remains active.
    // false o/w.
    bool checkActiveTransaction();

    ExploreWidget *m_tableTree;
    ResultWidget *m_tableResults;
    QueryWidget *m_queryEdit;

    QVBoxLayout *m_vLayoutRight;
    QVBoxLayout *m_vLayoutLeft;
    QHBoxLayout *m_hLayout;

    QSharedPointer<SQLite> m_sqlite;
};

#endif // DATABASEWIDGET_H
