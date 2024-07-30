// File: PreferencesDialog.cpp
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

#include "PreferencesDialog.h"
#include "DrawingPropertiesWidget.h"
#include "OdgDrawing.h"
#include "OdgStyle.h"
#include "SingleItemPropertiesWidget.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog(QWidget* parent) : QDialog(parent)
{
    // Create stacked widget
    mStackedWidget = new QStackedWidget();

    // Create list widget
    const QFontMetrics fontMetrics(font());
    const int width = fontMetrics.boundingRect("Drawing Defaults").width() + 24;
    const int height = fontMetrics.height();

    mListWidget = new QListWidget();

    mListWidget->setIconSize(QSize(2 * height, 2 * height));
    mListWidget->setGridSize(QSize(width, 4 * height));
    mListWidget->setViewMode(QListView::IconMode);
    mListWidget->setMovement(QListView::Static);
    mListWidget->setFixedWidth(width + 4);
    mListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(mListWidget, SIGNAL(currentRowChanged(int)), mStackedWidget, SLOT(setCurrentIndex(int)));

    // Create general widget
    mPromptOverwriteCheck = new QCheckBox("Prompt when overwriting existing files");
    mPromptCloseUnsavedCheck = new QCheckBox("Prompt when closing unsaved files");

    QGroupBox* promptGroup = new QGroupBox("Warnings");
    QVBoxLayout* promptLayout = new QVBoxLayout();
    promptLayout->addWidget(mPromptOverwriteCheck);
    promptLayout->addWidget(mPromptCloseUnsavedCheck);
    promptGroup->setLayout(promptLayout);

    QWidget* generalWidget = new QWidget();
    QVBoxLayout* generalLayout = new QVBoxLayout();
    generalLayout->addWidget(promptGroup);
    generalLayout->addWidget(new QWidget(), 100);
    generalWidget->setLayout(generalLayout);

    QListWidgetItem* generalItem = new QListWidgetItem(QIcon(":/icons/oxygen/configure.png"), "General");
    generalItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    generalItem->setTextAlignment(Qt::AlignCenter);
    mListWidget->addItem(generalItem);
    mStackedWidget->addWidget(generalWidget);

    // Create drawing defaults widget
    mDrawingPropertiesWidget = new DrawingPropertiesWidget();

    QWidget* drawingPropertiesWidget = new QWidget();
    QVBoxLayout* drawingPropertiesLayout = new QVBoxLayout();
    drawingPropertiesLayout->addWidget(mDrawingPropertiesWidget);
    drawingPropertiesLayout->addWidget(new QWidget(), 100);
    drawingPropertiesLayout->setContentsMargins(0, 0, 0, 0);
    drawingPropertiesWidget->setLayout(drawingPropertiesLayout);

    QListWidgetItem* drawingPropertiesItem = new QListWidgetItem(QIcon(":/icons/oxygen/document-properties.png"), "Drawing Defaults");
    drawingPropertiesItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    drawingPropertiesItem->setTextAlignment(Qt::AlignCenter);
    mListWidget->addItem(drawingPropertiesItem);
    mStackedWidget->addWidget(drawingPropertiesWidget);

    // Create item defaults widget
    mStylePropertiesWidget = new SingleItemPropertiesWidget();
    mStylePropertiesWidget->setUnits(mDrawingPropertiesWidget->units());
    connect(mDrawingPropertiesWidget, SIGNAL(unitsChanged(int)), mStylePropertiesWidget, SLOT(setUnits(int)));

    QWidget* itemPropertiesWidget = new QWidget();
    QVBoxLayout* itemPropertiesLayout = new QVBoxLayout();
    itemPropertiesLayout->addWidget(mStylePropertiesWidget);
    itemPropertiesLayout->addWidget(new QWidget(), 100);
    itemPropertiesLayout->setContentsMargins(0, 0, 0, 0);
    itemPropertiesWidget->setLayout(itemPropertiesLayout);

    QListWidgetItem* itemPropertiesItem = new QListWidgetItem(QIcon(":/icons/items/text-rectangle.png"), "Item Defaults");
    itemPropertiesItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    itemPropertiesItem->setTextAlignment(Qt::AlignCenter);
    mListWidget->addItem(itemPropertiesItem);
    mStackedWidget->addWidget(itemPropertiesWidget);

    // Create button box
    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->setCenterButtons(true);

    mOkButton = buttonBox->addButton(QDialogButtonBox::Ok);
    mCancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(mOkButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(mCancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

    // Assemble final layout
    QWidget *mainWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(mListWidget);
    mainLayout->addWidget(mStackedWidget, 100);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainWidget->setLayout(mainLayout);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(mainWidget, 100);
    layout->addWidget(buttonBox);
    setLayout(layout);

    setWindowTitle("Preferences");
    setMinimumSize(480, 600);
    resize(480, 600);
}

//======================================================================================================================

void PreferencesDialog::setPrompts(bool overwrite, bool closeUnsaved)
{
    mPromptOverwriteCheck->setChecked(overwrite);
    mPromptCloseUnsavedCheck->setChecked(closeUnsaved);
}

void PreferencesDialog::updatePrompts(bool& overwrite, bool& closeUnsaved)
{
    overwrite = mPromptOverwriteCheck->isChecked();
    closeUnsaved = mPromptCloseUnsavedCheck->isChecked();
}

//======================================================================================================================

void PreferencesDialog::setDrawingTemplate(OdgDrawing* drawingTemplate, OdgStyle* styleTemplate)
{
    if (drawingTemplate)
    {
        mDrawingPropertiesWidget->setUnits(drawingTemplate->units());
        mDrawingPropertiesWidget->setPageSize(drawingTemplate->pageSize());
        mDrawingPropertiesWidget->setPageMargins(drawingTemplate->pageMargins());
        mDrawingPropertiesWidget->setBackgroundColor(drawingTemplate->backgroundColor());

        mDrawingPropertiesWidget->setGrid(drawingTemplate->grid());
        mDrawingPropertiesWidget->setGridStyle(drawingTemplate->gridStyle());
        mDrawingPropertiesWidget->setGridColor(drawingTemplate->gridColor());
        mDrawingPropertiesWidget->setGridSpacingMajor(drawingTemplate->gridSpacingMajor());
        mDrawingPropertiesWidget->setGridSpacingMinor(drawingTemplate->gridSpacingMinor());

        mStylePropertiesWidget->setUnits(drawingTemplate->units());
    }

    if (styleTemplate)
    {
        mStylePropertiesWidget->setPenStyle(styleTemplate->penStyle());
        mStylePropertiesWidget->setPenWidth(styleTemplate->penWidth());
        mStylePropertiesWidget->setPenColor(styleTemplate->penColor());
        mStylePropertiesWidget->setBrushColor(styleTemplate->brushColor());

        mStylePropertiesWidget->setStartMarkerStyle(styleTemplate->startMarkerStyle());
        mStylePropertiesWidget->setStartMarkerSize(styleTemplate->startMarkerSize());
        mStylePropertiesWidget->setEndMarkerStyle(styleTemplate->endMarkerStyle());
        mStylePropertiesWidget->setEndMarkerSize(styleTemplate->endMarkerSize());

        mStylePropertiesWidget->setFontFamily(styleTemplate->fontFamily());
        mStylePropertiesWidget->setFontSize(styleTemplate->fontSize());
        mStylePropertiesWidget->setFontStyle(styleTemplate->fontStyle());
        mStylePropertiesWidget->setTextAlignment(styleTemplate->textAlignment());
        mStylePropertiesWidget->setTextPadding(styleTemplate->textPadding());
        mStylePropertiesWidget->setTextColor(styleTemplate->textColor());
    }
}

void PreferencesDialog::updateDrawingTemplate(OdgDrawing* drawingTemplate, OdgStyle* styleTemplate)
{
    if (drawingTemplate)
    {
        drawingTemplate->setUnits(mDrawingPropertiesWidget->units());
        drawingTemplate->setPageSize(mDrawingPropertiesWidget->pageSize());
        drawingTemplate->setPageMargins(mDrawingPropertiesWidget->pageMargins());
        drawingTemplate->setBackgroundColor(mDrawingPropertiesWidget->backgroundColor());

        drawingTemplate->setGrid(mDrawingPropertiesWidget->grid());
        drawingTemplate->setGridStyle(mDrawingPropertiesWidget->gridStyle());
        drawingTemplate->setGridColor(mDrawingPropertiesWidget->gridColor());
        drawingTemplate->setGridSpacingMajor(mDrawingPropertiesWidget->gridSpacingMajor());
        drawingTemplate->setGridSpacingMinor(mDrawingPropertiesWidget->gridSpacingMinor());
    }

    if (styleTemplate)
    {
        styleTemplate->setPenStyle(mStylePropertiesWidget->penStyle());
        styleTemplate->setPenWidth(mStylePropertiesWidget->penWidth());
        styleTemplate->setPenColor(mStylePropertiesWidget->penColor());
        styleTemplate->setBrushColor(mStylePropertiesWidget->brushColor());

        styleTemplate->setStartMarkerStyle(mStylePropertiesWidget->startMarkerStyle());
        styleTemplate->setStartMarkerSize(mStylePropertiesWidget->startMarkerSize());
        styleTemplate->setEndMarkerStyle(mStylePropertiesWidget->endMarkerStyle());
        styleTemplate->setEndMarkerSize(mStylePropertiesWidget->endMarkerSize());

        styleTemplate->setFontFamily(mStylePropertiesWidget->fontFamily());
        styleTemplate->setFontSize(mStylePropertiesWidget->fontSize());
        styleTemplate->setFontStyle(mStylePropertiesWidget->fontStyle());
        styleTemplate->setTextAlignment(mStylePropertiesWidget->textAlignment());
        styleTemplate->setTextPadding(mStylePropertiesWidget->textPadding());
        styleTemplate->setTextColor(mStylePropertiesWidget->textColor());
    }
}
