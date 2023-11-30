// File: OdgRectItem.cpp
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

#include "OdgRectItem.h"
#include "OdgControlPoint.h"
#include "OdgGluePoint.h"
#include <QPainter>

OdgRectItem::OdgRectItem() : mRect(), mBrush(), mPen()
{
    for(int i = 0; i < NumberOfControlPoints; i++) addControlPoint(new OdgControlPoint(QPointF(0, 0)));

    addGluePoint(new OdgGluePoint(QPointF(0, 0), Odg::EscapeUp));
    addGluePoint(new OdgGluePoint(QPointF(0, 0), Odg::EscapeRight));
    addGluePoint(new OdgGluePoint(QPointF(0, 0), Odg::EscapeDown));
    addGluePoint(new OdgGluePoint(QPointF(0, 0), Odg::EscapeLeft));
}

//======================================================================================================================

void OdgRectItem::setRect(const QRectF& rect)
{
    if (rect.width() >= 0 && rect.height() >= 0)
    {
        mRect = rect;

        // Put the item's position at the center of the rect
        const QPointF offset = mRect.center();
        setPosition(mapToScene(offset));
        mRect.translate(-offset);

        // Set control point positions to match mRect
        const QList<OdgControlPoint*> controlPoints = this->controlPoints();
        if (controlPoints.size() >= NumberOfControlPoints)
        {
            QPointF center = mRect.center();
            controlPoints[TopLeftControlPoint]->setPosition(QPointF(mRect.left(), mRect.top()));
            controlPoints[TopMiddleControlPoint]->setPosition(QPointF(center.x(), mRect.top()));
            controlPoints[TopRightControlPoint]->setPosition(QPointF(mRect.right(), mRect.top()));
            controlPoints[MiddleRightControlPoint]->setPosition(QPointF(mRect.right(), center.y()));
            controlPoints[BottomRightControlPoint]->setPosition(QPointF(mRect.right(), mRect.bottom()));
            controlPoints[BottomMiddleControlPoint]->setPosition(QPointF(center.x(), mRect.bottom()));
            controlPoints[BottomLeftControlPoint]->setPosition(QPointF(mRect.left(), mRect.bottom()));
            controlPoints[MiddleLeftControlPoint]->setPosition(QPointF(mRect.left(), center.y()));
        }

        // Set glue point positions to match mRect
        const QList<OdgGluePoint*> gluePoints = this->gluePoints();
        if (gluePoints.size() >= NumberOfGluePoints)
        {
            QPointF center = mRect.center();
            gluePoints[TopGluePoint]->setPosition(QPointF(center.x(), mRect.top()));
            gluePoints[RightGluePoint]->setPosition(QPointF(mRect.right(), center.y()));
            gluePoints[BottomGluePoint]->setPosition(QPointF(center.x(), mRect.bottom()));
            gluePoints[LeftGluePoint]->setPosition(QPointF(mRect.left(), center.y()));
        }
    }
}

QRectF OdgRectItem::rect() const
{
    return mRect;
}

//======================================================================================================================

void OdgRectItem::setBrush(const QBrush& brush)
{
    mBrush = brush;
}

void OdgRectItem::setPen(const QPen& pen)
{
    if (pen.widthF() >= 0) mPen = pen;
}

QBrush OdgRectItem::brush() const
{
    return mBrush;
}

QPen OdgRectItem::pen() const
{
    return mPen;
}

//======================================================================================================================

bool OdgRectItem::isValid() const
{
    return (mRect.width() > 0 || mRect.height() > 0);
}

//======================================================================================================================

void OdgRectItem::paint(QPainter& painter)
{
    painter.setBrush(mBrush);
    painter.setPen(mPen);
    painter.drawRect(mRect);
}

//======================================================================================================================

void OdgRectItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);
    setRect(QRectF(mRect.left() * scale, mRect.top() * scale, mRect.width() * scale, mRect.height() * scale));
    mPen.setWidthF(mPen.widthF() * scale);
}
