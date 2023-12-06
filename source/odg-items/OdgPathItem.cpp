// File: OdgPathItem.cpp
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

#include "OdgPathItem.h"
#include <QPainter>

OdgPathItem::OdgPathItem() : OdgRectItem(), mPath(), mPathRect()
{
    // Nothing more to do here.
}

//======================================================================================================================

OdgItem* OdgPathItem::copy() const
{
	OdgPathItem* pathItem = new OdgPathItem();
	pathItem->setPosition(mPosition);
	pathItem->setRotation(mRotation);
	pathItem->setFlipped(mFlipped);
	pathItem->setRect(mRect);
	pathItem->setBrush(mBrush);
	pathItem->setPen(mPen);
	pathItem->setPath(mPath, mPathRect);
	return pathItem;
}

//======================================================================================================================

void OdgPathItem::setRect(const QRectF& rect)
{
	OdgRectItem::setRect(rect);
	updateTransformedPath();
}

//======================================================================================================================

void OdgPathItem::setPath(const QPainterPath& path, const QRectF& pathRect)
{
	if (mPathRect.width() != 0 && mPathRect.height() != 0)
	{
		mPath = path;
		mPathRect = pathRect;
		updateTransformedPath();
	}
}

QPainterPath OdgPathItem::path() const
{
    return mPath;
}

QRectF OdgPathItem::pathRect() const
{
    return mPathRect;
}

//======================================================================================================================

QPainterPath OdgPathItem::shape() const
{
    QPainterPath shape;
    shape.addRect(boundingRect());
    return shape;
}

bool OdgPathItem::isValid() const
{
	return (OdgRectItem::isValid() && !mPath.isEmpty() && mPathRect.width() != 0 && mPathRect.height() != 0);
}

//======================================================================================================================

void OdgPathItem::paint(QPainter& painter)
{
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(pen());
	painter.drawPath(mTransformedPath);
}

//======================================================================================================================

void OdgPathItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
	double size = 8 * grid;
	if (size <= 0) size = contentRect.width() / 40;
	setRect(QRectF(mPathRect.left() * size, mPathRect.top() * size,
				   mPathRect.width() * size, mPathRect.height() * size));
}

//======================================================================================================================

void OdgPathItem::updateTransformedPath()
{
	QTransform transform;
	transform.translate(-mPathRect.left(), -mPathRect.top());
	transform.scale(mRect.width() / mPathRect.width(), mRect.height() / mPathRect.height());
	transform.translate(mRect.left(), mRect.top());

	mTransformedPath = transform.map(mPath);
}
