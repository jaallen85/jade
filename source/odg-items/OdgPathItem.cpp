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

void OdgPathItem::setPath(const QPainterPath& path, const QRectF& pathRect)
{
    mPath = path;
    mPathRect = pathRect;
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
    return (OdgRectItem::isValid() && !mPath.isEmpty() && (mPathRect.width() > 0 || mPathRect.height() > 0));
}

//======================================================================================================================

void OdgPathItem::paint(QPainter& painter)
{
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(pen());

    painter.translate(-rect().topLeft());
    painter.scale(mPathRect.width() / rect().width(), mPathRect.height() / rect().height());
    painter.translate(mPathRect.topLeft());
    painter.drawPath(mPath);
    painter.translate(-mPathRect.topLeft());
    painter.scale(rect().width() / mPathRect.width(), rect().height() / mPathRect.height());
    painter.translate(rect().topLeft());
}
