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
    for(int i = 0; i < NumberOfGluePoints; i++) addGluePoint(new OdgGluePoint(QPointF(0, 0)));
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
        if (mControlPoints.size() >= NumberOfControlPoints)
        {
            QPointF center = mRect.center();
            mControlPoints.at(TopLeftControlPoint)->setPosition(QPointF(mRect.left(), mRect.top()));
            mControlPoints.at(TopMiddleControlPoint)->setPosition(QPointF(center.x(), mRect.top()));
            mControlPoints.at(TopRightControlPoint)->setPosition(QPointF(mRect.right(), mRect.top()));
            mControlPoints.at(MiddleRightControlPoint)->setPosition(QPointF(mRect.right(), center.y()));
            mControlPoints.at(BottomRightControlPoint)->setPosition(QPointF(mRect.right(), mRect.bottom()));
            mControlPoints.at(BottomMiddleControlPoint)->setPosition(QPointF(center.x(), mRect.bottom()));
            mControlPoints.at(BottomLeftControlPoint)->setPosition(QPointF(mRect.left(), mRect.bottom()));
            mControlPoints.at(MiddleLeftControlPoint)->setPosition(QPointF(mRect.left(), center.y()));
        }

        // Set glue point positions to match mRect
        if (mGluePoints.size() >= NumberOfGluePoints)
        {
            QPointF center = mRect.center();
            mGluePoints.at(TopGluePoint)->setPosition(QPointF(center.x(), mRect.top()));
            mGluePoints.at(RightGluePoint)->setPosition(QPointF(mRect.right(), center.y()));
            mGluePoints.at(BottomGluePoint)->setPosition(QPointF(center.x(), mRect.bottom()));
            mGluePoints.at(LeftGluePoint)->setPosition(QPointF(mRect.left(), center.y()));
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

QRectF OdgRectItem::boundingRect() const
{
    QRectF rect = mRect.normalized();

    // Adjust for pen width
    const double halfPenWidth = mPen.widthF() / 2;
    rect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

    return rect;
}

QPainterPath OdgRectItem::shape() const
{
    QPainterPath shape;

    // Calculate rect shape
    const QRectF normalizedRect = mRect.normalized();

    if (mPen.style() != Qt::NoPen)
    {
        QPainterPath rectPath;
        rectPath.addRect(normalizedRect);

        shape = strokePath(rectPath, mPen);
        if (mBrush.color().alpha() > 0)
            shape = shape.united(rectPath);
    }
    else
    {
        shape.addRect(normalizedRect);
    }

    return shape;
}

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
