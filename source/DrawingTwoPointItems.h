/* DrawingTwoPointItems.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGTWOPOINTITEMS_H
#define DRAWINGTWOPOINTITEMS_H

#include <DrawingItem.h>

class DrawingTwoPointItem : public DrawingItem
{
public:
	DrawingTwoPointItem();
	DrawingTwoPointItem(const DrawingTwoPointItem& item);
	virtual ~DrawingTwoPointItem();

	// Selectors
	void setPen(const QPen& pen);
	void setPenColor(const QColor& color);
	void setPenWidth(qreal width);
	void setPenStyle(Qt::PenStyle style);
	void setPenCapStyle(Qt::PenCapStyle style);
	void setPenJoinStyle(Qt::PenJoinStyle style);
	QPen pen() const;
	QColor penColor() const;
	qreal penWidth() const;
	Qt::PenStyle penStyle() const;
	Qt::PenCapStyle penCapStyle() const;
	Qt::PenJoinStyle penJoinStyle() const;

	void setStartArrowStyle(DrawingArrowStyle style);
	void setStartArrowSize(qreal size);
	DrawingArrowStyle startArrowStyle() const;
	qreal startArrowSize() const;

	void setEndArrowStyle(DrawingArrowStyle style);
	void setEndArrowSize(qreal size);
	DrawingArrowStyle endArrowStyle() const;
	qreal endArrowSize() const;

	DrawingItemPoint* startPoint() const;
	DrawingItemPoint* endPoint() const;

protected:
	virtual bool newItemCopyEvent();
	virtual void newMousePressEvent(DrawingMouseEvent* event);
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);

	virtual void drawArrow(QPainter* painter, DrawingArrowStyle style, qreal size, const QPointF& pos, qreal direction,
		const QPen& pen, const QBrush& backgroundBrush);

	QPainterPath arrowShape(DrawingArrowStyle style, qreal size, const QPointF& pos, qreal direction) const;
	QPolygonF calculateArrowPoints(DrawingArrowStyle style, qreal size,	const QPointF& pos, qreal direction) const;
};

//==================================================================================================

class DrawingLineItem : public DrawingTwoPointItem
{
public:
	DrawingLineItem();
	DrawingLineItem(const DrawingLineItem& item);
	virtual ~DrawingLineItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setLine(const QLineF& line);
	void setLine(qreal x1, qreal y1, qreal x2, qreal y2);
	QLineF line() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	// Slots
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
};

//==================================================================================================

class DrawingArcItem : public DrawingTwoPointItem
{
public:
	DrawingArcItem();
	DrawingArcItem(const DrawingArcItem& item);
	virtual ~DrawingArcItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setArc(const QLineF& line);
	void setArc(qreal x1, qreal y1, qreal x2, qreal y2);
	QLineF arc() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

protected:
	QLineF itemArc() const;
	QRectF arcRect() const;
	qreal arcStartAngle() const;

	QPointF pointFromAngle(qreal angle) const;
	qreal startArrowAngle() const;
	qreal endArrowAngle() const;
};

//==================================================================================================

class DrawingCurveItem : public DrawingTwoPointItem
{
public:
	DrawingCurveItem();
	DrawingCurveItem(const DrawingCurveItem& item);
	virtual ~DrawingCurveItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setCurve(const QPointF& p1, const QPointF& controlP1, const QPointF& controlP2, const QPointF& p2);
	QPointF curveStartPos() const;
	QPointF curveStartControlPos() const;
	QPointF curveEndControlPos() const;
	QPointF curveEndPos() const;

	DrawingItemPoint* startControlPoint() const;
	DrawingItemPoint* endControlPoint() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	// Slots
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

protected:
	virtual bool newItemCopyEvent();
	virtual void newMousePressEvent(DrawingMouseEvent* event);
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);

	QPointF pointFromRatio(qreal ratio) const;
	qreal startArrowAngle() const;
	qreal endArrowAngle() const;
};

//==================================================================================================

class DrawingPolylineItem : public DrawingTwoPointItem
{
public:
	DrawingPolylineItem();
	DrawingPolylineItem(const DrawingPolylineItem& item);
	virtual ~DrawingPolylineItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setPolyline(const QPolygonF& polygon);
	QPolygonF polyline() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	// Slots
	virtual int insertItemPoint(DrawingItemPoint* itemPoint);
	virtual bool removeItemPoint(DrawingItemPoint* itemPoint);

private:
	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};

#endif
