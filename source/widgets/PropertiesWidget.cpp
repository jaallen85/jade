// File: PropertiesWidget.cpp
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

#include "PropertiesWidget.h"
#include "DrawingPropertiesWidget.h"
#include "DrawingWidget.h"
#include <QScrollArea>
#include <QVBoxLayout>

PropertiesWidget::PropertiesWidget(DrawingWidget* drawing) : QStackedWidget(),
    mDrawing(drawing), mDrawingPropertiesWidget(nullptr), mDrawingPropertiesScrollArea(nullptr)
{
    Q_ASSERT(mDrawing != nullptr);

    mDrawingPropertiesWidget = new DrawingPropertiesWidget();

    QWidget* drawingPropertiesWidget = new QWidget();
    QVBoxLayout* drawingPropertiesLayout = new QVBoxLayout();
    drawingPropertiesLayout->addWidget(mDrawingPropertiesWidget);
    drawingPropertiesLayout->addWidget(new QWidget(), 100);
    drawingPropertiesLayout->setContentsMargins(0, 0, 0, 0);
    drawingPropertiesWidget->setLayout(drawingPropertiesLayout);

    mDrawingPropertiesScrollArea = new QScrollArea();
    mDrawingPropertiesScrollArea->setWidget(drawingPropertiesWidget);
    mDrawingPropertiesScrollArea->setWidgetResizable(true);
    addWidget(mDrawingPropertiesScrollArea);

    connect(mDrawing, SIGNAL(propertyChanged(QString,QVariant)), this, SLOT(setDrawingProperty(QString,QVariant)));
    connect(mDrawing, SIGNAL(propertiesChanged()), this, SLOT(setAllDrawingProperties()));
}

//======================================================================================================================

void PropertiesWidget::setAllDrawingProperties()
{
    mDrawingPropertiesWidget->setUnits(mDrawing->units());
    mDrawingPropertiesWidget->setPageSize(mDrawing->pageSize());
    mDrawingPropertiesWidget->setPageMargins(mDrawing->pageMargins());
    mDrawingPropertiesWidget->setBackgroundColor(mDrawing->backgroundColor());

    mDrawingPropertiesWidget->setGrid(mDrawing->grid());
    mDrawingPropertiesWidget->setGridStyle(mDrawing->gridStyle());
    mDrawingPropertiesWidget->setGridColor(mDrawing->gridColor());
    mDrawingPropertiesWidget->setGridSpacingMajor(mDrawing->gridSpacingMajor());
    mDrawingPropertiesWidget->setGridSpacingMinor(mDrawing->gridSpacingMinor());

    setCurrentIndex(0);
}

void PropertiesWidget::setDrawingProperty(const QString& name, const QVariant& value)
{
    mDrawingPropertiesWidget->setProperty(name, value);
    setCurrentIndex(0);
}
