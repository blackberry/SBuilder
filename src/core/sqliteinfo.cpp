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
#include "sqliteinfo.h"
#include "sqlite3.h"

SQLiteColumnInfo::SQLiteColumnInfo()
    : m_columnName("")
    , m_affinity("")
    , m_nullable(false)
    , m_defaultValue("")
    , m_pk(false)
{

}

void SQLiteColumnInfo::setColumnName(const QString &columnName)
{
    m_columnName = columnName;
}

void SQLiteColumnInfo::setAffinity(const QString &affinity)
{
    m_affinity = affinity;
}

void SQLiteColumnInfo::setNullable(bool nullable)
{
    m_nullable = nullable;
}

void SQLiteColumnInfo::setDefaultValue(const QString &defaultValue)
{
    m_defaultValue = defaultValue;
}

void SQLiteColumnInfo::setPrimaryKey(bool pk)
{
    m_pk = pk;
}


QString SQLiteColumnInfo::getColumnName() const
{
   return m_columnName;
}

QString SQLiteColumnInfo::getAffinity() const
{
    return m_affinity;
}

bool SQLiteColumnInfo::getNullable() const
{
    return m_nullable;
}

QString SQLiteColumnInfo::getDefaultValue() const
{
    return m_defaultValue;
}

bool SQLiteColumnInfo::isPrimaryKey() const
{
    return m_pk;
}

SQLiteTableInfo::SQLiteTableInfo(const QString &tableName)
    : m_isVirtual(false)
    , m_tableName(tableName)
{
}

SQLiteTableInfo::~SQLiteTableInfo()
{
    foreach(SQLiteColumnInfo *pInfo, m_columns) {
        delete pInfo;
    }
}

bool SQLiteTableInfo::isVirtual() const
{
    return m_isVirtual;
}

void SQLiteTableInfo::setVirtual(bool virtualTable)
{
    m_isVirtual = virtualTable;
}

bool SQLiteTableInfo::isPrimaryKey(int column) const
{
    SQLiteColumnInfo *pColumn = m_columns.at(column);
    return pColumn->isPrimaryKey();
}

QString SQLiteTableInfo::getColumnName(int column) const
{
    SQLiteColumnInfo *pColumn = m_columns.at(column);
    return pColumn->getColumnName();
}

QString SQLiteTableInfo::getColumnAffinity(int column) const
{
    SQLiteColumnInfo *pColumn = m_columns.at(column);
    QString affinity = pColumn->getAffinity();

    if (affinity.isEmpty()) {
        return "none";
    }

    return affinity;
}

SQLiteColumnInfo *SQLiteTableInfo::addColumnInfo()
{
    SQLiteColumnInfo *pInfo = new SQLiteColumnInfo;
    m_columns.append(pInfo);

    return pInfo;
}

bool SQLiteTableInfo::hasAffinity(int column) const
{
    SQLiteColumnInfo *pColumn = m_columns.at(column);
    QString affinity = pColumn->getAffinity();

    if (affinity.isEmpty()) {
        return false;
    }

    return true;
}

int SQLiteTableInfo::size() const
{
    return m_columns.size();
}

SQLiteMemoryInfo::SQLiteMemoryInfo()
{

}

static const int Current = 0;
static const int HighWatermark = 1;

long SQLiteMemoryInfo::getCurrentTotalMemoryUsed() const
{
    return m_totalMemoryUsed[Current];
}

long SQLiteMemoryInfo::getHighTotalMemoryUsed() const
{
    return m_totalMemoryUsed[HighWatermark];
}

void SQLiteMemoryInfo::setCurrentTotalMemoryUsed(long value)
{
    m_totalMemoryUsed[Current] = value;
}

void SQLiteMemoryInfo::setHighTotalMemoryUsed(long value)
{
    m_totalMemoryUsed[HighWatermark] = value;
}

long SQLiteMemoryInfo::getCurrentPageCacheSize() const
{
    return m_pageCacheSize[Current];
}

long SQLiteMemoryInfo::getHighPageCacheSize() const
{
    return m_pageCacheSize[HighWatermark];
}

void SQLiteMemoryInfo::setCurrentPageCacheSize(long value)
{
    m_pageCacheSize[Current] = value;
}

void SQLiteMemoryInfo::setHighPageCacheSize(long value)
{
    m_pageCacheSize[HighWatermark] = value;
}

long SQLiteMemoryInfo::getCurrentPageCacheUsed() const
{
    return m_pageCacheUsed[Current];
}

long SQLiteMemoryInfo::getHighPageCacheUsed() const
{
    return m_pageCacheUsed[HighWatermark];
}

void SQLiteMemoryInfo::setCurrentPageCacheUsed(long value)
{
    m_pageCacheUsed[Current] = value;
}

void SQLiteMemoryInfo::setHighPageCacheUsed(long value)
{
    m_pageCacheUsed[HighWatermark] = value;
}

long SQLiteMemoryInfo::getCurrentPageCacheOverflow() const
{
    return m_pageCacheOverflow[Current];
}

long SQLiteMemoryInfo::getHighPageCacheOverflow() const
{
    return m_pageCacheOverflow[HighWatermark];
}

void SQLiteMemoryInfo::setCurrentPageCacheOverflow(long value)
{
    m_pageCacheOverflow[Current] = value;
}

void SQLiteMemoryInfo::setHighPageCacheOverflow(long value)
{
    m_pageCacheOverflow[HighWatermark] = value;
}

long SQLiteMemoryInfo::getCurrentParserStack() const
{
    return m_parserStack[Current];
}

long SQLiteMemoryInfo::getHighParserStack() const
{
    return m_parserStack[HighWatermark];
}

void SQLiteMemoryInfo::setCurrentParserStack(long value)
{
    m_parserStack[Current] = value;
}

void SQLiteMemoryInfo::setHighParserStack(long value)
{
    m_parserStack[HighWatermark] = value;
}

long SQLiteMemoryInfo::getCurrentMallocCount() const
{
    return m_mallocCount[Current];
}

long SQLiteMemoryInfo::getHighMallocCount() const
{
    return m_mallocCount[HighWatermark];
}

void SQLiteMemoryInfo::setCurrentMallocCount(long value)
{
    m_mallocCount[Current] = value;
}

void SQLiteMemoryInfo::setHighMallocCount(long value)
{
    m_mallocCount[HighWatermark] = value;
}

long SQLiteMemoryInfo::getCurrentMallocSize() const
{
    return m_mallocSize[Current];
}

long SQLiteMemoryInfo::getHighMallocSize() const
{
    return m_mallocSize[HighWatermark];
}

void SQLiteMemoryInfo::setCurrentMallocSize(long value)
{
    m_mallocSize[Current] = value;
}

void SQLiteMemoryInfo::setHighMallocSize(long value)
{
    m_mallocSize[HighWatermark] = value;
}

