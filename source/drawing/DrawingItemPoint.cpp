// DrawingItemPoint.cpp
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

#include "DrawingItemPoint.h"

DrawingItemPoint::DrawingItemPoint(const QPointF& position, Type type)
{
	mItem = nullptr;
	mPosition = position;
	mType = type;
}

DrawingItemPoint::DrawingItemPoint(const DrawingItemPoint& point)
{
	mItem = nullptr;
	mPosition = point.mPosition;
	mType = point.mType;
}

DrawingItemPoint::~DrawingItemPoint()
{
	clearConnections();
}

//==================================================================================================

DrawingItem* DrawingItemPoint::item() const
{
	return mItem;
}

//==================================================================================================

void DrawingItemPoint::setPosition(const QPointF& position)
{
	mPosition = position;
}

void DrawingItemPoint::setPosition(qreal x, qreal y)
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

QPointF DrawingItemPoint::position() const
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

void DrawingItemPoint::setType(Type type)
{
	mType = type;
}

DrawingItemPoint::Type DrawingItemPoint::type() const
{
	return mType;
}

bool DrawingItemPoint::isControlPoint() const
{
	return (mType == Control || mType == ControlAndConnection ||
		mType == FreeControlAndConnection);
}

bool DrawingItemPoint::isConnectionPoint() const
{
	return (mType == Connection || mType == ControlAndConnection ||
		mType == FreeControlAndConnection);
}

bool DrawingItemPoint::isFree() const
{
	return (mType == FreeControlAndConnection);
}

//==================================================================================================

void DrawingItemPoint::addConnection(DrawingItemPoint* point)
{
	if (point) mConnections.append(point);
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
