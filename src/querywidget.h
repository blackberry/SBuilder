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

#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <QTextEdit>
#include <QSharedPointer>
#include "sqlite.h"
#include "queryhighlighter.h"

class QAction;

class QueryWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit QueryWidget(QWidget *parent = 0);

    virtual void setEnabled(bool enabled);

signals:
    void execute();

public slots:
    void active(QSharedPointer<SQLite> pDb);
    void inactive();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:
    QSharedPointer<SQLite> m_pDb;
    QSharedPointer<QueryHighlighter> m_pHighlighter;
    QAction *m_executeAction;
};

#endif // QUERYWIDGET_H
