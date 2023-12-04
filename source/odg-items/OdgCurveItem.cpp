// File: OdgCurveItem.cpp
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

#include "OdgCurveItem.h"
#include "OdgControlPoint.h"
#include <QPainter>

OdgCurveItem::OdgCurveItem() : OdgItem(), mCurve(), mPen(), mStartMarker(), mEndMarker(), mCurvePath()
{
    addControlPoint(new OdgControlPoint(QPointF(0, 0), true));
    addControlPoint(new OdgControlPoint(QPointF(0, 0), false));
    addControlPoint(new OdgControlPoint(QPointF(0, 0), false));
    addControlPoint(new OdgControlPoint(QPointF(0, 0), true));
}

//======================================================================================================================

void OdgCurveItem::setCurve(const OdgCurve& curve)
{
    mCurve = curve;

    // Put the item's position at the center of the line
    const QPointF offset = mCurve.center();
    setPosition(mapToScene(offset));
    mCurve.translate(-offset);

    // Set control point positions to match mCurve
    if (mControlPoints.size() >= NumberOfControlPoints)
    {
        mControlPoints.at(StartControlPoint)->setPosition(mCurve.p1());
        mControlPoints.at(StartControlBezierPoint)->setPosition(mCurve.cp1());
        mControlPoints.at(EndControlBezierPoint)->setPosition(mCurve.cp2());
        mControlPoints.at(EndControlPoint)->setPosition(mCurve.p2());
    }

    // Update curve path
    mCurvePath.clear();
    mCurvePath.moveTo(mCurve.p1());
    mCurvePath.cubicTo(mCurve.cp1(), mCurve.cp2(), mCurve.p2());
}

OdgCurve OdgCurveItem::curve() const
{
    return mCurve;
}

//======================================================================================================================

void OdgCurveItem::setPen(const QPen& pen)
{
    if (pen.widthF() >= 0) mPen = pen;
}

void OdgCurveItem::setStartMarker(const OdgMarker& marker)
{
    mStartMarker = marker;
}

void OdgCurveItem::setEndMarker(const OdgMarker& marker)
{
    mEndMarker = marker;
}

QPen OdgCurveItem::pen() const
{
    return mPen;
}

OdgMarker OdgCurveItem::startMarker() const
{
    return mStartMarker;
}

OdgMarker OdgCurveItem::endMarker() const
{
    return mEndMarker;
}

//======================================================================================================================

QRectF OdgCurveItem::boundingRect() const
{
    QRectF rect = mCurvePath.boundingRect();

    // Adjust for pen width
    const double halfPenWidth = mPen.widthF() / 2;
    rect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

    return rect;
}

QPainterPath OdgCurveItem::shape() const
{
    QPainterPath shape;

    // Calculate curve shape
    shape = strokePath(mCurvePath, mPen);

    // Add shape for each marker, if necessary
    if (shouldShowMarker(mStartMarker.size()))
        shape.addPath(mStartMarker.shape(mPen, mCurve.p1(), startMarkerAngle()));
    if (shouldShowMarker(mEndMarker.size()))
        shape.addPath(mEndMarker.shape(mPen, mCurve.p2(), endMarkerAngle()));

    return shape;
}

bool OdgCurveItem::isValid() const
{
    QRectF boundingRect = mCurvePath.boundingRect();
    return (boundingRect.width() != 0 || boundingRect.height() != 0);
}

//======================================================================================================================

void OdgCurveItem::paint(QPainter& painter)
{
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(mPen);
    painter.drawPath(mCurvePath);

    if (shouldShowMarker(mStartMarker.size()))
        mStartMarker.paint(painter, mPen, mCurve.p1(), startMarkerAngle());
    if (shouldShowMarker(mEndMarker.size()))
        mEndMarker.paint(painter, mPen, mCurve.p2(), endMarkerAngle());
}

//======================================================================================================================

void OdgCurveItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);

    OdgCurve scaledCurve = mCurve;
    scaledCurve.scale(scale);
    setCurve(scaledCurve);

    mPen.setWidthF(mPen.widthF() * scale);
    mStartMarker.setSize(mStartMarker.size() * scale);
    mEndMarker.setSize(mEndMarker.size() * scale);
}

//======================================================================================================================

bool OdgCurveItem::shouldShowMarker(double size) const
{
    return (mCurve.length() >= size);
}

double OdgCurveItem::startMarkerAngle() const
{
    return mCurve.startAngle();
}

double OdgCurveItem::endMarkerAngle() const
{
    return mCurve.endAngle();
}
