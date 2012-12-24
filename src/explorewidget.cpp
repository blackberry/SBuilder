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

#include "explorewidget.h"
#include "explorewidgetitem.h"
#include "sqlite.h"
#include "sqlitestmt.h"
#include "sqliteinfo.h"

ExploreWidget::ExploreWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    setColumnCount(1);
    setHeaderLabel("");
    setEnabled(false);

    m_dropTableAction = new QAction(this);
    m_dropTableAction->setText(tr("&Drop Table"));
    connect(m_dropTableAction, SIGNAL(triggered()), this, SLOT(dropTable()));
}

void ExploreWidget::dropTable()
{
    ExploreWidgetItem *pItem = static_cast<ExploreWidgetItem *>(currentItem());

    int rc;
    if (!m_pDb->dropTable(pItem->tableName(), rc)) {
        QMessageBox::warning(this, tr("Drop Table"), tr("Could not drop table.  rc: %1, reason: %2").arg(rc).arg(m_pDb->getErrMsg()));
    } else {
        populateTableTree();
    }
}

void ExploreWidget::active(QSharedPointer<SQLite> pDb)
{
    setEnabled(true);
    m_pDb = pDb;
    setHeaderLabel(m_pDb->fileName());
    populateTableTree();
}

void ExploreWidget::inactive()
{
    setEnabled(false);
    setHeaderLabel(tr(""));
    clear();
    m_pDb.clear();
    m_tableNames.clear();
}

void ExploreWidget::contextMenuEvent(QContextMenuEvent *e)
{


    ExploreWidgetItem *pItem = static_cast<ExploreWidgetItem *>(currentItem());

    if (pItem != NULL && pItem->isTable()) {
        QMenu menu(this);
        menu.addAction(m_dropTableAction);
        menu.exec(e->globalPos());
    }
}

void ExploreWidget::populateTableTree()
{
    int rc;
    QStringList tableNames = m_pDb->getTableNames(rc);

    QStringList removeTables;

    foreach (QString table, tableNames) {
        if (!m_tableNames.contains(table)) {
            QSharedPointer<SQLiteTableInfo> pTableInfo(m_pDb->getTableInfo(table, rc));

            if (pTableInfo.isNull()) {
                continue;
            }

            ExploreWidgetItem * tableItem = new ExploreWidgetItem();
            tableItem->setTableName(table, pTableInfo->isVirtual());

            insertTopLevelItem(topLevelItemCount(), tableItem);

            for (int i = 0; i < pTableInfo->size(); i++) {
                ExploreWidgetItem *pColumnItem = new ExploreWidgetItem();
                pColumnItem->setColumnName(pTableInfo->getColumnName(i), pTableInfo->getColumnAffinity(i), pTableInfo->isPrimaryKey(i));
                tableItem->addChild(pColumnItem);
            }
        }
    }

    foreach (QString table, m_tableNames) {
        if (!tableNames.contains(table)) {
            removeTables.append(table);
        }
    }

    m_tableNames.clear();
    m_tableNames.append(tableNames);

    if (!removeTables.isEmpty()) {
        QTreeWidgetItem * pItem = invisibleRootItem();
        for (int i = 0; i < pItem->childCount(); i++) {
            ExploreWidgetItem *pChild = static_cast<ExploreWidgetItem *>(pItem->child(i));
            if (removeTables.contains(pChild->text(0))) {
                pItem->removeChild(pChild);
            }
        }
    }

}
