/* DynamicPropertiesWidget.h
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DYNAMICPROPERTIESWIDGET_H
#define DYNAMICPROPERTIESWIDGET_H

#include <DiagramWidget.h>

class DynamicPropertiesWidget : public QWidget
{
	Q_OBJECT

protected:


public:
	DynamicPropertiesWidget(DiagramWidget* diagram);
	virtual ~DynamicPropertiesWidget();

public slots:
	void setSelectedItems(const QList<DrawingItem*>& selectedItems);
	void setNewItem(DrawingItem* item);

	void updateGeometryFromSelectedItem();
	void updateCaptionFromSelectedItem();
	void updateCornerRadiusFromSelectedItem();
	void updateStylePropertiesFromSelectedItems();
	//void updatePropertiesFromDiagram();

signals:
	void selectedItemMoved(const QPointF& scenePos);		// connect to moveSelection
	void selectedItemResized(DrawingItemPoint* itemPoint, const QPointF& scenePos);		// connect to resizeSelection
	void selectedItemCaptionChanged(const QString& newCaption);
	void selectedItemCornerRadiusChanged(qreal radiusX, qreal radiusY);
	void selectedItemsStyleChanged(const QHash<DrawingItemStyle::Property,QVariant>& properties);
	//void diagramPropertiesChanged(const QHash<DiagramWidget::Property,QVariant>& properties);

	//todo: fill in default item style properties
	//todo: save/load default item style properties from INI file
};

#endif
