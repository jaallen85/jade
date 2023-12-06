// File: OdgPolylineItem.cpp
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

#include "OdgPolylineItem.h"
#include "OdgControlPoint.h"
#include <QPainter>

OdgPolylineItem::OdgPolylineItem() : OdgItem(), mPolyline(), mPen(), mStartMarker(), mEndMarker()
{
    for(int i = 0; i < 2; i++) addControlPoint(new OdgControlPoint(QPointF(0, 0), true));
}

//======================================================================================================================

OdgItem* OdgPolylineItem::copy() const
{
	OdgPolylineItem* polylineItem = new OdgPolylineItem();
	polylineItem->setPosition(mPosition);
	polylineItem->setRotation(mRotation);
	polylineItem->setFlipped(mFlipped);
	polylineItem->setPolyline(mPolyline);
	polylineItem->setPen(mPen);
	polylineItem->setStartMarker(mStartMarker);
	polylineItem->setEndMarker(mEndMarker);
	return polylineItem;
}

//======================================================================================================================

void OdgPolylineItem::setPolyline(const QPolygonF& polyline)
{
    if (polyline.size() >= 2)
    {
        mPolyline = polyline;

        // Put the item's position at the center of the polygon
        const QPointF offset = mPolyline.boundingRect().center();
        setPosition(mapToScene(offset));
        mPolyline.translate(-offset);

        // Ensure that the item has one control point for each polyline vertex
        const int numberOfPointsToAdd = mPolyline.size() - mControlPoints.size();
        const int numberOfPointsToRemove = mControlPoints.size() - mPolyline.size();

        for(int i = 0; i < numberOfPointsToAdd; i++)
            insertControlPoint(i + 1, new OdgControlPoint(QPointF()));

        OdgControlPoint* pointToRemove = nullptr;
        for(int i = 0; i < numberOfPointsToRemove; i++)
        {
            pointToRemove = mControlPoints.at(mControlPoints.size() - i - 2);
            removeControlPoint(pointToRemove);
            delete pointToRemove;
        }

        // Set point positions to match mPolygon
        const int finalControlPointCount = qMin(mControlPoints.size(), mPolyline.size());
        for(int i = 0; i < finalControlPointCount; i++)
            mControlPoints.at(i)->setPosition(mPolyline.at(i));
    }
}

QPolygonF OdgPolylineItem::polyline() const
{
    return mPolyline;
}

//======================================================================================================================

void OdgPolylineItem::setPen(const QPen& pen)
{
    if (pen.widthF() >= 0) mPen = pen;
}

void OdgPolylineItem::setStartMarker(const OdgMarker& marker)
{
    mStartMarker = marker;
}

void OdgPolylineItem::setEndMarker(const OdgMarker& marker)
{
    mEndMarker = marker;
}

QPen OdgPolylineItem::pen() const
{
    return mPen;
}

OdgMarker OdgPolylineItem::startMarker() const
{
    return mStartMarker;
}

OdgMarker OdgPolylineItem::endMarker() const
{
    return mEndMarker;
}

//======================================================================================================================

void OdgPolylineItem::setProperty(const QString &name, const QVariant &value)
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

QVariant OdgPolylineItem::property(const QString &name) const
{
	if (name == "polyline") return mPolyline;
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

QRectF OdgPolylineItem::boundingRect() const
{
    QRectF rect = mPolyline.boundingRect();

    // Adjust for pen width
    const double halfPenWidth = mPen.widthF() / 2;
    rect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

    return rect;
}

QPainterPath OdgPolylineItem::shape() const
{
    QPainterPath shape;

    // Calculate polygon shape
    if (mPen.style() != Qt::NoPen)
    {
        // Calculate polyline shape
        QPainterPath polygonPath;
        polygonPath.moveTo(mPolyline.at(0));
        for(int i = 1; i < mPolyline.size(); i++)
            polygonPath.moveTo(mPolyline.at(i));
        shape = strokePath(polygonPath, mPen);

        // Add shape for each marker, if necessary
        if (shouldShowStartMarker())
            shape.addPath(mStartMarker.shape(mPen, mPolyline.first(), startMarkerAngle()));
        if (shouldShowEndMarker())
            shape.addPath(mEndMarker.shape(mPen, mPolyline.last(), endMarkerAngle()));
    }
    else
    {
        shape.addPolygon(mPolyline);
    }

    return shape;
}

bool OdgPolylineItem::isValid() const
{
    QRectF boundingRect = mPolyline.boundingRect();
    return (boundingRect.width() != 0 || boundingRect.height() != 0);
}

//======================================================================================================================

void OdgPolylineItem::paint(QPainter& painter)
{
    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(mPen);
    painter.drawPolyline(mPolyline);

    if (shouldShowStartMarker())
        mStartMarker.paint(painter, mPen, mPolyline.first(), startMarkerAngle());
    if (shouldShowEndMarker())
        mEndMarker.paint(painter, mPen, mPolyline.last(), endMarkerAngle());
}

//======================================================================================================================

void OdgPolylineItem::resize(OdgControlPoint* point, const QPointF& position, bool snapTo45Degrees)
{
	if (point && mControlPoints.contains(point) && mControlPoints.size() >= 2)
	{
		// Determine final point position and convert to item coordinates
		QPointF pointPosition;
		if (snapTo45Degrees)
		{
			pointPosition = mapFromScene(snapResizeTo45Degrees(point, position, mControlPoints.first(),
															   mControlPoints.last()));
		}
		else
			pointPosition = mapFromScene(position);

		// Move corresponding polyline vertex
		QPolygonF polyline = mPolyline;

		const int pointIndex = mControlPoints.indexOf(point);
		polyline.takeAt(pointIndex);
		polyline.insert(pointIndex, pointPosition);

		setPolyline(polyline);
	}
}

//======================================================================================================================

bool OdgPolylineItem::canInsertPoints() const
{
	return (mControlPoints.size() >= 2);
}

bool OdgPolylineItem::canRemovePoints() const
{
	return (mControlPoints.size() > 2);
}

void OdgPolylineItem::insertPoint(const QPointF& position)
{
	if (canInsertPoints())
	{
		const QPointF pointPosition = mapFromScene(position);

		double minimumDistance = distanceFromPointToLineSegment(
			pointPosition, QLineF(mControlPoints.at(0)->position(), mControlPoints.at(1)->position()));
		double insertIndex = 1;
		double distance = 0;

		for(int index = 1; index < mControlPoints.size() - 1; index++)
		{
			distance = distanceFromPointToLineSegment(
				pointPosition, QLineF(mControlPoints.at(index)->position(), mControlPoints.at(index + 1)->position()));
			if (distance < minimumDistance)
			{
				insertIndex = index + 1;
				minimumDistance = distance;
			}
		}

		QPolygonF polyline = mPolyline;
		polyline.insert(insertIndex, pointPosition);
		setPolyline(polyline);
	}
}

void OdgPolylineItem::removePoint(const QPointF& position)
{
	if (canRemovePoints())
	{
		OdgControlPoint* point = pointNearest(mapFromScene(position));
		if (point == mControlPoints.first() || point == mControlPoints.last()) point = nullptr;

		if (point)
		{
			int removeIndex = mControlPoints.indexOf(point);

			QPolygonF polyline = mPolyline;
			polyline.takeAt(removeIndex);
			setPolyline(polyline);
		}
	}
}

//======================================================================================================================

void OdgPolylineItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);

    QPolygonF scaledPolyline;
    for(auto& point : qAsConst(mPolyline))
        scaledPolyline.append(QPointF(point.x() * scale, point.y() * scale));
    setPolyline(scaledPolyline);

    mPen.setWidthF(mPen.widthF() * scale);
    mStartMarker.setSize(mStartMarker.size() * scale);
    mEndMarker.setSize(mEndMarker.size() * scale);
}

//======================================================================================================================

void OdgPolylineItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
	QPolygonF polyline;
	polyline << QPointF() << QPointF();
	setPolyline(polyline);
}

OdgControlPoint* OdgPolylineItem::placeResizeStartPoint() const
{
	return (mControlPoints.size() >= 2) ? mControlPoints.first() : nullptr;
}

OdgControlPoint* OdgPolylineItem::placeResizeEndPoint() const
{
	return (mControlPoints.size() >= 2) ? mControlPoints.last() : nullptr;
}

//======================================================================================================================

bool OdgPolylineItem::shouldShowStartMarker() const
{
    const double length = QLineF(mPolyline.at(0), mPolyline.at(1)).length();
    return (length >= mStartMarker.size());
}

bool OdgPolylineItem::shouldShowEndMarker() const
{
    const double length = QLineF(mPolyline.at(mPolyline.size() - 1), mPolyline.at(mPolyline.size() - 2)).length();
    return (length >= mEndMarker.size());
}

double OdgPolylineItem::startMarkerAngle() const
{
    const QPointF p1 = mPolyline.at(0);
    const QPointF p2 = mPolyline.at(1);
    return qRadiansToDegrees(qAtan2(p1.y() - p2.y(), p1.x() - p2.x()));
}

double OdgPolylineItem::endMarkerAngle() const
{
    const QPointF p1 = mPolyline.at(mPolyline.size() - 1);
    const QPointF p2 = mPolyline.at(mPolyline.size() - 2);
    return qRadiansToDegrees(qAtan2(p1.y() - p2.y(), p1.x() - p2.x()));
}
