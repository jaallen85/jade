// DrawingItemGroup.cpp
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

#include "DrawingItemGroup.h"
#include "DrawingItemPoint.h"

DrawingItemGroup::DrawingItemGroup() : DrawingItem()
{
	for(int i = 0; i < 8; i++)
		addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::None));
}

DrawingItemGroup::DrawingItemGroup(const DrawingItemGroup& item) : DrawingItem(item)
{
	mItems = copyItems(item.mItems);
	mItemsRect = item.mItemsRect;
	mItemsShape = item.mItemsShape;
}

DrawingItemGroup::~DrawingItemGroup()
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
}

//==================================================================================================

QString DrawingItemGroup::uniqueKey() const
{
	return "group";
}

DrawingItem* DrawingItemGroup::copy() const
{
	return new DrawingItemGroup(*this);
}

//==================================================================================================

void DrawingItemGroup::setItems(const QList<DrawingItem*>& items)
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
	mItems = items;
	updateItemGeometry();
}

QList<DrawingItem*> DrawingItemGroup::items() const
{
	return mItems;
}

//==================================================================================================

void DrawingItemGroup::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("position"))
	{
		QVariant variant = properties.value("position");
		if (variant.canConvert<QPointF>()) setPosition(variant.value<QPointF>());
	}
}

QHash<QString,QVariant> DrawingItemGroup::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("position", position());

	return properties;
}

//==================================================================================================

QRectF DrawingItemGroup::boundingRect() const
{
	return mItemsRect;
}

QPainterPath DrawingItemGroup::shape() const
{
	return mItemsShape;
}

bool DrawingItemGroup::isValid() const
{
	return !mItems.isEmpty();
}

//==================================================================================================

void DrawingItemGroup::render(QPainter* painter)
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		painter->translate((*itemIter)->position());
		painter->setTransform((*itemIter)->transformInverted(), true);

		(*itemIter)->render(painter);

		painter->setTransform((*itemIter)->transform(), true);
		painter->translate(-(*itemIter)->position());
	}
}

//==================================================================================================

void DrawingItemGroup::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		writeTransformToXml(xml, "transform");

		factory.writeItemsToXml(xml, mItems);
	}
}

void DrawingItemGroup::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		readTransformFromXml(xml, "transform");

		setItems(factory.readItemsFromXml(xml));
	}
}

//==================================================================================================

void DrawingItemGroup::updateItemGeometry()
{
	// Update items rect
	QRectF itemRect;
	mItemsRect = QRectF();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		itemRect = (*itemIter)->mapToScene((*itemIter)->boundingRect()).boundingRect();

		if (!mItemsRect.isValid()) mItemsRect = itemRect;
		else mItemsRect = mItemsRect.united(itemRect);
	}

	// Update items shape
	mItemsShape = QPainterPath();

	//for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	//	mItemsShape = mItemsShape.united((*itemIter)->shape());

	mItemsShape.addRect(mItemsRect);

	// Update points
	QList<DrawingItemPoint*> points = DrawingItemGroup::points();
	if (points.size() >= 8)
	{
		points[0]->setPosition(mItemsRect.left(), mItemsRect.top());
		points[1]->setPosition(mItemsRect.center().x(), mItemsRect.top());
		points[2]->setPosition(mItemsRect.right(), mItemsRect.top());
		points[3]->setPosition(mItemsRect.right(), mItemsRect.center().y());
		points[4]->setPosition(mItemsRect.right(), mItemsRect.bottom());
		points[5]->setPosition(mItemsRect.center().x(), mItemsRect.bottom());
		points[6]->setPosition(mItemsRect.left(), mItemsRect.bottom());
		points[7]->setPosition(mItemsRect.left(), mItemsRect.center().y());
	}
}
