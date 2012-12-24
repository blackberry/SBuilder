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
#include "resultset.h"
#include "sqlitestmt.h"
#include "sqlite.h"
#include "sqlite3.h"

QSharedPointer<SQLite> SQLite::create(const QString &fileName, int &rc)
{
    sqlite3 *pDb = 0;
    rc = sqlite3_open_v2(fileName.toStdString().c_str(), &pDb, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, 0);

    if (pDb) {
        return QSharedPointer<SQLite>(new SQLite(fileName, pDb));
    }

    // TODO: Is this the correct approach?
    return QSharedPointer<SQLite>(0);
}

SQLite::SQLite(const QString &fileName, sqlite3 *pDb)
    : m_fileName(fileName)
    , m_pDb(pDb)
{
}

SQLite::~SQLite()
{
    sqlite3_close(m_pDb);
}


// Useful callback (maybe) for sqlite3_exec
static int execCallback(void *pCtx, int columns, char *columnData[], char *columnNames[])
{
    ResultSet *pResultSet = static_cast<ResultSet *>(pCtx);
    pResultSet->addRow(columns, columnData, columnNames);

    return 0;
}


QSharedPointer<ResultSet> SQLite::exec(const QString &query, int &rc)
{
    logMsg(query);
    char *errmsg = 0;
    QSharedPointer<ResultSet> resultSet(new ResultSet);
    rc = sqlite3_exec(m_pDb, query.toStdString().c_str(), &execCallback, resultSet.data(), &errmsg);

    if (errmsg) {
        logMsg(QString(errmsg));
    }

    return resultSet;
}

bool SQLite::insertTableRow(const QString &db, const QString &table, const QStringList &row, int &rc)
{
    QString insertQuery = tr("insert into '%1'.'%2' values (").arg(db).arg(table);
    QStringList bindParams;
    for (int i = 0; i < row.size(); i++) {
        bindParams += tr("?");
    }
    insertQuery.append(bindParams.join(",")).append(")");

    QSharedPointer<SQLiteTableInfo> pTableInfo = getTableInfo(table, rc);
    if (pTableInfo.isNull()) {
        return false;
    }

    QSharedPointer<SQLiteStmt> pStmt = prepare(insertQuery, rc);

    if (pStmt.isNull()) {
        return false;
    }

    for (int i = 0; i < row.size(); i++) {
        bool isBound;

        if (row.at(i).isNull()) {
            isBound = pStmt->bindNull(i + 1, rc);
        } else if (pTableInfo->hasAffinity(i)) {
            isBound = pStmt->bindText(i + 1, row.at(i), rc);
        } else {
            isBound = pStmt->bindConvert(i + 1, row.at(i), rc);
        }

        if (!isBound) {
            return false;
        }
    }

    pStmt->step(rc);

    if (SQLITE_DONE != rc) {
        return false;
    }

    return true;
}

bool SQLite::setPragmaValue(const QString &pragma, const QString &value, int &rc)
{
    QString pragmaQuery = tr("PRAGMA %1=%2").arg(pragma).arg(value);

    QSharedPointer<SQLiteStmt> pStmt = prepare(pragmaQuery, rc);

    if (pStmt.isNull()) {
        return false;
    }

    pStmt->step(rc);

    if (SQLITE_DONE != rc) {
        return false;
    }

    return true;
}

QString SQLite::pragmaValue(const QString &pragma, int &rc)
{
    QString pragmaQuery = tr("PRAGMA %1").arg(pragma);

    QSharedPointer<SQLiteStmt> pStmt = prepare(pragmaQuery, rc);

    if (pStmt.isNull()) {
        return "";
    }

    pStmt->step(rc);

    if (SQLITE_ROW != rc) {
        return "";
    }

    return pStmt->getString(0);
}

bool SQLite::deleteTableRow(const QString &db, const QString &table, int rowid, int &rc)
{
    QString deleteQuery = tr("delete from '%1'.'%2' where ROWID=%3").arg(db).arg(table).arg(rowid);

    QSharedPointer<SQLiteStmt> pStmt = prepare(deleteQuery, rc);

    if (pStmt.isNull()) {
        return false;
    }

    pStmt->step(rc);

    if (SQLITE_DONE != rc) {
        return false;
    }

    return true;
}


bool SQLite::updateTableCell(const QString &db, const QString &table, const QString &column, int rowid, const QVariant &data, int &rc)
{
    QSharedPointer<SQLiteTableInfo> pTableInfo = getTableInfo(table, rc);
    if (pTableInfo.isNull()) {
        return false;
    }

    bool hasAffinity;
    for(int i = 0; i < pTableInfo->size(); i++) {
        if (pTableInfo->getColumnName(i).compare(column) == 0) {
            hasAffinity = pTableInfo->hasAffinity(i);
        }
    }

    QString updateQuery = tr("update '%1'.'%2' set %3=? where rowid=%4")
                          .arg(db)
                          .arg(table)
                          .arg(column)
                          .arg(rowid);

    QSharedPointer<SQLiteStmt> pStmt = prepare(updateQuery, rc);

    if (pStmt.isNull()) {
        return false;
    }

    bool isBound = false;
    if (data.type() == QVariant::UserType) {
        isBound = pStmt->bindNull(1, rc);
    } else {
        // If there is no column affinity, we should try to intelligently bind to the
        // right type.  If there is column affinity, we will simply bind as text, and
        // let SQLite figure out the appropriate storage container.
        if (hasAffinity) {
            isBound = pStmt->bindText(1, data.toString(), rc);
        } else {
            isBound = pStmt->bindConvert(1, data.toString(), rc);
        }
    }

    if (!isBound) {
        return false;
    }

    pStmt->step(rc);

    if (SQLITE_DONE != rc) {
        return false;
    }

    return true;
}


bool SQLite::updateCellWithFile(const QString &db, const QString &table, const QString &column, int rowid, const QString &fileName, int &rc)
{
    // Rather than mess around with types, let's just assume binary and use a BLOB.
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        rc = SQLITE_IOERR;
        return false;
    }

    // First reserve enough space in the row to add the file contents

    QString updateQuery = tr("update '%1'.'%2' set %3=? where rowid=%4")
                          .arg(db)
                          .arg(table)
                          .arg(column)
                          .arg(rowid);

    QSharedPointer<SQLiteStmt> pStmt = prepare(updateQuery, rc);

    if (pStmt.isNull()) {
        return false;
    }

    if (!pStmt->bindZeroBlob(1, file.size(), rc)) {
        return false;
    }

    pStmt->step(rc);

    if (rc != SQLITE_DONE) {
        return false;
    }

    pStmt.clear();

    // Now open a blob stream and write to it in chunks.
    QDataStream in(&file);

    QSharedPointer<SQLiteIO> pIo = SQLiteIO::openBlob(m_pDb, db, table, column, rowid, rc);

    if (pIo.isNull()) {
        return false;
    }

    char buf[1024];
    int offset = 0;
    while (!in.atEnd()) {
        int bytes = in.readRawData(buf, 1024);

        if (!pIo->write(buf, bytes, offset, rc)) {
            return false;
        }

        offset += bytes;
    }

    return true;
}

bool SQLite::exportCellToFile(const QString &db, const QString &table, const QString &column, int rowid, const QString &fileName, int &rc)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        rc = SQLITE_IOERR;
        return false;
    }

    QDataStream out(&file);

    QSharedPointer<SQLiteIO> pIo = SQLiteIO::openBlob(m_pDb, db, table, column, rowid, rc);

    if (pIo.isNull()) {
        return false;
    }

    int bytes = pIo->size();

    char buffer[1024];

    for (int read = 0; read < bytes; read += 1024) {
        int size = ((bytes - read) < 1024) ? (bytes - read) : 1024;
        if (!pIo->read(buffer, size, read, rc)) {
            return false;
        }

        out.writeRawData(buffer, size);
    }

    return true;
}

bool SQLite::getCellRawData(const QString &db, const QString &table, const QString &column, int rowid, unsigned char **data, int *size, int &rc)
{
    QSharedPointer<SQLiteIO> pIo = SQLiteIO::openBlob(m_pDb, db, table, column, rowid, rc);

    if (pIo.isNull()) {
        return false;
    }

    int bytes = pIo->size();

    QScopedArrayPointer<unsigned char> buffer(new unsigned char[bytes]);
    unsigned char *pBuf = buffer.data();
    memset(pBuf, 0, bytes);

    int read = 0;
    while (read < bytes) {
        int size = ((bytes - read) < 1024) ? (bytes - read) : 1024;

        if (!pIo->read((char*)&pBuf[read], size, read, rc)) {
            return false;
        }

        read += size;
    }

    *size = bytes;
    *data = buffer.take();
    return true;
}

QString SQLite::fileName() const
{
    return m_fileName;
}

QSharedPointer<SQLiteStmt> SQLite::prepare(const QString &query, int &rc)
{
    const char *zTail;
    sqlite3_stmt *pStmt;
    rc = sqlite3_prepare(m_pDb, query.toStdString().c_str(), query.length(), &pStmt, &zTail);
    emit logMsg(query);

    if (SQLITE_OK == rc) {
        return QSharedPointer<SQLiteStmt>(new SQLiteStmt(pStmt));
    }

    return QSharedPointer<SQLiteStmt>(0);
}



sqlite3 *SQLite::getHandle()
{
    return m_pDb;
}

QString SQLite::getErrMsg() const
{
    return sqlite3_errmsg(m_pDb);
}

int SQLite::getErrCode() const
{
    return sqlite3_errcode(m_pDb);
}

bool SQLite::begin(int &rc)
{
    char *errmsg;
    rc = sqlite3_exec(m_pDb, "begin transaction", 0, 0, &errmsg);

    if (rc == SQLITE_OK) {
        return true;
    }

    return false;
}

bool SQLite::commit(int &rc)
{
    char *errmsg;
    rc = sqlite3_exec(m_pDb, "commit transaction", 0, 0, &errmsg);

    if (rc == SQLITE_OK) {
        return true;
    }

    return false;
}

bool SQLite::rollback(int &rc)
{
    char *errmsg;
    rc = sqlite3_exec(m_pDb, "rollback transaction", 0, 0, &errmsg);

    if (rc == SQLITE_OK) {
        return true;
    }

    return false;
}

QStringList SQLite::getTableNames(int &rc)
{
    QString tableNamesQuery = tr("select name from sqlite_master where type like 'table'");
    QSharedPointer<SQLiteStmt> pStmt = prepare(tableNamesQuery, rc);

    QStringList tableNames;

    if (pStmt.isNull()) {
        return tableNames;
    }

    tableNames += "sqlite_master";
    while (pStmt->step(rc)) {
        tableNames += pStmt->getString(0);
    }

    return tableNames;
}

bool SQLite::dropTable(const QString &table, int &rc)
{
    QSharedPointer<SQLiteStmt> pDropStmt(prepare(tr("drop table '%1'").arg(table), rc));

    if (pDropStmt.isNull()) {
        return false;
    }

    pDropStmt->step(rc);

    if (rc != SQLITE_DONE) {
        return false;
    }

    return true;
}

QSharedPointer<SQLiteTableInfo> SQLite::getTableInfo(const QString &tableName, int &rc)
{
    // Get all the column info
    QSharedPointer<SQLiteStmt> pPragmaStmt(prepare(tr("PRAGMA table_info(%1)").arg(tableName), rc));

    if (pPragmaStmt.isNull()) {
        return QSharedPointer<SQLiteTableInfo>(0);
    }

    QSharedPointer<SQLiteTableInfo> pTableInfo(new SQLiteTableInfo(tableName));

    while (pPragmaStmt->step(rc)) {
        SQLiteColumnInfo *pColumn = pTableInfo->addColumnInfo();
        pColumn->setColumnName(pPragmaStmt->getString(1));
        pColumn->setAffinity(pPragmaStmt->getString(2));
        pColumn->setNullable(pPragmaStmt->getInt(3));
        pColumn->setDefaultValue(pPragmaStmt->getString(4));
        pColumn->setPrimaryKey(static_cast<bool>(pPragmaStmt->getInt(5)));
    }

    // Now lets pull some attributes of the table itself from the sqlite_master
    QString masterQuery = tr("select sql from sqlite_master where type LIKE 'table' and name LIKE '%1';").arg(tableName);
    QSharedPointer<SQLiteStmt> pMasterStmt = prepare(masterQuery, rc);

    if (pMasterStmt.isNull()) {
        return QSharedPointer<SQLiteTableInfo>(0);
    }

    pMasterStmt->step(rc);

    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        return QSharedPointer<SQLiteTableInfo>(0);
    }

    QString createTableQuery = pMasterStmt->getString(0);

    if (createTableQuery.toLower().startsWith("create virtual table")) {
        pTableInfo->setVirtual(true);
    }

    return pTableInfo;
}

QSharedPointer<SQLiteMemoryInfo> SQLite::getMemoryInfo(bool reset, int &rc)
{
    QSharedPointer<SQLiteMemoryInfo> pMemInfo(new SQLiteMemoryInfo());

    int current;
    int high;

    int resetFlag = (int)reset;

    rc = sqlite3_status(SQLITE_STATUS_MEMORY_USED, &current, &high, resetFlag);

    if (rc != SQLITE_OK) {
        return QSharedPointer<SQLiteMemoryInfo>(0);
    }

    pMemInfo->setCurrentTotalMemoryUsed(current);
    pMemInfo->setHighTotalMemoryUsed(high);

    rc = sqlite3_status(SQLITE_STATUS_PAGECACHE_USED, &current, &high, resetFlag);

    if (rc != SQLITE_OK) {
        return QSharedPointer<SQLiteMemoryInfo>(0);
    }

    pMemInfo->setCurrentPageCacheUsed(current);
    pMemInfo->setHighPageCacheUsed(high);

    rc = sqlite3_status(SQLITE_STATUS_PAGECACHE_OVERFLOW, &current, &high, resetFlag);

    if (rc != SQLITE_OK) {
        return QSharedPointer<SQLiteMemoryInfo>(0);
    }

    pMemInfo->setCurrentPageCacheOverflow(current);
    pMemInfo->setHighPageCacheOverflow(high);

    rc = sqlite3_status(SQLITE_STATUS_MALLOC_SIZE, &current, &high, resetFlag);

    if (rc != SQLITE_OK) {
        return QSharedPointer<SQLiteMemoryInfo>(0);
    }

    pMemInfo->setCurrentMallocSize(current);
    pMemInfo->setHighMallocSize(high);

    rc = sqlite3_status(SQLITE_STATUS_PARSER_STACK, &current, &high, resetFlag);

    if (rc != SQLITE_OK) {
        return QSharedPointer<SQLiteMemoryInfo>(0);
    }

    pMemInfo->setCurrentParserStack(current);
    pMemInfo->setHighParserStack(high);

    rc = sqlite3_status(SQLITE_STATUS_PAGECACHE_SIZE, &current, &high, resetFlag);

    if (rc != SQLITE_OK) {
        return QSharedPointer<SQLiteMemoryInfo>(0);
    }

    pMemInfo->setCurrentPageCacheSize(current);
    pMemInfo->setHighPageCacheSize(high);

    rc = sqlite3_status(SQLITE_STATUS_MALLOC_COUNT, &current, &high, resetFlag);

    if (rc != SQLITE_OK) {
        return QSharedPointer<SQLiteMemoryInfo>(0);
    }

    pMemInfo->setCurrentMallocCount(current);
    pMemInfo->setHighMallocCount(high);

    return pMemInfo;
}
