// DrawingArrow.h
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

#ifndef DRAWINGARROW_H
#define DRAWINGARROW_H

#include <DrawingTypes.h>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QPolygonF>

class DrawingArrow
{
private:
	Drawing::ArrowStyle mStyle;
	qreal mSize;

	QPen mPen;

	QPointF mPosition;
	qreal mLength;
	qreal mAngle;

	QPainterPath mShape;
	QPolygonF mPolygon;

public:
	DrawingArrow(Drawing::ArrowStyle style = Drawing::ArrowNone, qreal size = 0, const QPen& pen = QPen());
	DrawingArrow(const DrawingArrow& arrow);
	~DrawingArrow();

	DrawingArrow& operator=(const DrawingArrow& arrow);

	void setStyle(Drawing::ArrowStyle style);
	void setSize(qreal size);
	Drawing::ArrowStyle style() const;
	qreal size() const;

	void setPen(const QPen& pen);
	QPen pen() const;

	void setVector(const QPointF& position, qreal length, qreal angle);
	QPointF position() const;
	qreal length() const;
	qreal angle() const;

	QPainterPath shape() const;
	QPolygonF polygon() const;

	void render(QPainter* painter, const QBrush& backgroundBrush);

private:
	void updateArrowGeometry();
	QPainterPath strokePath(const QPainterPath& path) const;
};

Q_DECLARE_METATYPE(DrawingArrow)

QDataStream& operator<<(QDataStream& out, const DrawingArrow& arrow);
QDataStream& operator>>(QDataStream& in, DrawingArrow& arrow);

#endif
