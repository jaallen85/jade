// File: OdgPolygonItem.cpp
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

#include "OdgPolygonItem.h"
#include "OdgControlPoint.h"
#include <QPainter>

OdgPolygonItem::OdgPolygonItem() : mPolygon(), mBrush(), mPen()
{
    for(int i = 0; i < 3; i++) addControlPoint(new OdgControlPoint(QPointF(0, 0)));
}

//======================================================================================================================

void OdgPolygonItem::setPolygon(const QPolygonF& polygon)
{
    if (polygon.size() >= 3)
    {
        mPolygon = polygon;

        // Put the item's position at the center of the polygon
        const QPointF offset = mPolygon.boundingRect().center();
        setPosition(mapToScene(offset));
        mPolygon.translate(-offset);

        // Ensure that the item has one control point for each polygon vertex
        const QList<OdgControlPoint*> controlPoints = this->controlPoints();
        const int numberOfPointsToAdd = mPolygon.size() - controlPoints.size();
        const int numberOfPointsToRemove = controlPoints.size() - mPolygon.size();

        for(int i = 0; i < numberOfPointsToAdd; i++)
            insertControlPoint(i + 1, new OdgControlPoint(QPointF()));

        OdgControlPoint* pointToRemove = nullptr;
        for(int i = 0; i < numberOfPointsToRemove; i++)
        {
            pointToRemove = controlPoints.at(controlPoints.size() - i - 2);
            removeControlPoint(pointToRemove);
            delete pointToRemove;
        }

        // Set point positions to match mPolygon
        const QList<OdgControlPoint*> finalControlPoints = this->controlPoints();
        const int finalControlPointCount = qMin(finalControlPoints.size(), mPolygon.size());
        for(int i = 0; i < finalControlPointCount; i++)
            finalControlPoints.at(i)->setPosition(mPolygon.at(i));
    }
}

QPolygonF OdgPolygonItem::polygon() const
{
    return mPolygon;
}

//======================================================================================================================

void OdgPolygonItem::setBrush(const QBrush& brush)
{
    mBrush = brush;
}

void OdgPolygonItem::setPen(const QPen& pen)
{
    if (pen.widthF() >= 0) mPen = pen;
}

QBrush OdgPolygonItem::brush() const
{
    return mBrush;
}

QPen OdgPolygonItem::pen() const
{
    return mPen;
}

//======================================================================================================================

QRectF OdgPolygonItem::boundingRect() const
{
    QRectF rect = mPolygon.boundingRect();

    // Adjust for pen width
    const double halfPenWidth = mPen.widthF() / 2;
    rect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

    return rect;
}

QPainterPath OdgPolygonItem::shape() const
{
    QPainterPath shape;

    // Calculate polygon shape
    if (mPen.style() != Qt::NoPen)
    {
        QPainterPath polygonPath;
        polygonPath.addPolygon(mPolygon);
        polygonPath.closeSubpath();

        shape = strokePath(polygonPath, mPen);
        if (mBrush.color().alpha() > 0)
            shape = shape.united(polygonPath);
    }
    else
    {
        shape.addPolygon(mPolygon);
        shape.closeSubpath();
    }

    return shape;
}

bool OdgPolygonItem::isValid() const
{
    QRectF boundingRect = mPolygon.boundingRect();
    return (boundingRect.width() != 0 || boundingRect.height() != 0);
}

//======================================================================================================================

void OdgPolygonItem::paint(QPainter& painter)
{
    painter.setBrush(mBrush);
    painter.setPen(mPen);
    painter.drawPolygon(mPolygon);
}

//======================================================================================================================

void OdgPolygonItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);

    QPolygonF scaledPolygon;
    for(auto& point : qAsConst(mPolygon))
        scaledPolygon.append(QPointF(point.x() * scale, point.y() * scale));
    setPolygon(scaledPolygon);

    mPen.setWidthF(mPen.widthF() * scale);
}
