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

#ifndef SQLITESTMT_H
#define SQLITESTMT_H

#include <QObject>

#ifdef _MSC_VER
typedef __int64 int64_t;
#endif

struct sqlite3_stmt;

class SQLiteStmt : public QObject
{
    Q_OBJECT

    friend class SQLite;

public:
    virtual ~SQLiteStmt();

    /*
     * Returns true if a row was fetched.  False o/w.
     */
    bool step(int &rc);

    /*
     * Result set operatons
     */

    // TODO: remove the "get" prefix, since Qt normally doesn't include it.
    int getType(int col);

    int getInt(int col);
    double getDouble(int col);
    QString getString(int col);

    int getColumnCount();
    QString getColumnName(int col);

    // Routines used by the result widget to update values.
    QString getColumnOriginName(int col);
    QString getColumnDatabaseName(int col);
    QString getColumnTableName(int col);

    // Bind routines
    bool bindDouble(int column, double param, int &rc);
    bool bindInt(int column, int param, int &rc);
    bool bindInt64(int column, int64_t param, int &rc);
    bool bindNull(int column, int &rc);
    bool bindText(int column, QString param, int &rc);
    bool bindZeroBlob(int column, int length, int &rc);

    // Takes a QString and will convert its type as
    // appropriate.
    bool bindConvert(int column, QString param, int &rc);

protected:
    SQLiteStmt(sqlite3_stmt *pStmt);

private:
    Q_DISABLE_COPY(SQLiteStmt);

    sqlite3_stmt *m_stmt;
};

#endif // SQLITESTMT_H
