// File: ExportDialog.cpp
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

#include "ExportDialog.h"
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QVBoxLayout>

ExportDialog::ExportDialog(QWidget* parent) : QDialog(parent),
    mPromptOverwrite(true), mPageRect(), mItemsRect(), mUnits(Odg::UnitsInches), mPathEdit(nullptr), mPathButton(nullptr),
    mPageRectButton(nullptr), mItemsRectButton(nullptr), mScaleEdit(nullptr), mWidthEdit(nullptr), mHeightEdit(nullptr),
    mOkButton(nullptr), mCancelButton(nullptr)
{
    // Create path group
    mPathEdit = new QTextEdit();
    mPathButton = new QPushButton(QIcon(":/icons/oxygen/document-open.png"), "");
    connect(mPathButton, SIGNAL(clicked(bool)), this, SLOT(browseForPath()));

    QGroupBox* pathGroup = new QGroupBox("Path");
    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->addWidget(mPathEdit, 100);
    pathLayout->addWidget(mPathButton, 0, Qt::AlignRight | Qt::AlignTop);
    pathGroup->setLayout(pathLayout);

    // Create options group
    mPageRectButton = new QRadioButton("Export entire page");
    mItemsRectButton = new QRadioButton("Export page items only");
    connect(mPageRectButton, SIGNAL(clicked(bool)), this, SLOT(updateWidthAndHeightFromScale()));
    connect(mItemsRectButton, SIGNAL(clicked(bool)), this, SLOT(updateWidthAndHeightFromScale()));

    QGroupBox* optionsGroup = new QGroupBox("Options");
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    optionsLayout->addWidget(mPageRectButton);
    optionsLayout->addWidget(mItemsRectButton);
    optionsGroup->setLayout(optionsLayout);

    // Create size group
    mScaleEdit = new QLineEdit("1");
    mWidthEdit = new QLineEdit("100");
    mHeightEdit = new QLineEdit("100");

    QDoubleValidator* scaleValidator = new QDoubleValidator(0, 1E12, -1, this);
    QIntValidator* sizeValidator = new QIntValidator(0, 2147483647, this);
    mScaleEdit->setValidator(scaleValidator);
    mWidthEdit->setValidator(sizeValidator);
    mHeightEdit->setValidator(sizeValidator);

    connect(mScaleEdit, SIGNAL(editingFinished()), this, SLOT(updateWidthAndHeightFromScale()));
    connect(mWidthEdit, SIGNAL(editingFinished()), this, SLOT(updateScaleAndHeightFromWidth()));
    connect(mHeightEdit, SIGNAL(editingFinished()), this, SLOT(updateScaleAndWidthFromHeight()));

    int labelWidth = QFontMetrics(font()).boundingRect("Height:").width() + 12;
    QGroupBox* sizeGroup = new QGroupBox("Size");
    QFormLayout* sizeLayout = new QFormLayout();
    sizeLayout->addRow("Scale:", mScaleEdit);
    sizeLayout->addRow("Width:", mWidthEdit);
    sizeLayout->addRow("Height:", mHeightEdit);
    sizeLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    sizeLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    sizeLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    sizeLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    sizeGroup->setLayout(sizeLayout);

    // Create button box
    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->setCenterButtons(true);

    mOkButton = buttonBox->addButton(QDialogButtonBox::Ok);
    mCancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(mOkButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(mCancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

    // Assemble dialog layout
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(pathGroup);
    layout->addWidget(optionsGroup);
    layout->addWidget(sizeGroup);
    layout->addWidget(buttonBox);
    setLayout(layout);

    setMinimumSize(400, 380);
    resize(400, 380);
}

//======================================================================================================================

void ExportDialog::setPath(const QString& path)
{
    mPathEdit->setPlainText(path);
}

void ExportDialog::setPromptOverwrite(bool prompt)
{
    mPromptOverwrite = prompt;
}

void ExportDialog::setPageRect(const QRectF& rect)
{
    mPageRect = rect;
    updateWidthAndHeightFromScale();
}

void ExportDialog::setItemsRect(const QRectF& rect)
{
    mItemsRect = rect;
    updateWidthAndHeightFromScale();
}

void ExportDialog::setUnits(Odg::Units units)
{
    mUnits = units;
}

void ExportDialog::setPixelsPerInch(double pixelsPerInch)
{
    const double scale = (mUnits == Odg::UnitsInches) ? pixelsPerInch : pixelsPerInch * 25;
    mScaleEdit->setText(QString::number(scale, 'g', 8));
    updateWidthAndHeightFromScale();
}

void ExportDialog::setExportItemsOnly(bool itemsOnly)
{
    mPageRectButton->setChecked(!itemsOnly);
    mItemsRectButton->setChecked(itemsOnly);
    updateWidthAndHeightFromScale();
}

QString ExportDialog::path() const
{
    return mPathEdit->toPlainText();
}

bool ExportDialog::shouldPromptOverwrite() const
{
    return mPromptOverwrite;
}

QRectF ExportDialog::pageRect() const
{
    return mPageRect;
}

QRectF ExportDialog::itemsRect() const
{
    return mItemsRect;
}

double ExportDialog::pixelsPerInch() const
{
    bool scaleOk = false;
    const double scale = mScaleEdit->text().toDouble(&scaleOk);
    if (scaleOk)
        return (mUnits == Odg::UnitsInches) ? scale : scale / 25;
    return 600;
}

bool ExportDialog::shouldExportItemsOnly() const
{
    return mItemsRectButton->isChecked();
}

//======================================================================================================================

void ExportDialog::accept()
{
    bool scaleOk = false;
    const double scale = mScaleEdit->text().toDouble(&scaleOk);
    if (scaleOk && scale > 0 && !path().isEmpty()) QDialog::accept();
}

//======================================================================================================================

void ExportDialog::browseForPath()
{
    QString fileFilter;
    if (windowTitle().contains("SVG"))
        fileFilter = "Scalable Vector Graphics (*.svg);;All Files (*)";
    else
        fileFilter = "Portable Network Graphics (*.png);;All Files (*)";

    QFileDialog::Options options = (mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite;

    QString selectedPath = QFileDialog::getSaveFileName(this, windowTitle(), path(), fileFilter, nullptr, options);
    if (!selectedPath.isEmpty()) setPath(selectedPath);
}

void ExportDialog::updateWidthAndHeightFromScale()
{
    bool scaleOk = false;
    const double scale = mScaleEdit->text().toDouble(&scaleOk);
    if (scaleOk && scale > 0)
    {
        blockSignals(true);
        if (shouldExportItemsOnly())
        {
            mWidthEdit->setText(QString::number(qRound(mItemsRect.width() * scale)));
            mHeightEdit->setText(QString::number(qRound(mItemsRect.height() * scale)));
        }
        else
        {
            mWidthEdit->setText(QString::number(qRound(mPageRect.width() * scale)));
            mHeightEdit->setText(QString::number(qRound(mPageRect.height() * scale)));
        }
        blockSignals(false);
    }
}

void ExportDialog::updateScaleAndHeightFromWidth()
{
    bool widthOk = false;
    const double width = mWidthEdit->text().toDouble(&widthOk);
    if (widthOk && width > 0)
    {
        blockSignals(true);
        if (shouldExportItemsOnly())
        {
            const double scale = width / mItemsRect.width();
            mScaleEdit->setText(QString::number(scale, 'g', 8));
            mHeightEdit->setText(QString::number(qRound(mItemsRect.height() * scale)));
        }
        else
        {
            const double scale = width / mPageRect.width();
            mScaleEdit->setText(QString::number(scale, 'g', 8));
            mHeightEdit->setText(QString::number(qRound(mPageRect.height() * scale)));
        }
        blockSignals(false);
    }
}

void ExportDialog::updateScaleAndWidthFromHeight()
{
    bool heightOk = false;
    const double height = mHeightEdit->text().toDouble(&heightOk);
    if (heightOk && height > 0)
    {
        blockSignals(true);
        if (shouldExportItemsOnly())
        {
            const double scale = height / mItemsRect.height();
            mScaleEdit->setText(QString::number(scale, 'g', 8));
            mWidthEdit->setText(QString::number(qRound(mItemsRect.width() * scale)));
        }
        else
        {
            const double scale = height / mPageRect.height();
            mScaleEdit->setText(QString::number(scale, 'g', 8));
            mWidthEdit->setText(QString::number(qRound(mPageRect.width() * scale)));
        }
        blockSignals(false);
    }
}
