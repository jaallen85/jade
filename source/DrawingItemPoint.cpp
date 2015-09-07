/* DrawingItemPoint.cpp
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

#include "DrawingItemPoint.h"
#include "DrawingItem.h"
#include "DrawingWidget.h"

DrawingItemPoint::DrawingItemPoint(const QPointF& position, Flags flags)
{
	mItem = nullptr;
	mPosition = position;
	mSize = QSize(8, 8);
	mFlags = flags;
	mCategory = 0;
}

DrawingItemPoint::DrawingItemPoint(const DrawingItemPoint& point)
{
	mItem = nullptr;
	mPosition = point.mPosition;
	mSize = point.mSize;
	mFlags = point.mFlags;
	mCategory = point.mCategory;
}

DrawingItemPoint::~DrawingItemPoint()
{
	clearConnections();
}

//==================================================================================================

DrawingItemPoint* DrawingItemPoint::copy() const
{
	return new DrawingItemPoint(*this);
}

//==================================================================================================

DrawingItem* DrawingItemPoint::item() const
{
	return mItem;
}

//==================================================================================================

void DrawingItemPoint::setPos(const QPointF& pos)
{
	mPosition = pos;
}

void DrawingItemPoint::setPos(qreal x, qreal y)
{
	mPosition.setX(x);
	mPosition.setY(y);
}

void DrawingItemPoint::setX(qreal x)
{
	mPosition.setX(x);
}

void DrawingItemPoint::setY(qreal y)
{
	mPosition.setY(y);
}

QPointF DrawingItemPoint::pos() const
{
	return mPosition;
}

qreal DrawingItemPoint::x() const
{
	return mPosition.x();
}

qreal DrawingItemPoint::y() const
{
	return mPosition.y();
}

//==================================================================================================

void DrawingItemPoint::setSize(const QSize& size)
{
	mSize = size;
}

void DrawingItemPoint::setSize(int width, int height)
{
	mSize.setWidth(width);
	mSize.setHeight(height);
}

void DrawingItemPoint::setWidth(int width)
{
	mSize.setWidth(width);
}

void DrawingItemPoint::setHeight(int height)
{
	mSize.setHeight(height);
}

QSize DrawingItemPoint::size() const
{
	return mSize;
}

int DrawingItemPoint::width() const
{
	return mSize.width();
}

int DrawingItemPoint::height() const
{
	return mSize.height();
}

//==================================================================================================

void DrawingItemPoint::setFlags(Flags flags)
{
	mFlags = flags;
}

DrawingItemPoint::Flags DrawingItemPoint::flags() const
{
	return mFlags;
}

bool DrawingItemPoint::isControlPoint() const
{
	return (mFlags & Control);
}

bool DrawingItemPoint::isConnectionPoint() const
{
	return (mFlags & Connection);
}

bool DrawingItemPoint::isFree() const
{
	return (mFlags & Free);
}

//==================================================================================================

void DrawingItemPoint::setCategory(int category)
{
	mCategory = category;
}

int DrawingItemPoint::category() const
{
	return mCategory;
}

//==================================================================================================

void DrawingItemPoint::addConnection(DrawingItemPoint* point)
{
	if (point && !isConnected(point)) mConnections.append(point);
}

void DrawingItemPoint::removeConnection(DrawingItemPoint* point)
{
	if (point) mConnections.removeAll(point);
}

void DrawingItemPoint::clearConnections()
{
	DrawingItemPoint* point;

	while (!mConnections.isEmpty())
	{
		point = mConnections.first();

		removeConnection(point);
		point->removeConnection(this);
	}
}

QList<DrawingItemPoint*> DrawingItemPoint::connections() const
{
	return mConnections;
}

//==================================================================================================

bool DrawingItemPoint::isConnected(DrawingItemPoint* point) const
{
	return (point) ? mConnections.contains(point) : false;
}

bool DrawingItemPoint::isConnected(DrawingItem* item) const
{
	bool connected = false;

	if (item)
	{
		for(auto pointIter = mConnections.begin(); !connected && pointIter != mConnections.end(); pointIter++)
			connected = ((*pointIter)->item() == item);
	}

	return connected;
}

//==================================================================================================

QRect DrawingItemPoint::screenRect() const
{
	QRect viewRect;

	DrawingWidget* drawing = (mItem) ? mItem->drawing() : nullptr;

	if (drawing)
	{
		QPoint centerPoint = drawing->mapFromScene(mItem->mapToScene(mPosition));
		int hDelta = mSize.width() / 2 * drawing->devicePixelRatio();
		int vDelta = mSize.height() / 2 * drawing->devicePixelRatio();
		QPoint deltaPoint(hDelta, vDelta);

		viewRect = QRect(centerPoint - deltaPoint, centerPoint + deltaPoint);
	}

	return viewRect;
}

QRectF DrawingItemPoint::sceneRect() const
{
	QRectF sceneRect;

	DrawingWidget* drawing = (mItem) ? mItem->drawing() : nullptr;

	if (drawing) sceneRect = drawing->mapToScene(screenRect());

	return sceneRect;
}

QRectF DrawingItemPoint::itemRect() const
{
	return (mItem) ? mItem->mapFromScene(sceneRect()) : QRectF();
}

//==================================================================================================

bool DrawingItemPoint::shouldConnect(DrawingItemPoint* otherPoint) const
{
	// Assume both points are members of different items in the same scene
	bool shouldConnect = false;

	if (mItem && otherPoint && otherPoint->item())
	{
		DrawingWidget* drawing = (mItem) ? mItem->drawing() : nullptr;
		qreal threshold = (drawing) ? (drawing->grid() / 4000) : 1E-9;
		QPointF vec = mItem->mapToScene(pos()) - otherPoint->item()->mapToScene(otherPoint->pos());
		qreal distance = qSqrt(vec.x() * vec.x() + vec.y() * vec.y());

		shouldConnect = (isConnectionPoint() && otherPoint->isConnectionPoint() &&
			(isFree() || otherPoint->isFree()) && mCategory == otherPoint->category() &&
			!isConnected(otherPoint) && !isConnected(otherPoint->item()) && distance <= threshold);
	}

	return shouldConnect;
}

bool DrawingItemPoint::shouldDisconnect(DrawingItemPoint* otherPoint) const
{
	bool shouldDisconnect = true;

	if (mItem && otherPoint && otherPoint->item())
	{
		shouldDisconnect = (
			mItem->mapToScene(pos()) != otherPoint->item()->mapToScene(otherPoint->pos()) &&
			!otherPoint->isControlPoint());
	}

	return shouldDisconnect;
}
