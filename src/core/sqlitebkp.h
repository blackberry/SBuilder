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

#ifndef SQLITEBKP_H
#define SQLITEBKP_H

#include <QObject>
#include <QSharedPointer>

#include "sqlite.h"

struct sqlite3_backup;

class SQLiteBkp : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<SQLiteBkp> create(QSharedPointer<SQLite> pSrc, QSharedPointer<SQLite> pDest);
    ~SQLiteBkp();

    void step(int &rc);


protected:
    SQLiteBkp(sqlite3_backup *bkp);

private:
    sqlite3_backup *m_bkp;

};

#endif // SQLITEBKP_H
