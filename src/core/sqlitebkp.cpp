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

#include "sqlitebkp.h"

#include <QtGui>

#include "sqlite.h"
#include "sqlite3.h"

QSharedPointer<SQLiteBkp> SQLiteBkp::create(QSharedPointer<SQLite> pSrc, QSharedPointer<SQLite> pDest)
{
    sqlite3_backup * bkp = sqlite3_backup_init(pDest->getHandle(), "main", pSrc->getHandle(), "main");

    if (bkp) {
        QSharedPointer<SQLiteBkp> pBkp(new SQLiteBkp(bkp));
        return pBkp;
    }

    // TODO: error handling
    return QSharedPointer<SQLiteBkp>(0);
}

SQLiteBkp::SQLiteBkp(sqlite3_backup *bkp)
    : m_bkp(bkp)
{
}

SQLiteBkp::~SQLiteBkp()
{
    sqlite3_backup_finish(m_bkp);
}

void SQLiteBkp::step(int &rc)
{
    rc = sqlite3_backup_step(m_bkp, -1);
}


