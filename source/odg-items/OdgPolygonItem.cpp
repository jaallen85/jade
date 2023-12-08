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

OdgPolygonItem::OdgPolygonItem() : mPolygon(3, QPointF()), mBrush(), mPen()
{
    for(int i = 0; i < 3; i++) addControlPoint(new OdgControlPoint(QPointF(0, 0)));
}

//======================================================================================================================

OdgItem* OdgPolygonItem::copy() const
{
	OdgPolygonItem* polygonItem = new OdgPolygonItem();
	polygonItem->setPosition(mPosition);
	polygonItem->setRotation(mRotation);
	polygonItem->setFlipped(mFlipped);
	polygonItem->setPolygon(mPolygon);
	polygonItem->setBrush(mBrush);
	polygonItem->setPen(mPen);
	return polygonItem;
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
        const int numberOfPointsToAdd = mPolygon.size() - mControlPoints.size();
        const int numberOfPointsToRemove = mControlPoints.size() - mPolygon.size();

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
        const int finalControlPointCount = qMin(mControlPoints.size(), mPolygon.size());
        for(int i = 0; i < finalControlPointCount; i++)
            mControlPoints.at(i)->setPosition(mPolygon.at(i));
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

void OdgPolygonItem::setProperty(const QString &name, const QVariant &value)
{
	if (name == "brush" && value.canConvert<QBrush>())
	{
		setBrush(value.value<QBrush>());
	}
	else if (name == "brushColor" && value.canConvert<QColor>())
	{
		setBrush(QBrush(value.value<QColor>()));
	}
	else if (name == "pen" && value.canConvert<QPen>())
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
}

QVariant OdgPolygonItem::property(const QString &name) const
{
	if (name == "polygon") return mPolygon;
	if (name == "brush") return mBrush;
	if (name == "brushColor") return mBrush.color();
	if (name == "pen") return mPen;
	if (name == "penStyle") return static_cast<int>(mPen.style());
	if (name == "penWidth") return mPen.widthF();
	if (name == "penColor") return mPen.brush().color();
	return QVariant();
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

void OdgPolygonItem::resize(OdgControlPoint* point, const QPointF& position, bool snapTo45Degrees)
{
	if (point && mControlPoints.contains(point) && mControlPoints.size() >= 2)
	{
		// Determine final point position and convert to item coordinates
		QPointF pointPosition = mapFromScene(position);

		// Move corresponding polygon vertex
		QPolygonF polygon = mPolygon;

		const int pointIndex = mControlPoints.indexOf(point);
		polygon.takeAt(pointIndex);
		polygon.insert(pointIndex, pointPosition);

		setPolygon(polygon);
	}
}

//======================================================================================================================

bool OdgPolygonItem::canInsertPoints() const
{
	return (mControlPoints.size() >= 3);
}

bool OdgPolygonItem::canRemovePoints() const
{
	return (mControlPoints.size() > 3);
}

int OdgPolygonItem::insertPointIndex(const QPointF& position)
{
	if (canInsertPoints())
	{
		const QPointF pointPosition = mapFromScene(position);

        int insertIndex = 0;
        double minimumDistance = distanceFromPointToLineSegment(
			pointPosition, QLineF(mControlPoints.last()->position(), mControlPoints.first()->position()));
		double distance = 0;

		for(int index = 0; index < mControlPoints.size() - 1; index++)
		{
			distance = distanceFromPointToLineSegment(
				pointPosition, QLineF(mControlPoints.at(index)->position(), mControlPoints.at(index + 1)->position()));
			if (distance < minimumDistance)
			{
				insertIndex = index + 1;
				minimumDistance = distance;
			}
		}

        return insertIndex;
	}

    return -1;
}

int OdgPolygonItem::removePointIndex(const QPointF& position)
{
	if (canRemovePoints())
	{
		OdgControlPoint* point = pointNearest(mapFromScene(position));
        return (point) ? mControlPoints.indexOf(point) : -1;
	}

    return -1;
}

void OdgPolygonItem::insertControlPoint(int index, OdgControlPoint* point)
{
    OdgItem::insertControlPoint(index, point);
    updatePolygonFromPoints();
}

void OdgPolygonItem::removeControlPoint(OdgControlPoint* point)
{
    OdgItem::removeControlPoint(point);
    updatePolygonFromPoints();
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

//======================================================================================================================

void OdgPolygonItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
    double size = 4 * grid;
    if (size <= 0) size = contentRect.width() / 80;

	QPolygonF polygon;
	polygon << QPointF(-size, -size) << QPointF(size, 0) << QPointF(-size, size);
	setPolygon(polygon);
}

//======================================================================================================================

void OdgPolygonItem::updatePolygonFromPoints()
{
    QPolygonF polygon;
    for(auto& controlPoint : qAsConst(mControlPoints)) polygon << controlPoint->position();
    mPolygon = polygon;
}
