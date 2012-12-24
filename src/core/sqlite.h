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

#ifndef SQLITE_H
#define SQLITE_H

#include <QObject>
#include <QSharedPointer>
#include "resultset.h"
#include "sqliteinfo.h"
#include "sqliteio.h"

struct sqlite3;

class SQLiteStmt;
class SQLiteBkp;




/*
 * Wrap sqlite C code
 */
class SQLite : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<SQLite> create(const QString &fileName, int &rc);

    ~SQLite();

    QSharedPointer<ResultSet> exec(const QString &query, int &rc);

    QString fileName() const;
    QSharedPointer<SQLiteStmt> prepare(const QString &query, int &rc);
    QString getErrMsg() const;
    int getErrCode() const;

    bool begin(int &rc);
    bool commit(int &rc);
    bool rollback(int &rc);

    QStringList getTableNames(int &rc);
    QSharedPointer<SQLiteTableInfo> getTableInfo(const QString &tableName, int &rc);
    QSharedPointer<SQLiteMemoryInfo> getMemoryInfo(bool reset, int &rc);

    bool updateTableCell(const QString &db, const QString &table, const QString &column, int rowid, const QVariant &data, int &rc);
    bool insertTableRow(const QString &db, const QString &table, const QStringList &row, int &rc);
    bool deleteTableRow(const QString &db, const QString &table, int rowid, int &rc);

    bool setPragmaValue(const QString &pragma, const QString &value, int &rc);
    QString pragmaValue(const QString &pragma, int &rc);

    bool updateCellWithFile(const QString &db, const QString &table, const QString &column, int rowid, const QString &fileName, int &rc);

    bool exportCellToFile(const QString &db, const QString &table, const QString &column, int rowid, const QString &fileName, int &rc);

    bool getCellRawData(const QString &db, const QString &table, const QString &column, int rowid, unsigned char **data, int *size, int &rc);

    bool dropTable(const QString &table, int &rc);

signals:
    void logMsg(const QString &msg);

protected:
    SQLite(const QString &fileName, sqlite3 *pDb);

    friend class SQLiteBkp;
    sqlite3 *getHandle();

private:
    QString m_fileName;
    sqlite3 *m_pDb;
};


#endif // SQLITE_H
