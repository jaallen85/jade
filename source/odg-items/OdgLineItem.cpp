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

OdgItem* OdgLineItem::copy() const
{
	OdgLineItem* lineItem = new OdgLineItem();
	lineItem->setPosition(mPosition);
	lineItem->setRotation(mRotation);
	lineItem->setFlipped(mFlipped);
	lineItem->setLine(mLine);
	lineItem->setPen(mPen);
	lineItem->setStartMarker(mStartMarker);
	lineItem->setEndMarker(mEndMarker);
	return lineItem;
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

void OdgLineItem::setProperty(const QString &name, const QVariant &value)
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

QVariant OdgLineItem::property(const QString &name) const
{
	if (name == "line") return mLine;
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

void OdgLineItem::resize(OdgControlPoint *point, const QPointF &position, bool snapTo45Degrees)
{
	if (point && mControlPoints.contains(point) && mControlPoints.size() >= NumberOfControlPoints)
	{
		// Determine final point position and convert to item coordinates
		QPointF pointPosition;
		if (snapTo45Degrees)
		{
			pointPosition = mapFromScene(snapResizeTo45Degrees(point, position, mControlPoints.at(StartControlPoint),
															   mControlPoints.at(EndControlPoint)));
		}
		else
			pointPosition = mapFromScene(position);

		// Move corresponding line vertex
		QLineF line = mLine;

		const int pointIndex = mControlPoints.indexOf(point);
		if (pointIndex == StartControlPoint)
			line.setP1(pointPosition);
		else if (pointIndex == EndControlPoint)
			line.setP2(pointPosition);

		setLine(line);
	}
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

void OdgLineItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
	setLine(QLineF());
}

OdgControlPoint* OdgLineItem::placeResizeStartPoint() const
{
	return (mControlPoints.size() >= NumberOfControlPoints) ? mControlPoints.at(StartControlPoint) : nullptr;
}

OdgControlPoint* OdgLineItem::placeResizeEndPoint() const
{
	return (mControlPoints.size() >= NumberOfControlPoints) ? mControlPoints.at(EndControlPoint) : nullptr;
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
