// File: OdgLineItem.cpp
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

#include "OdgLineItem.h"
#include "OdgControlPoint.h"
#include <QPainter>

OdgLineItem::OdgLineItem() : OdgItem(), mLine(), mPen(), mStartMarker(), mEndMarker()
{
    for(int i = 0; i < NumberOfControlPoints; i++) addControlPoint(new OdgControlPoint(QPointF(0, 0), true));
}

//======================================================================================================================

void OdgLineItem::setLine(const QLineF& line)
{
    mLine = line;

    // Put the item's position at the center of the line
    const QPointF offset = mLine.center();
    setPosition(mapToScene(offset));
    mLine.translate(-offset);

    // Set control point positions to match mLine
    if (mControlPoints.size() >= NumberOfControlPoints)
    {
        mControlPoints.at(StartControlPoint)->setPosition(mLine.p1());
        mControlPoints.at(EndControlPoint)->setPosition(mLine.p2());
    }
}

QLineF OdgLineItem::line() const
{
    return mLine;
}

//======================================================================================================================

void OdgLineItem::setPen(const QPen& pen)
{
    if (pen.widthF() >= 0) mPen = pen;
}

void OdgLineItem::setStartMarker(const OdgMarker& marker)
{
    mStartMarker = marker;
}

void OdgLineItem::setEndMarker(const OdgMarker& marker)
{
    mEndMarker = marker;
}

QPen OdgLineItem::pen() const
{
    return mPen;
}

OdgMarker OdgLineItem::startMarker() const
{
    return mStartMarker;
}

OdgMarker OdgLineItem::endMarker() const
{
    return mEndMarker;
}

//======================================================================================================================

QRectF OdgLineItem::boundingRect() const
{
    QRectF rect = QRectF(mLine.p1(), mLine.p2()).normalized();

    // Adjust for pen width
    const double halfPenWidth = mPen.widthF() / 2;
    rect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

    return rect;
}

QPainterPath OdgLineItem::shape() const
{
    QPainterPath shape;

    // Calculate line shape
    QPainterPath linePath;
    linePath.moveTo(mLine.p1());
    linePath.lineTo(mLine.p2());
    shape = strokePath(linePath, mPen);

    // Add shape for each marker, if necessary
    if (shouldShowMarker(mStartMarker.size()))
        shape.addPath(mStartMarker.shape(mPen, mLine.p1(), startMarkerAngle()));
    if (shouldShowMarker(mEndMarker.size()))
        shape.addPath(mEndMarker.shape(mPen, mLine.p2(), endMarkerAngle()));

    return shape;
}

bool OdgLineItem::isValid() const
{
    return (mLine.x1() != mLine.x2() || mLine.y1() != mLine.y2());
}

//======================================================================================================================

void OdgLineItem::paint(QPainter& painter)
{
    painter.setPen(mPen);
    painter.drawLine(mLine);

    if (shouldShowMarker(mStartMarker.size()))
        mStartMarker.paint(painter, mPen, mLine.p1(), startMarkerAngle());
    if (shouldShowMarker(mEndMarker.size()))
        mEndMarker.paint(painter, mPen, mLine.p2(), endMarkerAngle());
}

//======================================================================================================================

void OdgLineItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);

    setLine(QLineF(mLine.x1() * scale, mLine.y1() * scale, mLine.x2() * scale, mLine.y2() * scale));

    mPen.setWidthF(mPen.widthF() * scale);
    mStartMarker.setSize(mStartMarker.size() * scale);
    mEndMarker.setSize(mEndMarker.size() * scale);
}

//======================================================================================================================

bool OdgLineItem::shouldShowMarker(double size) const
{
    return (mLine.length() >= size);
}

double OdgLineItem::startMarkerAngle() const
{
    return qRadiansToDegrees(qAtan2(mLine.y1() - mLine.y2(), mLine.x1() - mLine.x2()));
}

double OdgLineItem::endMarkerAngle() const
{
    return qRadiansToDegrees(qAtan2(mLine.y2() - mLine.y1(), mLine.x2() - mLine.x1()));
}
