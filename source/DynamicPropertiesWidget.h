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

#ifndef DYNAMICPROPERTIESWIDGET_H
#define DYNAMICPROPERTIESWIDGET_H

#include <DrawingTypes.h>

class ScenePropertiesWidget;
class ItemPropertiesWidget;

class DynamicPropertiesWidget : public QStackedWidget
{
	Q_OBJECT

private:
	QScrollArea* mScrollArea;

	ScenePropertiesWidget* mSceneWidget;
	ItemPropertiesWidget* mItemDefaultsWidget;

	ItemPropertiesWidget* mItemWidget;

public:
	DynamicPropertiesWidget();
	~DynamicPropertiesWidget();

	QSize sizeHint() const;

public slots:
	// Show/hide widgets as needed
	void setFromItems(const QList<DrawingItem*>& items = QList<DrawingItem*>());
	void setFromItem(DrawingItem* item = nullptr);

	// Just update widget values
	void updateItemGeometry();

	// Update scene properties
	void setDrawingProperties(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);

signals:
	void itemPositionUpdated(const QPointF& pos);
	void itemPointPositionUpdated(DrawingItemPoint* itemPoint, const QPointF& pos);
	void itemPropertiesUpdated(const QMap<QString,QVariant>& properties);
	void drawingPropertiesUpdated(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);
	void defaultItemPropertiesUpdated(const QMap<QString,QVariant>& properties);
};

#endif
