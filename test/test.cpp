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

/*
 * Generate test databases and verify them after
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"

class Test {

    public:
        Test();
        ~Test();
        void run();

    private:
        void runColumnTypes();
        void runVirtualTable();

        void handleError(const char *marker, int rc);
        sqlite3 *m_pDb;
};

Test::Test() {
    sqlite3_initialize();
}

Test::~Test() {
    sqlite3_shutdown();
}

void Test::run() {
    runColumnTypes();
    runVirtualTable();
}

void Test::runColumnTypes() {
    int rc = sqlite3_open_v2("columntypes.db",
                             &m_pDb,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                             NULL);

    if (rc != SQLITE_OK) {
        printf("Error at open, error code %d\n", rc);
        exit(1);
    }

    const char *zSql = "create table columntypes (R REAL, I INTEGER, B BLOB, T TEXT);";
    const char *zTail;
    sqlite3_stmt *pStmt;
    rc = sqlite3_prepare_v2(m_pDb, zSql, strlen(zSql), &pStmt, &zTail);

    if (rc != SQLITE_OK) {
        handleError("prepare", rc);
    }

    rc = sqlite3_step(pStmt);

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(pStmt);
        handleError("step", rc);
    }

    sqlite3_finalize(pStmt);
    sqlite3_close(m_pDb);
}

void Test::runVirtualTable() {
    int rc = sqlite3_open_v2("virtualtable.db",
                             &m_pDb,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                             NULL);

    if (rc != SQLITE_OK) {
        printf("Error at open, error code %d\n", rc);
        exit(1);
    }


    const char *zSql = "CREATE VIRTUAL TABLE mail USING fts3(subject VARCHAR(256) NOT NULL, body TEXT CHECK(length(body)<10240));";
    const char *zTail;
    sqlite3_stmt *pStmt;
    rc = sqlite3_prepare_v2(m_pDb, zSql, strlen(zSql), &pStmt, &zTail);

    if (rc != SQLITE_OK) {
        handleError("prepare", rc);
    }

    rc = sqlite3_step(pStmt);

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(pStmt);
        handleError("step", rc);
    }

    sqlite3_finalize(pStmt);
    sqlite3_close(m_pDb);
}

void Test::handleError(const char *marker, int rc) {
    printf("Error at %s, error code %d, error str %s\n", marker, rc, sqlite3_errmsg(m_pDb));
    sqlite3_close(m_pDb);
    exit(1);
}

int main(int argc, char ** argv) {
    Test test;
    test.run();
}
