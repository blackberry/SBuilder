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

#include "blobviewerdialog.h"
#include "ui_blobviewerdialog.h"

BlobViewerDialog::BlobViewerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlobViewerDialog)
{
    ui->setupUi(this);


}

BlobViewerDialog::~BlobViewerDialog()
{
    delete ui;
}

bool BlobViewerDialog::useCell(QSharedPointer<SQLite> pDb, const QString &db, const QString &table, const QString &column, int rowid)
{
    int size;
    int rc;

    QScopedArrayPointer<unsigned char> data;
    unsigned char *temp;

    if (pDb->getCellRawData(db, table, column, rowid, &temp, &size, rc)) {
        data.reset(temp);
    } else {
        return false;
    }


    QImage img;
    if (img.loadFromData(data.data(), size)) {
        QPixmap pixmap = QPixmap::fromImage(img);

        if (pixmap.height() > (QApplication::desktop()->height() * 2/3)) {
            pixmap = pixmap.scaledToHeight(QApplication::desktop()->height() * 2/3);
        }

        if (pixmap.width() > (QApplication::desktop()->width() * 2/3)) {
            pixmap = pixmap.scaledToWidth(QApplication::desktop()->width() * 2/3);
        }

        ui->image->setPixmap(pixmap);
        return true;
    }

    QMessageBox::warning(this, tr("Not an image"), tr("The cell does not contain image in a format that SBuilder recognizes."));
    return false;
}
