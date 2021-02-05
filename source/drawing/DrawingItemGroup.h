// DrawingItemGroup.h
// Copyright (C) 2020  Jason Allen
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

#ifndef DRAWINGITEMGROUP_H
#define DRAWINGITEMGROUP_H

#include <DrawingItem.h>

class DrawingItemGroup : public DrawingItem
{
private:
	QList<DrawingItem*> mItems;
	QRectF mItemsRect;
	QPainterPath mItemsShape;

public:
	DrawingItemGroup();
	DrawingItemGroup(const DrawingItemGroup& item);
	virtual ~DrawingItemGroup();

	virtual QString uniqueKey() const;
	virtual DrawingItem* copy() const;

	void setItems(const QList<DrawingItem*>& items);
	QList<DrawingItem*> items() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties);
	virtual QHash<QString,QVariant> properties() const;

	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void render(QPainter* painter);

	virtual void writeToXml(QXmlStreamWriter* xml);
	virtual void readFromXml(QXmlStreamReader* xml);

private:
	void updateItemGeometry();
};

#endif
