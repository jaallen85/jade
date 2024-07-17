// File: OdgRoundedRectItem.cpp
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

#include "OdgRoundedRectItem.h"
#include <QPainter>

OdgRoundedRectItem::OdgRoundedRectItem() : OdgRectItem(), mCornerRadius(0)
{
    // Nothing more to do here.
}

//======================================================================================================================

OdgItem* OdgRoundedRectItem::copy() const
{
	OdgRoundedRectItem* rectItem = new OdgRoundedRectItem();
	rectItem->setPosition(mPosition);
	rectItem->setRotation(mRotation);
	rectItem->setFlipped(mFlipped);
	rectItem->setRect(mRect);
	rectItem->setCornerRadius(mCornerRadius);
	rectItem->setBrush(mBrush);
	rectItem->setPen(mPen);
	return rectItem;
}

//======================================================================================================================

void OdgRoundedRectItem::setCornerRadius(double radius)
{
    if (radius >= 0) mCornerRadius = radius;
}

double OdgRoundedRectItem::cornerRadius() const
{
    return mCornerRadius;
}

//======================================================================================================================

void OdgRoundedRectItem::setProperty(const QString &name, const QVariant &value)
{
	if (name == "cornerRadius" && value.canConvert<double>())
		setCornerRadius(value.toDouble());
	else
		OdgRectItem::setProperty(name, value);
}

QVariant OdgRoundedRectItem::property(const QString &name) const
{
	if (name == "cornerRadius") return mCornerRadius;
	return OdgRectItem::property(name);
}

//======================================================================================================================

QPainterPath OdgRoundedRectItem::shape() const
{
    QPainterPath shape;

    // Calculate rect shape
    const QRectF normalizedRect = rect().normalized();

    if (pen().style() != Qt::NoPen)
    {
        QPainterPath rectPath;
        rectPath.addRoundedRect(normalizedRect, mCornerRadius, mCornerRadius);

        shape = strokePath(rectPath, pen());
        if (brush().color().alpha() > 0)
            shape = shape.united(rectPath);
    }
    else
    {
        shape.addRoundedRect(normalizedRect, mCornerRadius, mCornerRadius);
    }

    return shape;
}

//======================================================================================================================

void OdgRoundedRectItem::paint(QPainter& painter)
{
    painter.setBrush(brush());
    painter.setPen(pen());
    painter.drawRoundedRect(rect(), mCornerRadius, mCornerRadius);
}

//======================================================================================================================

void OdgRoundedRectItem::scaleBy(double scale)
{
    OdgRectItem::scaleBy(scale);

    mCornerRadius *= scale;
}
