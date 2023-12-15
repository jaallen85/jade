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
    addControlPoint(new OdgControlPoint(QPointF(0, 0)));
    addControlPoint(new OdgControlPoint(QPointF(0, 0)));
    addControlPoint(new OdgControlPoint(QPointF(0, 0)));
    addControlPoint(new OdgControlPoint(QPointF(0, 0)));
}

//======================================================================================================================

OdgItem* OdgCurveItem::copy() const
{
	OdgCurveItem* curveItem = new OdgCurveItem();
	curveItem->setPosition(mPosition);
	curveItem->setRotation(mRotation);
	curveItem->setFlipped(mFlipped);
	curveItem->setCurve(mCurve);
	curveItem->setPen(mPen);
	curveItem->setStartMarker(mStartMarker);
	curveItem->setEndMarker(mEndMarker);
	return curveItem;
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

void OdgCurveItem::setProperty(const QString &name, const QVariant &value)
{
	if (name == "pen" && value.canConvert<QPen>())
	{
		setPen(value.value<QPen>());
	}
	else if (name == "penStyle" && value.canConvert<int>())
	{
		QPen pen = mPen;
		pen.setStyle(static_cast<Qt::PenStyle>(value.toInt()));
		setPen(pen);
	}
	else if (name == "penWidth" && value.canConvert<double>())
	{
		QPen pen = mPen;
		pen.setWidthF(value.toDouble());
		setPen(pen);
	}
	else if (name == "penColor" && value.canConvert<QColor>())
	{
		QPen pen = mPen;
		pen.setBrush(QBrush(value.value<QColor>()));
		setPen(pen);
	}
	else if (name == "startMarker" && value.canConvert<OdgMarker>())
	{
		setStartMarker(value.value<OdgMarker>());
	}
	else if (name == "startMarkerStyle" && value.canConvert<int>())
	{
		OdgMarker marker = mStartMarker;
		marker.setStyle(static_cast<Odg::MarkerStyle>(value.toInt()));
		setStartMarker(marker);
	}
	else if (name == "startMarkerSize" && value.canConvert<double>())
	{
		OdgMarker marker = mStartMarker;
		marker.setSize(value.toDouble());
		setStartMarker(marker);
	}
	else if (name == "endMarker" && value.canConvert<OdgMarker>())
	{
		setEndMarker(value.value<OdgMarker>());
	}
	else if (name == "endMarkerStyle" && value.canConvert<int>())
	{
		OdgMarker marker = mEndMarker;
		marker.setStyle(static_cast<Odg::MarkerStyle>(value.toInt()));
		setEndMarker(marker);
	}
	else if (name == "endMarkerSize" && value.canConvert<double>())
	{
		OdgMarker marker = mEndMarker;
		marker.setSize(value.toDouble());
		setEndMarker(marker);
	}
}

QVariant OdgCurveItem::property(const QString &name) const
{
	if (name == "curve") return QVariant::fromValue<OdgCurve>(mCurve);
	if (name == "pen") return mPen;
	if (name == "penStyle") return static_cast<int>(mPen.style());
	if (name == "penWidth") return mPen.widthF();
	if (name == "penColor") return mPen.brush().color();
	if (name == "startMarker") return QVariant::fromValue<OdgMarker>(mStartMarker);
	if (name == "startMarkerStyle") return static_cast<int>(mStartMarker.style());
	if (name == "startMarkerSize") return mStartMarker.size();
	if (name == "endMarker") return QVariant::fromValue<OdgMarker>(mEndMarker);
	if (name == "endMarkerStyle") return static_cast<int>(mEndMarker.style());
	if (name == "endMarkerSize") return mEndMarker.size();
	return QVariant();
}

//======================================================================================================================

QRectF OdgCurveItem::boundingRect() const
{
    QRectF rect = mCurvePath.boundingRect().normalized();

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

void OdgCurveItem::resize(OdgControlPoint *point, const QPointF &position, bool snapTo45Degrees)
{
	if (point && mControlPoints.contains(point) && mControlPoints.size() >= NumberOfControlPoints)
	{
		// Determine final point position and convert to item coordinates
		QPointF pointPosition = mapFromScene(position);

		// Move corresponding curve vertex (and possibly its control point as well)
		OdgCurve curve = mCurve;
		const QPointF originalStartControlOffset = mCurve.cp1() - mCurve.p1();
		const QPointF originalEndControlOffset = mCurve.cp2() - mCurve.p2();

		const int pointIndex = mControlPoints.indexOf(point);
		if (pointIndex == StartControlPoint)
		{
			curve.setP1(pointPosition);
			curve.setCP1(pointPosition + originalStartControlOffset);
		}
		else if (pointIndex == StartControlBezierPoint)
			curve.setCP1(pointPosition);
		else if (pointIndex == EndControlBezierPoint)
			curve.setCP2(pointPosition);
		else if (pointIndex == EndControlPoint)
		{
			curve.setP2(pointPosition);
            curve.setCP2(pointPosition + originalEndControlOffset);
		}

		setCurve(curve);
	}
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

void OdgCurveItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
    double size = 4 * grid;
    if (size <= 0) size = contentRect.width() / 80;
	setCurve(OdgCurve(QPointF(-size, -size), QPointF(0, -size), QPointF(0, size), QPointF(size, size)));
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
