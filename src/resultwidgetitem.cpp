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
#include "sqlite3.h"
#include "sqlitestmt.h"
#include "resultwidgetitem.h"

static const int ResultType = QTableWidgetItem::UserType + 1;

ResultWidgetItem::ResultWidgetItem(QSharedPointer<SQLite> pDb, const QString &db, const QString &table, const QString &column, int rowid)
    : QTableWidgetItem(ResultType)
    , m_pDb(pDb)
    , m_db(db)
    , m_table(table)
    , m_column(column)
    , m_rowid(rowid)
    , m_state(Init)
{
    if (db.isEmpty() || table.isEmpty() || column.isEmpty()) {
        setFlags(flags() & ~Qt::ItemIsEditable);
    }
}

void ResultWidgetItem::setNull()
{
    if (data(Qt::UserRole).type() == QVariant::UserType) {
        return;
    }

    m_state = Dirty;

    setBackground(QBrush(Qt::red));

    QTableWidgetItem::setData(Qt::UserRole, QVariant(QVariant::UserType));
    QTableWidgetItem::setData(Qt::EditRole, "#NULL");

    int rc;
    m_pDb->updateTableCell(m_db, m_table, m_column, m_rowid, data(Qt::UserRole), rc);
}

void ResultWidgetItem::updateDatabase()
{
    // Assert: state is dirty
    if (m_state == Dirty) {
        QVariant newValue = data(Qt::UserRole);

        int rc;
        m_pDb->updateTableCell(m_db, m_table, m_column, m_rowid, newValue, rc);
        m_state = Clean;
        // TODO: error handling
    }
}

ResultWidgetItem::state ResultWidgetItem::getState() const
{
    return m_state;
}

void ResultWidgetItem::markClean()
{
    // Assert: state is dirty
    m_state = Clean;
}

QString ResultWidgetItem::getDb() const
{
    return m_db;
}

QString ResultWidgetItem::getTable() const
{
    return m_table;
}

QString ResultWidgetItem::getColumn() const
{
    return m_column;
}

int ResultWidgetItem::getRowId() const
{
    return m_rowid;
}

void ResultWidgetItem::setInitialValue(const QVariant &value)
{
    m_state = Init;
    setData(Qt::EditRole, value);
}

bool ResultWidgetItem::isPrintable(const QVariant &variant) const
{
    QString valueString = variant.toString();
    for (int i = 0; i < valueString.length(); i++) {
        QChar ch = valueString[i];

        if (!ch.isPrint()) {
            return false;
        }
    }

    return true;
}

void ResultWidgetItem::setData(int role, const QVariant &value)
{
    if (role == Qt::EditRole) {
        if (m_state == Init) {
            m_state = Clean;

            if (value.isNull()) {
                QTableWidgetItem::setData(Qt::EditRole, "#NULL");
                QTableWidgetItem::setData(Qt::UserRole, QVariant(QVariant::UserType));
            } else if (isPrintable(value)){
                QTableWidgetItem::setData(Qt::EditRole, value);
                QTableWidgetItem::setData(Qt::UserRole, value);
            } else {
                QTableWidgetItem::setData(Qt::EditRole, "#BLOB");
                QTableWidgetItem::setData(Qt::UserRole, QVariant(QVariant::UserType));
                setFlags(flags() & ~Qt::ItemIsEditable);
            }
        } else if (m_state == Clean) {
            if (data(Qt::UserRole) == value || data(Qt::UserRole).type() == QVariant::UserType) {
                // Return if the UserRole is the same as the new value, or
                // if the UserRole is set to NULL (since that is handled separately).
                // or if this is an unprintable blob (which we can't edit period).
                return;
            }

            m_state = Dirty;

            setBackground(QBrush(Qt::red));

            if (value.isNull()) {
                QTableWidgetItem::setData(Qt::UserRole, QVariant(QVariant::UserType));
                QTableWidgetItem::setData(Qt::EditRole, "#NULL");
            } else if (isPrintable(value)) {
                QTableWidgetItem::setData(Qt::UserRole, value);
                QTableWidgetItem::setData(Qt::EditRole, value);
            } else {
                QTableWidgetItem::setData(Qt::UserRole, QVariant(QVariant::UserType));
                QTableWidgetItem::setData(Qt::EditRole, "#BLOB");
            }

            int rc;
            m_pDb->updateTableCell(m_db, m_table, m_column, m_rowid, data(Qt::UserRole), rc);
        } else if (m_state == Dirty) {
            if (data(Qt::UserRole) == value || data(Qt::UserRole).type() == QVariant::UserType) {
                // Return if the UserRole is the same as the new value, or
                // if the UserRole is set to NULL (since that is handled separately.
                // or if this is an unprintable blob (which we can't edit period).
                return;
            }

            if (value.isNull()) {
                QTableWidgetItem::setData(Qt::UserRole, QVariant(QVariant::UserType));
                QTableWidgetItem::setData(Qt::EditRole, "#NULL");
            } else if (isPrintable(value)) {
                QTableWidgetItem::setData(Qt::UserRole, value);
                QTableWidgetItem::setData(Qt::EditRole, value);
            } else {
                QTableWidgetItem::setData(Qt::UserRole, QVariant(QVariant::UserType));
                QTableWidgetItem::setData(Qt::EditRole, "#BLOB");
            }

            int rc;
            m_pDb->updateTableCell(m_db, m_table, m_column, m_rowid, data(Qt::UserRole), rc);
        }
    } else {
        QTableWidgetItem::setData(role, value);
    }
}
