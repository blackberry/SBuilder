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

#include "resultwidget.h"
#include "resultwidgetitem.h"
#include "sqlite.h"
#include "sqlitestmt.h"
#include "resultset.h"
#include "blobviewerdialog.h"
#include "sqlite3.h"

ResultWidget::ResultWidget(QWidget *parent)
    : QTableWidget(parent)
    , m_activeTransaction(false)
{
    setEnabled(false);
    setSelectionMode(QAbstractItemView::SingleSelection);

    m_insertAction = new QAction(this);
    m_insertAction->setText(tr("&Insert Row"));
    connect(m_insertAction, SIGNAL(triggered()), this, SLOT(insert()));

    m_setNullAction = new QAction(this);
    m_setNullAction->setText(tr("&Set Null"));
    connect(m_setNullAction, SIGNAL(triggered()), this, SLOT(setNull()));

    m_cutAction = new QAction(this);
    m_cutAction->setText(tr("Cu&t"));
    connect(m_cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    m_copyAction = new QAction(this);
    m_copyAction->setText(tr("&Copy"));
    connect(m_copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    m_pasteAction = new QAction(this);
    m_pasteAction->setText(tr("&Paste"));
    connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    m_deleteRowAction = new QAction(this);
    m_deleteRowAction->setText(tr("Delete &Row"));
    connect(m_deleteRowAction, SIGNAL(triggered()), this, SLOT(deleteRow()));

    m_insertFileContentsAction = new QAction(this);
    m_insertFileContentsAction->setText(tr("&Import File Contents"));
    connect(m_insertFileContentsAction, SIGNAL(triggered()), this, SLOT(insertFileContents()));

    m_exportCellContentsAction = new QAction(this);
    m_exportCellContentsAction->setText(tr("&Export Cell Contents"));
    connect(m_exportCellContentsAction, SIGNAL(triggered()), this, SLOT(exportCellContents()));

    m_viewBlobImageAction = new QAction(this);
    m_viewBlobImageAction->setText(tr("&View Blob Image"));
    connect(m_viewBlobImageAction, SIGNAL(triggered()), this, SLOT(viewBlobImage()));

    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(startTransaction(QTableWidgetItem*)));
}

void ResultWidget::active(QSharedPointer<SQLite> pDb)
{
    setEnabled(true);
    m_pDb = pDb;
}

void ResultWidget::inactive()
{
    setEnabled(false);

    if (m_activeTransaction) {
        rollback();
    }

    clear();
    setRowCount(0);
    setColumnCount(0);
    m_pDb.clear();
    m_tableName.clear();
    m_columnNames.clear();
    m_dbName.clear();
}

bool ResultWidget::isActiveTransaction() const
{
    return m_activeTransaction;
}

void ResultWidget::insertFileContents()
{
    if (m_activeTransaction) {
        QMessageBox::warning(this, tr("Transaction"), tr("Cannot insert file contents while there is a transaction in progress"));
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("All files (*.*)"));

    if (!fileName.isEmpty()) {
        insertFileContents(fileName);
    }
}

void ResultWidget::exportCellContents()
{
    if (m_activeTransaction) {
        QMessageBox::warning(this, tr("Transaction"), tr("Cannot export cell contents while there is a transaction in progress"));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("All files (*.*)"));

    if (!fileName.isEmpty()) {
        exportCellContents(fileName);
    }
}

void ResultWidget::insertFileContents(const QString &fileName)
{

    ResultWidgetItem *pItem = static_cast<ResultWidgetItem *>(currentItem());

    int rc;
    if (!m_pDb->updateCellWithFile(pItem->getDb(), pItem->getTable(), pItem->getColumn(), pItem->getRowId(), fileName, rc)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not insert file contents into cell, error: %1 reason: %2").arg(rc).arg(m_pDb->getErrMsg()));
        return;
    }

    dumpTable(m_tableName);
}

void ResultWidget::exportCellContents(const QString &fileName)
{
    ResultWidgetItem *pItem = static_cast<ResultWidgetItem*>(currentItem());

    int rc;
    if (!m_pDb->exportCellToFile(pItem->getDb(),pItem->getTable(),pItem->getColumn(),pItem->getRowId(),fileName,rc)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not export file contents to file, error: %1 reson: %2").arg(rc).arg(m_pDb->getErrMsg()));
    }
}

bool ResultWidget::beginTransaction()
{
    if (!m_activeTransaction) {
        int rc;
        if (m_pDb->begin(rc)) {
            m_activeTransaction = true;
        } else {
            QMessageBox::warning(this, tr("Transaction error"), tr("Failed to start transaction.  rc: %1, reason: %2").arg(rc).arg(m_pDb->getErrMsg()));
            return false;
        }
    }

    return true;
}

void ResultWidget::startTransaction(QTableWidgetItem *item)
{
    if (m_mode != DumpTable) {
        return;
    }

    ResultWidgetItem * resultItem = static_cast<ResultWidgetItem*>(item);

    if (resultItem->getState() == ResultWidgetItem::Dirty) {
        beginTransaction();
    }

}

void ResultWidget::displayResultSet(const ResultSet &resultSet)
{
    m_mode = DisplayResults;
    clear();
    setRowCount(0);
    setColumnCount(0);

    QStringList columnHeaders = resultSet.getColumns();

    setColumnCount(columnHeaders.size());
    setHorizontalHeaderLabels(columnHeaders);

    setRowCount(resultSet.size());

    for (int i = 0; i < resultSet.size(); i++) {
        QStringList data = resultSet.getRow(i);
        int j = 0;
        foreach (QString dataItem, data) {
            ResultWidgetItem *item = new ResultWidgetItem(m_pDb, "", "", "", -1);
            // TODO for now we are just grabbing everything as text
            item->setInitialValue(dataItem);
            setItem(i, j, item);
            j++;
        }
    }
}

void ResultWidget::setNull()
{
    ResultWidgetItem *pItem = static_cast<ResultWidgetItem *>(currentItem());
    pItem->setNull();
}

void ResultWidget::cut()
{
    ResultWidgetItem *pItem = static_cast<ResultWidgetItem *>(currentItem());
    QApplication::clipboard()->setText(pItem->data(Qt::EditRole).toString());
    pItem->setData(Qt::EditRole, "");
}

void ResultWidget::copy()
{
    ResultWidgetItem *pItem = static_cast<ResultWidgetItem *>(currentItem());
    QApplication::clipboard()->setText(pItem->data(Qt::EditRole).toString());
}

void ResultWidget::paste()
{
    ResultWidgetItem *pItem = static_cast<ResultWidgetItem *>(currentItem());
    pItem->setData(Qt::EditRole, QApplication::clipboard()->text());
}

void ResultWidget::deleteRow()
{
    QTableWidgetItem *pItem = currentItem();

    int row = pItem->row();

    QTableWidgetItem *pRowHeader = verticalHeaderItem(row);

    if (!beginTransaction()) {
        return;
    }

    int rowid = pRowHeader->text().toInt();

    int rc;
    if (!m_pDb->deleteTableRow(m_dbName, m_tableName, rowid, rc)) {
         QMessageBox::warning(this, tr("Delete Row"), tr("Error deleting row. rc: %1").arg(rc));
         return;
    }

    for (int i = 0; i < columnCount(); i++) {
        pItem = item(row, i);
        pItem->setBackground(QBrush(Qt::red));
        pItem->setFlags(pItem->flags() & ~Qt::ItemIsEditable);
    }
}

void ResultWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(this);

    QTableWidgetItem *pItem = currentItem();

    if (pItem != NULL) {
        if (pItem->flags() & Qt::ItemIsEditable) {
            menu.addAction(m_cutAction);
            menu.addAction(m_copyAction);
            menu.addAction(m_pasteAction);
            menu.addSeparator();
            menu.addAction(m_setNullAction);
            menu.addSeparator();
        }

        menu.addAction(m_deleteRowAction);
        menu.addSeparator();
        menu.addAction(m_insertFileContentsAction);
        menu.addAction(m_exportCellContentsAction);
        menu.addAction(m_viewBlobImageAction);
    }

    menu.addAction(m_insertAction);

    menu.exec(e->globalPos());
}

void ResultWidget::insert()
{
    QTableWidgetItem *pItem = verticalHeaderItem(rowCount() - 1);
    if (rowCount() > 0 && pItem && !pItem->text().isNull() && (pItem->text().compare(tr("*")) == 0)) {
        QMessageBox::warning(this, tr("Insert Row"), tr("Cannot insert another row, commit the current one, and try again."));
        return;
    }

    if (!beginTransaction()) {
        return;
    }

    insertRow(rowCount());
    pItem = new QTableWidgetItem(tr("*"));
    setVerticalHeaderItem(rowCount() - 1, pItem);

    for (int i = 0; i < columnCount(); i++) {
        ResultWidgetItem *pItem = new ResultWidgetItem(m_pDb, m_dbName, m_tableName, m_columnNames.at(i), -1);
        pItem->setText(tr(""));
        setItem(rowCount()-1, i, pItem);
    }
}

void ResultWidget::viewBlobImage()
{
    BlobViewerDialog dialog(this);
    ResultWidgetItem *pItem = static_cast<ResultWidgetItem *>(currentItem());
    if (dialog.useCell(m_pDb, pItem->getDb(), pItem->getTable(), pItem->getColumn(), pItem->getRowId())) {
        dialog.exec();
    }
}

void ResultWidget::commit()
{
    if (!m_activeTransaction) {
        QMessageBox::warning(this, tr("Transaction error"), tr("No active transaction.  Nothing to commit."));
        return;
    }

    // First we check to see if a new row was added
    if (rowCount() > 0 && (verticalHeaderItem(rowCount() - 1)->text().compare(tr("*")) == 0)) {
        QStringList values;

        for (int i = 0; i < columnCount(); i++) {
            ResultWidgetItem *pItem = static_cast<ResultWidgetItem*>(item(rowCount() - 1, i));
            values.append(pItem->text());
        }

        if (!values.isEmpty()) {
            int rc;
            if (! m_pDb->insertTableRow(m_dbName, m_tableName, values, rc)) {
                QMessageBox::warning(this, tr("Insert Error"), tr("Could not insert new row into the table.  Reason: %1").arg(m_pDb->getErrMsg()));
                return;
            }
        }
    }

    int rc;
    if (m_pDb->commit(rc)) {
        m_activeTransaction = false;
    } else {
        QMessageBox::warning(this, tr("Transaction error"), tr("Failed to commit transaction.  rc: %1, reason: %2").arg(rc).arg(m_pDb->getErrMsg()));
        return;
    }

    // Successful commit, re-dump the table
    dumpTable(m_tableName);
}

void ResultWidget::rollback()
{
    if (m_activeTransaction) {
        int rc;
        if (m_pDb->rollback(rc)) {
            m_activeTransaction = false;
        } else {
            QMessageBox::warning(this, tr("Transaction error"), tr("Failed to rollback transaction.  rc: %1, reason: %2").arg(rc).arg(m_pDb->getErrMsg()));
            return;
        }
    } else {
        QMessageBox::warning(this, tr("Transaction error"), tr("No active transaction.  Nothing to rollback."));
        return;
    }

    // Dump the table again because all the values have changed.
    dumpTable(m_tableName);
}

void ResultWidget::dumpTable(const QString &tableName)
{
    m_mode = DumpTable;
    m_tableName = tableName;
    clear();
    setRowCount(0);
    setColumnCount(0);

    // Top-level items are table names
    QString query = tr("select rowid, * from '%1'").arg(tableName);
    emit logMsg(query);
    int rc;
    QSharedPointer<SQLiteStmt> pStmt(m_pDb->prepare(query, rc));

    if (pStmt.isNull()) {
        QMessageBox::warning(this, tr("Displaying Table"), tr("Could not display table contents, query failed: %1, %2").arg(rc).arg(m_pDb->getErrMsg()));
        return;
    }

    m_columnNames.clear();

    setColumnCount(pStmt->getColumnCount() - 1);

    for (int i = 1; i < pStmt->getColumnCount(); ++i) {
        // First one is rowid, we can skip it
        m_columnNames += pStmt->getColumnName(i);
    }

    m_dbName = pStmt->getColumnDatabaseName(0);

    setHorizontalHeaderLabels(m_columnNames);

    while (pStmt->step(rc)) {
        int rows = rowCount();
        insertRow(rows);

        int rowid = pStmt->getInt(0);

        QTableWidgetItem *pHeaderItem = new QTableWidgetItem(tr("%1").arg(rowid));
        setVerticalHeaderItem(rows, pHeaderItem);

        for (int i = 0; i < columnCount(); ++i) {
            ResultWidgetItem *pItem = new ResultWidgetItem(m_pDb, m_dbName, m_tableName, m_columnNames.at(i), rowid);
            // TODO for now we are just grabbing everything as text
            pItem->setInitialValue(pStmt->getString(i + 1));
            setItem(rows, i, pItem);
        }
    }
}
