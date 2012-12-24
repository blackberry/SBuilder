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

#ifndef SQLITEINFO_H
#define SQLITEINFO_H

#include <QList>
class QString;

class SQLiteColumnInfo
{
public:
    SQLiteColumnInfo();

    void setColumnName(const QString &columnName);
    void setAffinity(const QString &affinity);
    void setNullable(bool nullable);
    void setDefaultValue(const QString &defaultValue);
    void setPrimaryKey(bool pk);

    QString getColumnName() const;
    QString getAffinity() const;
    bool getNullable() const;
    QString getDefaultValue() const;
    bool isPrimaryKey() const;

private:
    QString m_columnName;
    QString m_affinity;
    bool m_nullable;
    QString m_defaultValue;
    bool m_pk;
};

class SQLiteTableInfo
{
public:
    SQLiteTableInfo(const QString &tableName);
    ~SQLiteTableInfo();

    QString getColumnName(int column) const;
    QString getColumnAffinity(int column) const;
    bool hasAffinity(int column) const;

    bool isVirtual() const;
    void setVirtual(bool virtualTable);

    bool isPrimaryKey(int column) const;

    int size() const;

    SQLiteColumnInfo *addColumnInfo();

private:
    bool m_isVirtual;
    QString m_tableName;
    QList<SQLiteColumnInfo *> m_columns;
};

class SQLiteMemoryInfo
{
public:
    SQLiteMemoryInfo();

    long getCurrentTotalMemoryUsed() const;
    long getHighTotalMemoryUsed() const;

    long getCurrentPageCacheSize() const;
    long getHighPageCacheSize() const;

    long getCurrentPageCacheUsed() const;
    long getHighPageCacheUsed() const;

    long getCurrentPageCacheOverflow() const;
    long getHighPageCacheOverflow() const;

    long getCurrentParserStack() const;
    long getHighParserStack() const;

    long getCurrentMallocCount() const;
    long getHighMallocCount() const;

    long getCurrentMallocSize() const;
    long getHighMallocSize() const;

    void setCurrentTotalMemoryUsed(long value);
    void setHighTotalMemoryUsed(long value);

    void setCurrentPageCacheSize(long value);
    void setHighPageCacheSize(long value);

    void setCurrentPageCacheUsed(long value);
    void setHighPageCacheUsed(long value);

    void setCurrentPageCacheOverflow(long value);
    void setHighPageCacheOverflow(long value);

    void setCurrentParserStack(long value);
    void setHighParserStack(long value);

    void setCurrentMallocCount(long value);
    void setHighMallocCount(long value);

    void setCurrentMallocSize(long value);
    void setHighMallocSize(long value);

private:
    long m_totalMemoryUsed[2];
    long m_pageCacheSize[2];
    long m_pageCacheUsed[2];
    long m_pageCacheOverflow[2];
    long m_parserStack[2];
    long m_mallocCount[2];
    long m_mallocSize[2];
};

#endif // SQLITEINFO_H
