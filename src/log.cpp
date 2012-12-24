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

#include "log.h"

Log::Log(QWidget *parent) :
    QDialog(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setObjectName(tr("mainLayout"));

    m_log = new QTextEdit(this);
    m_log->setReadOnly(true);

    m_mainLayout->addWidget(m_log);

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setObjectName(tr("buttonLayout"));

    m_buttonLayout->addStretch();

    m_clearButton = new QPushButton(tr("Clear"), this);
    connect(m_clearButton, SIGNAL(clicked()), m_log, SLOT(clear()));
    m_buttonLayout->addWidget(m_clearButton);

    m_hideButton = new QPushButton(tr("Hide"), this);
    connect(m_hideButton, SIGNAL(clicked()), this, SLOT(hide()));
    m_buttonLayout->addWidget(m_hideButton);

    m_mainLayout->addLayout(m_buttonLayout);
}

void Log::addMsg(const QString &msg)
{
    m_log->append(msg);
}

