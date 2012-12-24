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

#include "sqlite3.h"
#include "sqliteio.h"

QSharedPointer<SQLiteIO> SQLiteIO::openBlob(sqlite3 *pDb, const QString &db, const QString &table, const QString &column, int rowid, int &rc)
{
    sqlite3_blob *pBlob;
    rc = sqlite3_blob_open(pDb,
                           db.toStdString().c_str(),
                           table.toStdString().c_str(),
                           column.toStdString().c_str(),
                           rowid,
                           1,
                           &pBlob);

    if (rc == SQLITE_OK) {
        return  QSharedPointer<SQLiteIO>(new SQLiteIO(pBlob));
    }

    return QSharedPointer<SQLiteIO>(0);
}

SQLiteIO::SQLiteIO(sqlite3_blob *pBlob)
{
    m_pBlob = pBlob;
}

SQLiteIO::~SQLiteIO()
{
    sqlite3_blob_close(m_pBlob);
}

bool SQLiteIO::write(const char *buffer, int size, int offset, int &rc)
{
    rc = sqlite3_blob_write(m_pBlob, buffer, size, offset);
    return (rc == SQLITE_OK);
}

bool SQLiteIO::read(char *buffer, int size, int offset, int &rc)
{
    rc = sqlite3_blob_read(m_pBlob, buffer, size, offset);
    return (rc == SQLITE_OK);
}

int SQLiteIO::size()
{
    return sqlite3_blob_bytes(m_pBlob);
}
