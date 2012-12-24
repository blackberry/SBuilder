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

#ifndef EXPLOREWIDGET_H
#define EXPLOREWIDGET_H

#include <QTreeWidget>
#include <QSharedPointer>
#include "sqlite.h"

class QContextMenuEvent;
class QAction;

class ExploreWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ExploreWidget(QWidget *parent = 0);
    void populateTableTree();

signals:

public slots:
    void active(QSharedPointer<SQLite> pDb);
    void inactive();
    void dropTable();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);


private:
    QStringList getColumnNames(const SQLiteTableInfo &tableName);

    QSharedPointer<SQLite> m_pDb;
    QStringList m_tableNames;
    QAction *m_dropTableAction;

};

#endif // EXPLOREWIDGET_H
