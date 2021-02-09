// DrawingArrow.cpp
// Copyright (C) 2020  Jason Allen
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

#include "DrawingArrow.h"
#include <QPainter>
#include <QtMath>

DrawingArrow::DrawingArrow(Drawing::ArrowStyle style, qreal size, const QPen& pen)
{
	mStyle = style;
	mSize = size;

	mPen = pen;

	mLength = 0;
	mAngle = 0;

	updateArrowGeometry();
}

DrawingArrow::DrawingArrow(const DrawingArrow& arrow)
{
	mStyle = arrow.mStyle;
	mSize = arrow.mSize;
	mPen = arrow.mPen;
	mPosition = arrow.mPosition;
	mLength = arrow.mLength;
	mAngle = arrow.mAngle;
	mShape = arrow.mShape;
	mPolygon = arrow.mPolygon;
}

DrawingArrow::~DrawingArrow() { }

//==================================================================================================

DrawingArrow& DrawingArrow::operator=(const DrawingArrow& arrow)
{
	mStyle = arrow.mStyle;
	mSize = arrow.mSize;
	mPen = arrow.mPen;
	mPosition = arrow.mPosition;
	mLength = arrow.mLength;
	mAngle = arrow.mAngle;
	mShape = arrow.mShape;
	mPolygon = arrow.mPolygon;
	return *this;
}

//==================================================================================================

void DrawingArrow::setStyle(Drawing::ArrowStyle style)
{
	mStyle = style;
	updateArrowGeometry();
}

void DrawingArrow::setSize(qreal size)
{
	mSize = size;
	updateArrowGeometry();
}

Drawing::ArrowStyle DrawingArrow::style() const
{
	return mStyle;
}

qreal DrawingArrow::size() const
{
	return mSize;
}

//==================================================================================================

void DrawingArrow::setPen(const QPen& pen)
{
	mPen = pen;
	updateArrowGeometry();
}

QPen DrawingArrow::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingArrow::setVector(const QPointF& position, qreal length, qreal angle)
{
	mPosition = position;
	mLength = length;
	mAngle = angle;
	updateArrowGeometry();
}

QPointF DrawingArrow::position() const
{
	return mPosition;
}

qreal DrawingArrow::length() const
{
	return mLength;
}

qreal DrawingArrow::angle() const
{
	return mAngle;
}

//==================================================================================================

QPainterPath DrawingArrow::shape() const
{
	return mShape;
}

QPolygonF DrawingArrow::polygon() const
{
	return mPolygon;
}

//==================================================================================================

void DrawingArrow::render(QPainter* painter, const QBrush& backgroundBrush)
{
	if (mStyle != Drawing::ArrowNone && mSize <= mLength && mPen.style() != Qt::NoPen)
	{
		QBrush originalBrush = painter->brush();
		QPen originalPen = painter->pen();

		// Set pen
		QPen arrowPen = mPen;
		arrowPen.setStyle(Qt::SolidLine);
		painter->setPen(arrowPen);

		// Set brush
		switch (mStyle)
		{
		case Drawing::ArrowTriangleFilled:
		case Drawing::ArrowConcaveFilled:
		case Drawing::ArrowCircleFilled:
			painter->setBrush(arrowPen.brush());
			break;
		case Drawing::ArrowTriangle:
		case Drawing::ArrowConcave:
		case Drawing::ArrowCircle:
			painter->setBrush(backgroundBrush);
			break;
		default:
			painter->setBrush(Qt::transparent);
			break;
		}

		// Draw arrow
		switch (mStyle)
		{
		case Drawing::ArrowNormal:
			painter->drawLine(mPolygon[0], mPolygon[1]);
			painter->drawLine(mPolygon[0], mPolygon[2]);
			break;
		case Drawing::ArrowCircle:
		case Drawing::ArrowCircleFilled:
			painter->drawEllipse(mPosition, mSize / 2, mSize / 2);
			break;
		default:
			painter->drawPolygon(mPolygon);
			break;
		}

		painter->setPen(originalPen);
		painter->setBrush(originalBrush);
	}
}

//==================================================================================================

void DrawingArrow::updateArrowGeometry()
{
	mShape = QPainterPath();
	mPolygon.clear();

	if (mStyle != Drawing::ArrowNone && mSize <= mLength && mPen.style() != Qt::NoPen)
	{
		// Calculate polygon points
		const qreal sqrt2 = qSqrt(2);
		qreal direction = mAngle * 3.141592654 / 180;
		qreal angle = 0;
		QPainterPath drawPath;

		switch (mStyle)
		{
		case Drawing::ArrowNormal:
		case Drawing::ArrowTriangle:
		case Drawing::ArrowTriangleFilled:
			angle = 3.141592654 / 6;

			mPolygon.append(mPosition);
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction - angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction - angle)));
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction + angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction + angle)));
			break;
		case Drawing::ArrowConcave:
		case Drawing::ArrowConcaveFilled:
			angle = 3.141592654 / 6;

			mPolygon.append(mPosition);
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction - angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction - angle)));
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 / 2 * qCos(direction),
									mPosition.y() + mSize / sqrt2 / 2 * qSin(direction)));
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction + angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction + angle)));
			break;
		default:
			break;
		}

		// Create shape
		switch (mStyle)
		{
		case Drawing::ArrowCircle:
		case Drawing::ArrowCircleFilled:
			mShape.addEllipse(mPosition, (mSize + mPen.widthF()) / 2, (mSize + mPen.widthF()) / 2);
			break;
		default:
			drawPath.addPolygon(mPolygon);
			drawPath.closeSubpath();

			mShape = strokePath(drawPath);
			mShape.addPolygon(mPolygon);
			break;
		}
	}
}

QPainterPath DrawingArrow::strokePath(const QPainterPath& path) const
{
	if (path == QPainterPath()) return path;

	QPainterPathStroker ps;
	const qreal penWidthZero = qreal(0.00000001);

	if (mPen.widthF() <= 0.0)
		ps.setWidth(penWidthZero);
	else
		ps.setWidth(mPen.widthF());

	ps.setCapStyle(Qt::RoundCap);
	ps.setJoinStyle(Qt::RoundJoin);

	return ps.createStroke(path);
}

//==================================================================================================

QDataStream& operator<<(QDataStream& out, const DrawingArrow& arrow)
{
	out << (uint)arrow.style() << arrow.size();
	return out;
}

QDataStream& operator>>(QDataStream& in, DrawingArrow& arrow)
{
	uint style = 0;
	qreal size = 0.0;

	in >> style >> size;

	arrow.setStyle((Drawing::ArrowStyle)style);
	arrow.setSize(size);

	return in;
}
