/* DrawingItemGroup.cpp
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

#include "DrawingItemGroup.h"
#include "DrawingItemPoint.h"

DrawingItemGroup::DrawingItemGroup() : DrawingItem()
{
	for(int i = 0; i < 8; i++)
		addPoint(new DrawingItemPoint(QPointF(0.0, 0.0), DrawingItemPoint::NoFlags));
}

DrawingItemGroup::DrawingItemGroup(const DrawingItemGroup& item) : DrawingItem(item)
{
	mItems = copyItems(item.mItems);
	mItemsRect = item.mItemsRect;
}

DrawingItemGroup::~DrawingItemGroup()
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
}

//==================================================================================================

DrawingItem* DrawingItemGroup::copy() const
{
	return new DrawingItemGroup(*this);
}

//==================================================================================================

void DrawingItemGroup::setItems(const QList<DrawingItem*>& items)
{
	while (!mItems.isEmpty()) delete mItems.takeFirst();
	mItems = items;
	recalculateContentsRect();
}

QList<DrawingItem*> DrawingItemGroup::items() const
{
	return mItems;
}

//==================================================================================================

QRectF DrawingItemGroup::boundingRect() const
{
	return mItemsRect;
}

QPainterPath DrawingItemGroup::shape() const
{
	//return DrawingItem::shape();

	QPainterPath shape;

	if (isValid())
	{
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			if ((*itemIter)->isVisible())
				shape = shape.united((*itemIter)->mapToScene((*itemIter)->shape()));
		}
	}

	return shape;
}

bool DrawingItemGroup::isValid() const
{
	return !mItems.isEmpty();
}

//==================================================================================================

void DrawingItemGroup::paint(QPainter* painter)
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			painter->translate((*itemIter)->pos());
			painter->setTransform((*itemIter)->transform().inverted(), true);
			(*itemIter)->paint(painter);
			painter->setTransform((*itemIter)->transform(), true);
			painter->translate(-(*itemIter)->pos());
		}
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

void DrawingItemGroup::rotateItem(const QPointF& scenePos)
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateItem(scenePos - pos());

	recalculateContentsRect();
}

void DrawingItemGroup::rotateBackItem(const QPointF& scenePos)
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->rotateBackItem(scenePos - pos());

	recalculateContentsRect();
}

void DrawingItemGroup::flipItem(const QPointF& scenePos)
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->flipItem(scenePos - pos());

	recalculateContentsRect();
}

//==================================================================================================

void DrawingItemGroup::recalculateContentsRect()
{
	// Update items rect
	QRectF itemRect;
	mItemsRect = QRectF();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			itemRect = (*itemIter)->mapToScene((*itemIter)->boundingRect());

			if (!mItemsRect.isValid()) mItemsRect = itemRect;
			else mItemsRect = mItemsRect.united(itemRect);
		}
	}

	// Update points
	QList<DrawingItemPoint*> points = DrawingItemGroup::points();
	if (points.size() >= 8)
	{
		points[0]->setPos(mItemsRect.left(), mItemsRect.top());
		points[1]->setPos(mItemsRect.center().x(), mItemsRect.top());
		points[2]->setPos(mItemsRect.right(), mItemsRect.top());
		points[3]->setPos(mItemsRect.right(), mItemsRect.center().y());
		points[4]->setPos(mItemsRect.right(), mItemsRect.bottom());
		points[5]->setPos(mItemsRect.center().x(), mItemsRect.bottom());
		points[6]->setPos(mItemsRect.left(), mItemsRect.bottom());
		points[7]->setPos(mItemsRect.left(), mItemsRect.center().y());
	}
}
