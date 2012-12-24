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

#ifndef RESULTWIDGET_H
#define RESULTWIDGET_H

#include <QTableWidget>
#include <QSharedPointer>
#include <QStringList>
#include "sqlite.h"

class QTableWidgetItem;
class ResultSet;
class ResultWidgetItem;
class QAction;

class ResultWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit ResultWidget(QWidget *parent = 0);

    void dumpTable(const QString &tableName);

    void displayResultSet(const ResultSet &resultSet);

    bool isActiveTransaction() const;

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    bool beginTransaction();
    void insertFileContents(const QString &fileName);
    void exportCellContents(const QString &fileName);

signals:
    void logMsg(const QString &msg);

public slots:
    void active(QSharedPointer<SQLite> pDb);
    void inactive();
    void commit();
    void rollback();
    void insert();
    void setNull();
    void deleteRow();
    void insertFileContents();
    void exportCellContents();
    void viewBlobImage();

    void cut();
    void copy();
    void paste();

protected slots:
    void startTransaction(QTableWidgetItem *item);

private:
    bool m_activeTransaction;
    QSharedPointer<SQLite> m_pDb;
    QString m_tableName;
    QString m_dbName;
    QStringList m_columnNames;
    QAction *m_insertAction;
    QAction *m_setNullAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_deleteRowAction;
    QAction *m_insertFileContentsAction;
    QAction *m_exportCellContentsAction;
    QAction *m_viewBlobImageAction;
    enum { DumpTable, DisplayResults } m_mode;
};

#endif // RESULTWIDGET_H
