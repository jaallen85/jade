// File: OdgEllipseItem.cpp
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

#include "OdgEllipseItem.h"
#include <QPainter>

OdgEllipseItem::OdgEllipseItem() : OdgRectItem()
{
    // Nothing more to do here.
}

//======================================================================================================================

OdgItem* OdgEllipseItem::copy() const
{
	OdgEllipseItem* ellipseItem = new OdgEllipseItem();
	ellipseItem->setPosition(mPosition);
	ellipseItem->setRotation(mRotation);
	ellipseItem->setFlipped(mFlipped);
	ellipseItem->setRect(mRect);
	ellipseItem->setBrush(mBrush);
	ellipseItem->setPen(mPen);
	return ellipseItem;
}

//======================================================================================================================

void OdgEllipseItem::setEllipse(const QRectF& ellipse)
{
	setRect(ellipse);
}

QRectF OdgEllipseItem::ellipse() const
{
	return rect();
}

//======================================================================================================================

void OdgEllipseItem::setProperty(const QString &name, const QVariant &value)
{
	OdgRectItem::setProperty(name, value);
}

QVariant OdgEllipseItem::property(const QString &name) const
{
	if (name == "ellipse") return mRect;
	return OdgRectItem::property(name);
}

//======================================================================================================================

QPainterPath OdgEllipseItem::shape() const
{
    QPainterPath shape;

    // Calculate rect shape
    const QRectF normalizedRect = rect().normalized();

    if (pen().style() != Qt::NoPen)
    {
        QPainterPath ellipsePath;
        ellipsePath.addEllipse(normalizedRect);

        shape = strokePath(ellipsePath, pen());
        if (brush().color().alpha() > 0)
            shape = shape.united(ellipsePath);
    }
    else
    {
        shape.addEllipse(normalizedRect);
    }

    return shape;
}

//======================================================================================================================

void OdgEllipseItem::paint(QPainter& painter)
{
    painter.setBrush(brush());
    painter.setPen(pen());
    painter.drawEllipse(rect());
}
