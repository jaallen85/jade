// File: ExportDialog.h
// Copyright (C) 2023  Jason Allen
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include "OdgGlobal.h"

class QLineEdit;
class QRadioButton;
class QTextEdit;

class ExportDialog : public QDialog
{
    Q_OBJECT

private:
    bool mPromptOverwrite;
    QRectF mPageRect;
    QRectF mItemsRect;
    Odg::Units mUnits;

    QTextEdit* mPathEdit;
    QPushButton* mPathButton;

    QRadioButton* mPageRectButton;
    QRadioButton* mItemsRectButton;

    QLineEdit* mScaleEdit;
    QLineEdit* mWidthEdit;
    QLineEdit* mHeightEdit;

    QPushButton* mOkButton;
    QPushButton* mCancelButton;

public:
    ExportDialog(QWidget* parent = nullptr);

    void setPath(const QString& path);
    void setPromptOverwrite(bool prompt);
    void setPageRect(const QRectF& rect);
    void setItemsRect(const QRectF& rect);
    void setUnits(Odg::Units units);
    void setPixelsPerInch(double pixelsPerInch);
    void setExportItemsOnly(bool itemsOnly);
    QString path() const;
    bool shouldPromptOverwrite() const;
    QRectF pageRect() const;
    QRectF itemsRect() const;
    Odg::Units units() const;
    double pixelsPerInch() const;
    bool shouldExportItemsOnly() const;

public slots:
    void accept() override;

private slots:
    void browseForPath();
    void updateWidthAndHeightFromScale();
    void updateScaleAndHeightFromWidth();
    void updateScaleAndWidthFromHeight();
};

#endif
