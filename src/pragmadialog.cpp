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

#include "pragmadialog.h"
#include "ui_pragmadialog.h"

PragmaDialog::PragmaDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PragmaDialog)
    , m_isActive(false)
{
    ui->setupUi(this);

    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(changeWidget(QWidget*)));

    QPushButton *saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    saveButton->setEnabled(false);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
}

PragmaDialog::~PragmaDialog()
{
    delete ui;
}

void PragmaDialog::active(QSharedPointer<SQLite> pDb)
{
    m_pDb = pDb;
    m_isActive = true;
}

void PragmaDialog::inactive()
{
    m_pDb.clear();
    m_isActive = false;
}

void PragmaDialog::save()
{
    int rc;
    foreach (QString pragmaChange, m_pragmaChanges.keys()) {
        if (!m_pDb->setPragmaValue(pragmaChange, m_pragmaChanges[pragmaChange], rc)) {
            QMessageBox::warning(this, "Save Pragma", tr("Could not save pragma %1.  rc: %2").arg(pragmaChange).arg(rc));
        }
    }

    m_pragmaChanges.clear();

    QPushButton *saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    saveButton->setEnabled(false);
}

void PragmaDialog::closeEvent(QCloseEvent *e)
{
    m_pragmaChanges.clear();
    e->accept();
}

void PragmaDialog::changeWidget(QWidget *pWidget)
{
    QPushButton *saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    saveButton->setEnabled(true);

    QComboBox *pComboBox = dynamic_cast<QComboBox *>(pWidget);
    QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pWidget);

    QString pragma = pWidget->objectName();
    QString value;
    if (pComboBox) {
        value = pComboBox->currentText();
    } else {
        value = pLineEdit->text();
    }

    m_pragmaChanges[pragma] = value;
}

void PragmaDialog::showEvent(QShowEvent *event)
{
    QPushButton *saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    saveButton->setEnabled(false);

    if (m_isActive) {
        // Let's cheat!  Grab all combo boxes and line edits and use there
        // name to query what the setting should be.  I'm lazy.
        QList<QObject*> children = this->children();

        foreach (QObject *pChild, children) {
            QComboBox *pComboBox = dynamic_cast<QComboBox *>(pChild);
            QLineEdit *pLineEdit = dynamic_cast<QLineEdit *>(pChild);

            if (!pComboBox && !pLineEdit) {
                continue;
            }

            int rc;
            QString value = m_pDb->pragmaValue(pChild->objectName(), rc);

            if (pComboBox) {
                setComboBox(pComboBox, value);
                pComboBox->setEnabled(true);
                connect(pComboBox, SIGNAL(currentIndexChanged(int)), m_signalMapper, SLOT(map()));
                m_signalMapper->setMapping(pComboBox, pComboBox);
            } else {
                setLineEdit(pLineEdit, value);
                pLineEdit->setEnabled(true);
                connect(pLineEdit, SIGNAL(textChanged(QString)), m_signalMapper, SLOT(map()));
                m_signalMapper->setMapping(pLineEdit, pLineEdit);
            }
        }
    }

    QDialog::showEvent(event);
}

void PragmaDialog::setComboBox(QComboBox *pComboBox, QString value)
{
    bool ok;
    int index = value.toInt(&ok);
    if (!ok) {
        index = pComboBox->findText(value, Qt::MatchFixedString);
    }
    pComboBox->setCurrentIndex(index);
}

void PragmaDialog::setLineEdit(QLineEdit *pLineEdit, QString value)
{
    pLineEdit->setText(value);
}
