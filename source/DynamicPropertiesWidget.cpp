/* DynamicPropertiesWidget.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DynamicPropertiesWidget.h"
#include "ScenePropertiesWidget.h"
#include "ItemPropertiesWidget.h"

DynamicPropertiesWidget::DynamicPropertiesWidget() : QStackedWidget()
{
	mItemWidget = nullptr;
	mSceneWidget = new ScenePropertiesWidget();
	mItemDefaultsWidget = new ItemPropertiesWidget(QList<DrawingItem*>());

	QTabWidget* tabWidget = new QTabWidget();
	tabWidget->setTabPosition(QTabWidget::South);
	tabWidget->addTab(mSceneWidget, "Drawing");
	tabWidget->addTab(mItemDefaultsWidget, "Item Defaults");
	addWidget(tabWidget);

	connect(mSceneWidget, SIGNAL(propertiesUpdated(const QRectF&,qreal,const QBrush&,DrawingGridStyle,const QBrush&,int,int)),
		this, SIGNAL(drawingPropertiesUpdated(const QRectF&,qreal,const QBrush&,DrawingGridStyle,const QBrush&,int,int)));
	connect(mItemDefaultsWidget, SIGNAL(propertiesUpdated(const QMap<QString,QVariant>&)),
		this, SIGNAL(defaultItemPropertiesUpdated(const QMap<QString,QVariant>&)));

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
}

DynamicPropertiesWidget::~DynamicPropertiesWidget() { }

//==================================================================================================

QSize DynamicPropertiesWidget::sizeHint() const
{
	return QSize(280, -1);
}

//==================================================================================================

void DynamicPropertiesWidget::setFromItems(const QList<DrawingItem*>& items)
{
	if (mItemWidget)
	{
		setCurrentIndex(0);
		removeWidget(mItemWidget);
		delete mItemWidget;
		mItemWidget = nullptr;
	}

	if (items.size() > 0)
	{
		mItemWidget = new ItemPropertiesWidget(items);
		addWidget(mItemWidget);
		setCurrentIndex(1);

		connect(mItemWidget, SIGNAL(positionUpdated(const QPointF&)),
			this, SIGNAL(itemPositionUpdated(const QPointF&)));
		connect(mItemWidget, SIGNAL(pointPositionUpdated(DrawingItemPoint*,const QPointF&)),
			this, SIGNAL(itemPointPositionUpdated(DrawingItemPoint*,const QPointF&)));
		connect(mItemWidget, SIGNAL(propertiesUpdated(const QMap<QString,QVariant>&)),
			this, SIGNAL(itemPropertiesUpdated(const QMap<QString,QVariant>&)));
	}
}

void DynamicPropertiesWidget::setFromItem(DrawingItem* item)
{
	QList<DrawingItem*> items;
	if (item) items.append(item);
	setFromItems(items);
}

void DynamicPropertiesWidget::updateItemGeometry()
{
	if (mItemWidget) mItemWidget->updateGeometry();
}

//==================================================================================================

void DynamicPropertiesWidget::setDrawingProperties(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
	DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor)
{
	mSceneWidget->setProperties(sceneRect, grid, backgroundBrush,
		gridStyle, gridBrush, gridSpacingMajor, gridSpacingMinor);
}

void DynamicPropertiesWidget::setDefaultItemProperties(const QMap<QString,QVariant>& properties)
{
	mItemDefaultsWidget->updateProperties(properties);
}
