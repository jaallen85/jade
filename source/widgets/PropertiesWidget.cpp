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
#include "MultipleItemPropertiesWidget.h"
#include "SingleItemPropertiesWidget.h"
#include "DrawingWidget.h"
#include <QScrollArea>
#include <QVBoxLayout>

PropertiesWidget::PropertiesWidget(DrawingWidget* drawing) : QStackedWidget(),
    mDrawing(drawing), mDrawingPropertiesWidget(nullptr), mDrawingPropertiesScrollArea(nullptr),
    mMultipleItemPropertiesWidget(nullptr), mMultipleItemPropertiesScrollArea(nullptr),
    mSingleItemPropertiesWidget(nullptr), mSingleItemPropertiesScrollArea(nullptr)
{
    Q_ASSERT(mDrawing != nullptr);

    // Add drawing properties widget
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

    // Add multiple item properties widget
    mMultipleItemPropertiesWidget = new MultipleItemPropertiesWidget();
    mMultipleItemPropertiesWidget->setUnits(mDrawingPropertiesWidget->units());

    QWidget* multipleItemPropertiesWidget = new QWidget();
    QVBoxLayout* multipleItemPropertiesLayout = new QVBoxLayout();
    multipleItemPropertiesLayout->addWidget(mMultipleItemPropertiesWidget);
    multipleItemPropertiesLayout->addWidget(new QWidget(), 100);
    multipleItemPropertiesLayout->setContentsMargins(0, 0, 0, 0);
    multipleItemPropertiesWidget->setLayout(multipleItemPropertiesLayout);

    mMultipleItemPropertiesScrollArea = new QScrollArea();
    mMultipleItemPropertiesScrollArea->setWidget(multipleItemPropertiesWidget);
    mMultipleItemPropertiesScrollArea->setWidgetResizable(true);
    addWidget(mMultipleItemPropertiesScrollArea);

    // Add single item properties widget
    mSingleItemPropertiesWidget = new SingleItemPropertiesWidget();
    mSingleItemPropertiesWidget->setUnits(mDrawingPropertiesWidget->units());

    QWidget* singleItemPropertiesWidget = new QWidget();
    QVBoxLayout* singleItemPropertiesLayout = new QVBoxLayout();
    singleItemPropertiesLayout->addWidget(mSingleItemPropertiesWidget);
    singleItemPropertiesLayout->addWidget(new QWidget(), 100);
    singleItemPropertiesLayout->setContentsMargins(0, 0, 0, 0);
    singleItemPropertiesWidget->setLayout(singleItemPropertiesLayout);

    mSingleItemPropertiesScrollArea = new QScrollArea();
    mSingleItemPropertiesScrollArea->setWidget(singleItemPropertiesWidget);
    mSingleItemPropertiesScrollArea->setWidgetResizable(true);
    addWidget(mSingleItemPropertiesScrollArea);

    // Connect signals/slots between drawing widget and properties widgets
    connect(mDrawing, SIGNAL(propertyChanged(QString,QVariant)), this, SLOT(setDrawingProperty(QString,QVariant)));
    connect(mDrawing, SIGNAL(propertiesChanged()), this, SLOT(setAllDrawingProperties()));
    connect(mDrawing, SIGNAL(currentItemsChanged(QList<OdgItem*>)), this, SLOT(setItems(QList<OdgItem*>)));
    connect(mDrawing, SIGNAL(currentItemsGeometryChanged(QList<OdgItem*>)), this, SLOT(setItems(QList<OdgItem*>)));
    connect(mDrawing, SIGNAL(currentItemsGeometryChanged(QList<OdgItem*>)), this, SLOT(setItems(QList<OdgItem*>)));

    connect(mDrawingPropertiesWidget, SIGNAL(propertyChanged(QString,QVariant)),
            mDrawing, SLOT(setDrawingProperty(QString,QVariant)));

    connect(mMultipleItemPropertiesWidget, SIGNAL(itemsMovedDelta(QPointF)), mDrawing, SLOT(moveDelta(QPointF)));
    connect(mMultipleItemPropertiesWidget, SIGNAL(itemsPropertyChanged(QString,QVariant)),
            mDrawing, SLOT(setItemsProperty(QString,QVariant)));

    connect(mSingleItemPropertiesWidget, SIGNAL(itemMoved(QPointF)), mDrawing, SLOT(move(QPointF)));
    connect(mSingleItemPropertiesWidget, SIGNAL(itemResized(OdgControlPoint*,QPointF)), mDrawing,
            SLOT(resize(OdgControlPoint*,QPointF)));
    connect(mSingleItemPropertiesWidget, SIGNAL(itemResized2(OdgControlPoint*,QPointF,OdgControlPoint*,QPointF)),
            mDrawing, SLOT(resize2(OdgControlPoint*,QPointF,OdgControlPoint*,QPointF)));
    connect(mSingleItemPropertiesWidget, SIGNAL(itemPropertyChanged(QString,QVariant)),
            mDrawing, SLOT(setItemsProperty(QString,QVariant)));

    connect(mDrawingPropertiesWidget, SIGNAL(unitsChanged(int)), mMultipleItemPropertiesWidget, SLOT(setUnits(int)));
    connect(mDrawingPropertiesWidget, SIGNAL(unitsChanged(int)), mSingleItemPropertiesWidget, SLOT(setUnits(int)));
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

void PropertiesWidget::setItems(const QList<OdgItem*>& items)
{
    if (items.size() > 1)
    {
        mMultipleItemPropertiesWidget->setItems(items);
        setCurrentIndex(1);
    }
    else if (items.size() == 1)
    {
        mSingleItemPropertiesWidget->setItem(items.first());
        setCurrentIndex(2);
    }
    else setCurrentIndex(0);
}
