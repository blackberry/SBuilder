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

#include "querywidget.h"
#include "sqlite.h"

static const QString DefaultQuery = "select * from sqlite_master;";

QueryWidget::QueryWidget(QWidget *parent) :
    QTextEdit(parent)
{
    m_pHighlighter = QSharedPointer<QueryHighlighter>(new QueryHighlighter(this->document()));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    setEnabled(false);

    m_executeAction = new QAction(this);
    m_executeAction->setText(tr("Run"));
    connect(m_executeAction, SIGNAL(triggered()), this, SIGNAL(execute()));
}

void QueryWidget::setEnabled(bool enabled)
{
    QTextEdit::setEnabled(enabled);
    if (!enabled) {
        setText(DefaultQuery);
    }
}

void QueryWidget::active(QSharedPointer<SQLite> pDb)
{
    setEnabled(true);
    m_pDb = pDb;
}

void QueryWidget::inactive()
{
    setEnabled(false);
    m_pDb.clear();
}

void QueryWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QScopedPointer<QMenu> menu(createStandardContextMenu());

    menu->addSeparator();
    menu->addAction(m_executeAction);

    menu->exec(e->globalPos());
}

