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
#include "sqlitestmt.h"
#include "databasewidget.h"
#include "querywidget.h"
#include "explorewidget.h"
#include "explorewidgetitem.h"
#include "resultwidget.h"
#include "transactiondialog.h"

DatabaseWidget::DatabaseWidget(QWidget *parent, Qt::WindowFlags f)
        : QWidget(parent, f)
        , m_sqlite(0)
{
    m_queryEdit = new QueryWidget(this);
    m_tableResults = new ResultWidget(this);    
    m_tableTree = new ExploreWidget(this);

    m_hLayout = new QHBoxLayout(this);
    m_vLayoutLeft = new QVBoxLayout();
    m_vLayoutRight = new QVBoxLayout();

    m_hLayout->addLayout(m_vLayoutLeft);
    m_hLayout->addLayout(m_vLayoutRight);

    m_vLayoutLeft->setObjectName("vLayoutLeft");
    m_vLayoutLeft->addWidget(m_tableTree);

    m_vLayoutRight->setObjectName("vLayoutRight");
    m_vLayoutRight->addWidget(m_queryEdit);
    m_vLayoutRight->addWidget(m_tableResults);

    connect(this, SIGNAL(active(QSharedPointer<SQLite>)), m_queryEdit, SLOT(active(QSharedPointer<SQLite>)));
    connect(this, SIGNAL(inactive()), m_queryEdit, SLOT(inactive()));

    connect(this, SIGNAL(active(QSharedPointer<SQLite>)), m_tableTree, SLOT(active(QSharedPointer<SQLite>)));
    connect(this, SIGNAL(inactive()), m_tableTree, SLOT(inactive()));

    connect(this, SIGNAL(active(QSharedPointer<SQLite>)), m_tableResults, SLOT(active(QSharedPointer<SQLite>)));
    connect(this, SIGNAL(inactive()), m_tableResults, SLOT(inactive()));
    connect(this, SIGNAL(commit()), m_tableResults, SLOT(commit()));
    connect(this, SIGNAL(rollback()), m_tableResults, SLOT(rollback()));

    connect(m_tableTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(dumpTable(QTreeWidgetItem*,int)));

    connect(m_queryEdit, SIGNAL(execute()), this, SLOT(execute()));

    connect(m_tableResults, SIGNAL(logMsg(QString)), this, SIGNAL(logMsg(QString)));

}

DatabaseWidget::~DatabaseWidget()
{
}

QSharedPointer<SQLite> DatabaseWidget::getDb()
{
    return m_sqlite;
}

void DatabaseWidget::cut()
{
    QWidget *pWidget = focusWidget();

    QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pWidget);

    if (pLineEdit) {
        pLineEdit->cut();
        return;
    }

    QTextEdit *pTextEdit = dynamic_cast<QTextEdit *>(pWidget);

    if (pTextEdit) {
        pTextEdit->cut();
        return;
    }

    ResultWidget *pResultWidget = dynamic_cast<ResultWidget *>(pWidget);

    if (pResultWidget) {
        pResultWidget->cut();
    }
}
void DatabaseWidget::undo()
{
    QWidget *pWidget = focusWidget();

    QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pWidget);

    if (pLineEdit) {
        pLineEdit->undo();
        return;
    }

    QTextEdit *pTextEdit = dynamic_cast<QTextEdit *>(pWidget);

    if (pTextEdit) {
        pTextEdit->undo();
        return;
    }
}

void DatabaseWidget::redo()
{
    QWidget *pWidget = focusWidget();

    QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pWidget);

    if (pLineEdit) {
        pLineEdit->redo();
        return;
    }

    QTextEdit *pTextEdit = dynamic_cast<QTextEdit *>(pWidget);

    if (pTextEdit) {
        pTextEdit->redo();
        return;
    }
}

void DatabaseWidget::copy()
{
    QWidget *pWidget = focusWidget();

    QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pWidget);

    if (pLineEdit) {
        pLineEdit->copy();
        return;
    }

    QTextEdit *pTextEdit = dynamic_cast<QTextEdit *>(pWidget);

    if (pTextEdit) {
        pTextEdit->copy();
        return;
    }

    ResultWidget *pResultWidget = dynamic_cast<ResultWidget *>(pWidget);

    if (pResultWidget) {
        pResultWidget->copy();
    }
}

void DatabaseWidget::paste()
{
    QWidget *pWidget = focusWidget();

    QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pWidget);

    if (pLineEdit) {
        pLineEdit->paste();
        return;
    }

    QTextEdit *pTextEdit = dynamic_cast<QTextEdit *>(pWidget);

    if (pTextEdit) {
        pTextEdit->paste();
        return;
    }

    ResultWidget *pResultWidget = dynamic_cast<ResultWidget *>(pWidget);

    if (pResultWidget) {
        pResultWidget->paste();
    }
}

void DatabaseWidget::selectAll()
{
    QWidget *pWidget = focusWidget();

    QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pWidget);

    if (pLineEdit) {
        pLineEdit->selectAll();
        return;
    }

    QTextEdit *pTextEdit = dynamic_cast<QTextEdit *>(pWidget);

    if (pTextEdit) {
        pTextEdit->selectAll();
        return;
    }
}

bool DatabaseWidget::openDatabase(const QString &fileName)
{
    if (!m_sqlite) {
        int rc;
        m_sqlite = SQLite::create(fileName, rc);

        if (m_sqlite) {
            connect(m_sqlite.data(), SIGNAL(logMsg(QString)), this, SIGNAL(logMsg(QString)));
            emit active(m_sqlite);
            emit logMsg(tr("Opened db file %1").arg(fileName));
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool DatabaseWidget::closeDatabase()
{
    if (m_sqlite) {
        m_sqlite.clear();
        emit inactive();
    }
    return true;
}

bool DatabaseWidget::checkActiveTransaction()
{
    bool transaction = false;
    if (m_tableResults->isActiveTransaction()) {
        TransactionDialog dialog(this);
        int response = dialog.exec();

        if (response == QDialog::Rejected) {
            transaction = true;
        } else if (response == TransactionDialog::Commit) {
            m_tableResults->commit();
        } else {
            m_tableResults->rollback();
        }
    }

    return transaction;
}

void DatabaseWidget::execute()
{
    if (!m_queryEdit->isEnabled()) {
        return;
    }

    if (checkActiveTransaction()) {
        return;
    }

    // If part of the query is highlighted, execute just that portion.  o/w execute everything
    QTextCursor cursor = m_queryEdit->textCursor();
    QString selectedQueries = cursor.selectedText().simplified();

    int rc;
    QSharedPointer<ResultSet> resultSet;
    if (selectedQueries.isEmpty()) {
        resultSet = m_sqlite->exec(m_queryEdit->toPlainText().simplified(), rc);
    } else {
       resultSet = m_sqlite->exec(selectedQueries, rc);
    }

    if (rc != 0) {
        QString errmsg = m_sqlite->getErrMsg();
        QMessageBox::warning(this, tr("Error"), tr("Could not execute query: %1").arg(errmsg));
    }

    emit logMsg(tr("exec sqlite error: %1").arg(rc));
    // TODO: error handling.

    if (!resultSet.isNull()) {
        m_tableResults->displayResultSet(*resultSet);
    }

    // repopulate the table tree in case we added a table or
    m_tableTree->populateTableTree();
}

void DatabaseWidget::dumpTable(QTreeWidgetItem *item ,int)
{
    if (!item->parent() && !checkActiveTransaction()) {
        ExploreWidgetItem *pItem = static_cast<ExploreWidgetItem *>(item);
        m_tableResults->dumpTable(pItem->tableName());
    }
}
