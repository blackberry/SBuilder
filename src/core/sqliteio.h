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

#ifndef SQLITEIO_H
#define SQLITEIO_H

#include <QObject>
#include <QSharedPointer>
#include "sqlite.h"
#include "sqlite3.h"

class SQLite;

class SQLiteIO : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<SQLiteIO> openBlob(sqlite3 *pDb,
                                             const QString &db,
                                             const QString &table,
                                             const QString &column,
                                             int rowid,
                                             int &rc);

    ~SQLiteIO();

    bool write(const char *buffer, int size, int offset, int &rc);
    bool read(char *buffer, int size, int offset, int &rc);
    int size();

protected:
    SQLiteIO(sqlite3_blob *pBlob);
    Q_DISABLE_COPY(SQLiteIO);

    sqlite3_blob *m_pBlob;
};

#endif // SQLITEIO_H
