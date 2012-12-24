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

#include "memorydialog.h"
#include "ui_memorydialog.h"

#include "sqlite.h"
#include "sqliteinfo.h"

MemoryDialog::MemoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MemoryDialog)
{
    ui->setupUi(this);
    connect(ui->resetHighWatermarkButton, SIGNAL(clicked()), this, SLOT(resetHighWatermark()));

    ui->resetHighWatermarkButton->setEnabled(false);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateMemInfo()));
}

MemoryDialog::~MemoryDialog()
{
    delete ui;
}

void MemoryDialog::updateMemInfo()
{
    updateMemInfo(false);
}

void MemoryDialog::updateMemInfo(bool resetFlag)
{
    int rc;
    QSharedPointer<SQLiteMemoryInfo> pMemInfo(m_pDb->getMemoryInfo(resetFlag, rc));

    ui->currentTotalMemoryUsed->setText(tr("%1").arg(pMemInfo->getCurrentTotalMemoryUsed()));
    ui->highTotalMemoryUsed->setText(tr("%1").arg(pMemInfo->getHighTotalMemoryUsed()));

    ui->currentPageCacheSize->setText(tr("%1").arg(pMemInfo->getCurrentPageCacheSize()));
    ui->highPageCacheSize->setText(tr("%1").arg(pMemInfo->getHighPageCacheSize()));

    ui->currentPageCacheUsed->setText(tr("%1").arg(pMemInfo->getCurrentPageCacheUsed()));
    ui->highPageCacheUsed->setText(tr("%1").arg(pMemInfo->getHighPageCacheUsed()));

    ui->currentPageCacheOverflow->setText(tr("%1").arg(pMemInfo->getCurrentPageCacheOverflow()));
    ui->highPageCacheOverflow->setText(tr("%1").arg(pMemInfo->getHighPageCacheOverflow()));

    ui->currentParserStack->setText(tr("%1").arg(pMemInfo->getCurrentParserStack()));
    ui->highParserStack->setText(tr("%1").arg(pMemInfo->getHighParserStack()));

    ui->currentMallocCount->setText(tr("%1").arg(pMemInfo->getCurrentMallocCount()));
    ui->highMallocCount->setText(tr("%1").arg(pMemInfo->getHighMallocCount()));

    ui->currentMallocSize->setText(tr("%1").arg(pMemInfo->getCurrentMallocSize()));
    ui->highMallocSize->setText(tr("%1").arg(pMemInfo->getHighMallocSize()));
}

void MemoryDialog::active(QSharedPointer<SQLite> pDb)
{
    m_pDb = pDb;
    m_isActive = true;

    ui->resetHighWatermarkButton->setEnabled(true);
}

void MemoryDialog::inactive()
{
    m_pDb.clear();
    m_isActive = false;

    ui->resetHighWatermarkButton->setEnabled(false);
    close();
}

void MemoryDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous() && m_isActive) {
        updateMemInfo(false);

        // Start up the timer to fire 4 times a second.
        m_timer->start(250);
    }
}

void MemoryDialog::closeEvent(QCloseEvent *event)
{
    m_timer->stop();
    event->accept();
}

void MemoryDialog::resetHighWatermark()
{
    updateMemInfo(true);
}

