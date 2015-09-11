/* PropertiesWidget.cpp
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

#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <DrawingTypes.h>

class ItemPropertiesWidget;

class PropertiesWidget : public QStackedWidget
{
	Q_OBJECT

private:
	QScrollArea* mScrollArea;
	ItemPropertiesWidget* mItemWidget;
	// ScenePropertiesWidget
	// ItemDefaultPropertiesWidget

public:
	PropertiesWidget();
	~PropertiesWidget();

	QSize sizeHint() const;

public slots:
	// Show/hide widgets as needed
	void setFromItems(const QList<DrawingItem*>& items);
	void setFromItem(DrawingItem* item);

	// Just update widget values
	void updateItemGeometry();

signals:
	void itemPositionUpdated(const QPointF& pos);
	void itemPointPositionUpdated(DrawingItemPoint* itemPoint, const QPointF& pos);
	void itemPropertiesUpdated(const QMap<QString,QVariant>& properties);
	void drawingPropertiesUpdated(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);
};

#endif
