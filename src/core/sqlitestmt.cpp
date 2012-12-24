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

#include "sqlitestmt.h"

#include "sqlite3.h"

SQLiteStmt::SQLiteStmt(sqlite3_stmt *pStmt)
    : m_stmt(pStmt)
{
}

SQLiteStmt::~SQLiteStmt()
{
    sqlite3_finalize(m_stmt);
}

bool SQLiteStmt::step(int &rc)
{
    rc = sqlite3_step(m_stmt);

    if (SQLITE_ROW == rc) {
        return true;
    }

    return false;
}

int SQLiteStmt::getInt(int col)
{
    return sqlite3_column_int(m_stmt, col);
}

double SQLiteStmt::getDouble(int col)
{
    return sqlite3_column_double(m_stmt, col);
}

QString SQLiteStmt::getString(int col)
{
    QString str((const char *)sqlite3_column_text(m_stmt, col));
    return str;
}

int SQLiteStmt::getType(int col)
{
    return sqlite3_column_type(m_stmt, col);
}

int SQLiteStmt::getColumnCount()
{
    return sqlite3_column_count(m_stmt);
}

QString SQLiteStmt::getColumnName(int col)
{
    return sqlite3_column_name(m_stmt, col);
}

QString SQLiteStmt::getColumnOriginName(int col)
{
    return sqlite3_column_origin_name(m_stmt, col);
}

QString SQLiteStmt::getColumnDatabaseName(int col)
{
    return sqlite3_column_database_name(m_stmt, col);
}

QString SQLiteStmt::getColumnTableName(int col)
{
    return sqlite3_column_table_name(m_stmt, col);
}

bool SQLiteStmt::bindDouble(int column, double param, int &rc)
{
    rc = sqlite3_bind_double(m_stmt, column, param);

    return (rc == SQLITE_OK);
}

bool SQLiteStmt::bindInt(int column, int param, int &rc)
{
    rc = sqlite3_bind_int(m_stmt, column, param);

    return (rc == SQLITE_OK);
}

bool SQLiteStmt::bindInt64(int column, int64_t param, int &rc)
{
    rc = sqlite3_bind_int64(m_stmt, column, param);

    return (rc == SQLITE_OK);
}

bool SQLiteStmt::bindNull(int column, int &rc)
{
    rc = sqlite3_bind_null(m_stmt, column);

    return (rc == SQLITE_OK);
}

bool SQLiteStmt::bindZeroBlob(int column, int length, int &rc)
{
    rc = sqlite3_bind_zeroblob(m_stmt, column, length);

    return (rc == SQLITE_OK);
}

bool SQLiteStmt::bindText(int column, QString param, int &rc)
{
    rc = sqlite3_bind_text(m_stmt,
                           column,
                           param.toStdString().c_str(),
                           param.length(),
                           SQLITE_TRANSIENT);

    return (rc == SQLITE_OK);
}

bool SQLiteStmt::bindConvert(int column, QString value, int &rc)
{
    bool isBound = false;

    bool okInt;
    int asInt = value.toInt(&okInt);

    bool okDouble;
    double asDouble = value.toDouble(&okDouble);

    bool okLongLong;
    qlonglong asLongLong = value.toLongLong(&okLongLong);

    if (okInt) {
        isBound = bindInt(column, asInt, rc);
    } else if (okLongLong) {
        isBound = bindInt64(column, asLongLong, rc);
    } else if (okDouble) {
        isBound = bindDouble(column, asDouble, rc);
    } else {
        isBound = bindText(column, value, rc);
    }

    return isBound;
}
