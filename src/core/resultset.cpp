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

#include "resultset.h"

ResultSet::ResultSet()
{
}

ResultSet::~ResultSet()
{
    if (!m_rows.isEmpty()) {
        foreach (QStringList *pData, m_rows) {
            m_rows.removeAll(pData);
            delete pData;
        }
    }
}

int ResultSet::size() const
{
    return m_rows.size();
}


void ResultSet::addRow(int columns, char *columnData[], char *columnNames[])
{
    if (m_columns.isEmpty()) {
        for (int i = 0; i < columns; i++) {
            m_columns.append(columnNames[i]);
        }
    }

    QStringList *pData = new QStringList();
    for (int i = 0; i < columns; i++) {
        pData->append(columnData[i]);
    }

    m_rows.append(pData);
}

QStringList ResultSet::getRow(int rowid) const
{
    if (rowid < m_rows.size() && rowid >= 0) {
        return *(m_rows.at(rowid));
    }

    return QStringList();
}

QStringList ResultSet::getColumns() const
{
    return m_columns;
}
