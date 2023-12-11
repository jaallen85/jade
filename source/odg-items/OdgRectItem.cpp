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

OdgItem* OdgRectItem::copy() const
{
	OdgRectItem* rectItem = new OdgRectItem();
	rectItem->setPosition(mPosition);
	rectItem->setRotation(mRotation);
	rectItem->setFlipped(mFlipped);
	rectItem->setRect(mRect);
	rectItem->setBrush(mBrush);
	rectItem->setPen(mPen);
	return rectItem;
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

void OdgRectItem::setProperty(const QString &name, const QVariant &value)
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

QVariant OdgRectItem::property(const QString &name) const
{
	if (name == "rect") return mRect;
	if (name == "brush") return mBrush;
	if (name == "brushColor") return mBrush.color();
	if (name == "pen") return mPen;
	if (name == "penStyle") return static_cast<int>(mPen.style());
	if (name == "penWidth") return mPen.widthF();
	if (name == "penColor") return mPen.brush().color();
	return QVariant();
}

//======================================================================================================================

QRectF OdgRectItem::boundingRect() const
{
    QRectF rect = mRect.normalized();

    // Adjust for pen width and antialiasing
    const double penWidth = mPen.widthF();
    rect.adjust(-penWidth, -penWidth, penWidth, penWidth);

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

void OdgRectItem::resize(OdgControlPoint *point, const QPointF &position, bool snapTo45Degrees)
{
	if (point && mControlPoints.contains(point) && mControlPoints.size() >= NumberOfControlPoints)
	{
		// Determine final point position and convert to item coordinates
		QPointF pointPosition;
		if (snapTo45Degrees)
		{
			pointPosition = mapFromScene(snapResizeTo45Degrees(point, position, mControlPoints.at(TopLeftControlPoint),
															   mControlPoints.at(BottomRightControlPoint)));
		}
		else
			pointPosition = mapFromScene(position);

		// Move corresponding rect vertex (and adjacent vertices as needed to maintain rect shape)
		QRectF rect = mRect;
		const int pointIndex = mControlPoints.indexOf(point);

		// Ensure that rect.width() >= 0
		if (pointIndex == TopLeftControlPoint || pointIndex == MiddleLeftControlPoint ||
			pointIndex == BottomLeftControlPoint)
		{
			if (pointPosition.x() > rect.right()) rect.setLeft(rect.right());
			else rect.setLeft(pointPosition.x());
		}
		else if (pointIndex == TopRightControlPoint || pointIndex == MiddleRightControlPoint ||
			pointIndex == BottomRightControlPoint)
		{
			if (pointPosition.x() < rect.left()) rect.setRight(rect.left());
			else rect.setRight(pointPosition.x());
		}

		// Ensure that rect.height() >= 0
		if (pointIndex == TopLeftControlPoint || pointIndex == TopMiddleControlPoint ||
			pointIndex == TopRightControlPoint)
		{
			if (pointPosition.y() > rect.bottom()) rect.setTop(rect.bottom());
			else rect.setTop(pointPosition.y());
		}
		else if (pointIndex == BottomLeftControlPoint || pointIndex == BottomMiddleControlPoint ||
				 pointIndex == BottomRightControlPoint)
		{
			if (pointPosition.y() < rect.top()) rect.setBottom(rect.top());
			else rect.setBottom(pointPosition.y());
		}

		setRect(rect);
	}
}

//======================================================================================================================

void OdgRectItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);

    setRect(QRectF(mRect.left() * scale, mRect.top() * scale, mRect.width() * scale, mRect.height() * scale));

    mPen.setWidthF(mPen.widthF() * scale);
}

//======================================================================================================================

void OdgRectItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
	double size = 8 * grid;
	if (size <= 0) size = contentRect.width() / 40;
	setRect(QRectF(-size, -size / 2, 2 * size, size));
}

OdgControlPoint* OdgRectItem::placeResizeStartPoint() const
{
	return (mControlPoints.size() >= NumberOfControlPoints) ? mControlPoints.at(TopLeftControlPoint) : nullptr;
}

OdgControlPoint* OdgRectItem::placeResizeEndPoint() const
{
	return (mControlPoints.size() >= NumberOfControlPoints) ? mControlPoints.at(BottomRightControlPoint) : nullptr;
}
