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

#include "queryhighlighter.h"

QueryHighlighter::QueryHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{
    m_keywords << "ABORT" << "ACTION" << "ADD" << "AFTER" << "ALL" << "ALTER";
    m_keywords << "ANALYZE" << "AND" << "AS" << "ASC" << "ATTACH";
    m_keywords << "AUTOINCREMENT" << "BEFORE" << "BEGIN" << "BETWEEN" << "BY";
    m_keywords << "CASCADE" << "CASE" << "CAST" << "CHECK" << "COLLATE";
    m_keywords << "COLUMN" << "COMMIT" << "CONFLICT" << "CONSTRAINT";
    m_keywords << "CREATE" << "CROSS" << "CURRENT_DATE" << "CURRENT_TIME";
    m_keywords << "CURRENT_TIMESTAMP" << "DATABASE" << "DEFAULT";
    m_keywords << "DEFERRABLE" << "DEFERRED" << "DELETE" << "DESC" << "DETACH";
    m_keywords << "DISTINCT" << "DROP" << "EACH" << "ELSE" << "END" << "ESCAPE";
    m_keywords << "EXCEPT" << "EXCLUSIVE" << "EXISTS" << "EXPLAIN" << "FAIL";
    m_keywords << "FOR" << "FOREIGN" << "FROM" << "FULL" << "GLOB" << "GROUP";
    m_keywords << "HAVING" << "IF" << "IGNORE" << "IMMEDIATE" << "IN";
    m_keywords << "INDEX" << "INDEXED" << "INITIALLY" << "INNER" << "INSERT";
    m_keywords << "INSTEAD" << "INTERSECT" << "INTO" << "IS" << "ISNULL";
    m_keywords << "JOIN" << "KEY" << "LEFT" << "LIKE" << "LIMIT" << "MATCH";
    m_keywords << "NATURAL" << "NO" << "NOT" << "NOTNULL" << "NULL" << "OF";
    m_keywords << "OFFSET" << "ON" << "OR" << "ORDER" << "OUTER" << "PLAN";
    m_keywords << "PRAGMA" << "PRIMARY" << "QUERY" << "RAISE" << "REFERENCES";
    m_keywords << "REGEXP" << "REINDEX" << "RELEASE" << "RENAME" << "REPLACE";
    m_keywords << "RESTRICT" << "RIGHT" << "ROLLBACK" << "ROW" << "SAVEPOINT";
    m_keywords << "SELECT" << "SET" << "TABLE" << "TEMP" << "TEMPORARY";
    m_keywords << "THEN" << "TO" << "TRANSACTION" << "TRIGGER" << "UNION";
    m_keywords << "UNIQUE" << "UPDATE" << "USING" << "VACUUM" << "VALUES";
    m_keywords << "VIEW" << "VIRTUAL" << "WHEN" << "WHERE";


    m_separators << ' ' << ';' << '\t' << '=' << '>' << '<' << '+' << '-' << '*' << '/';
}

void QueryHighlighter::highlightBlock(const QString &text)
{
    enum { FindingStart, FindingEnd };

    int state = FindingStart;
    int start = 0;
    int end = 0;

    for (int i = 0; i < text.length(); ++i) {
        if (state == FindingStart) {
            if (!m_separators.contains(text.at(i)) && i != text.length()) {
                start = i;
                state = FindingEnd;
            }
        } else if (m_separators.contains(text.at(i)) || i == text.length()) {
            // Finding end.
            end = i;
            QString word = text.mid(start, end - start);
            if (m_keywords.contains(word.toUpper())) {
                setFormat(start, end - start, Qt::blue);
            }
            state = FindingStart;
        }
    }

    if (state == FindingEnd) {
        end = text.length();
        QString word = text.mid(start, end - start);
        if (m_keywords.contains(word.toUpper())) {
            setFormat(start, end - start, Qt::blue);
        }
    }
}
