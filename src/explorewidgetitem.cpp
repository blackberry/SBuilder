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

#include "explorewidgetitem.h"

ExploreWidgetItem::ExploreWidgetItem()
    : QTreeWidgetItem()
    , m_tableName("")
    , m_columnName("")
{
}

void ExploreWidgetItem::setTableName(const QString &tableName, bool isVirtual)
{
    m_tableName = tableName;

    QString displayName = m_tableName;
    if (isVirtual) {
        displayName += " (virtual)";
    }

    setText(0, displayName);
}

void ExploreWidgetItem::setColumnName(const QString &columnName, const QString &affinity, bool isPk)
{
    m_columnName = columnName;

    QString displayName = m_columnName;

    displayName += ": ";
    displayName += affinity.toLower();
    if (isPk) {
        displayName += " (pk)";
    }

    setText(0, displayName);
}

QString ExploreWidgetItem::tableName() const
{
    return m_tableName;
}

bool ExploreWidgetItem::isTable() const
{
    return (m_tableName.size() > 0);
}
