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

#ifndef PRAGMADIALOG_H
#define PRAGMADIALOG_H

#include <QDialog>
#include <QSharedPointer>
#include <QHash>
#include <QString>
#include "sqlite.h"

class QComboBox;
class QLineEdit;
class QSignalMapper;

namespace Ui {
    class PragmaDialog;
}

class PragmaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PragmaDialog(QWidget *parent = 0);
    ~PragmaDialog();

public slots:
    void active(QSharedPointer<SQLite> pDb);
    void inactive();

protected slots:
    void changeWidget(QWidget*);
    void save();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);
    void changeComboBox(const QComboBox &comboBox);
    void changeLineEdit(const QLineEdit &lineEdit);

private:
    Ui::PragmaDialog *ui;


    bool m_isActive;
    QSharedPointer<SQLite> m_pDb;
    QSignalMapper *m_signalMapper;
    QHash<QString, QString> m_pragmaChanges;

    void setComboBox(QComboBox *pComboBox, QString value);
    void setLineEdit(QLineEdit *pLineEdit, QString value);

};

#endif // PRAGMADIALOG_H
