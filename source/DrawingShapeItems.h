/* DrawingShapeItems.cpp
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

#ifndef DRAWINGSHAPEITEMS_H
#define DRAWINGSHAPEITEMS_H

#include <DrawingItem.h>

class DrawingShapeItem : public DrawingItem
{
public:
	DrawingShapeItem();
	DrawingShapeItem(const DrawingShapeItem& item);
	virtual ~DrawingShapeItem();

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

	void setBrush(const QBrush& brush);
	void setBrushColor(const QColor& color);
	QBrush brush() const;
	QColor brushColor() const;
};

//==================================================================================================

class DrawingRectResizeItem : public DrawingShapeItem
{
public:
	DrawingRectResizeItem();
	DrawingRectResizeItem(const DrawingRectResizeItem& item);
	virtual ~DrawingRectResizeItem();

	// Selectors
	void setRect(const QRectF& rect);
	void setRect(qreal left, qreal top, qreal width, qreal height);
	QRectF rect() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual bool isValid() const;

	// Slots
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);

protected:
	virtual bool newItemCopyEvent();
	virtual void newMousePressEvent(DrawingMouseEvent* event);
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);

	QRectF itemRect() const;
};

//==================================================================================================

class DrawingRectItem : public DrawingRectResizeItem
{
public:
	DrawingRectItem();
	DrawingRectItem(const DrawingRectItem& item);
	virtual ~DrawingRectItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setCornerRadii(qreal cornerRadiusX, qreal cornerRadiusY);
	qreal cornerRadiusX() const;
	qreal cornerRadiusY() const;

	// Description
	virtual QPainterPath shape() const;

	virtual void paint(QPainter* painter);
};

//==================================================================================================

class DrawingEllipseItem : public DrawingRectResizeItem
{
public:
	DrawingEllipseItem();
	DrawingEllipseItem(const DrawingEllipseItem& item);
	virtual ~DrawingEllipseItem();

	virtual DrawingItem* copy() const;

	// Description
	virtual QPainterPath shape() const;

	virtual void paint(QPainter* painter);
};

//==================================================================================================

class DrawingPolygonItem : public DrawingShapeItem
{
public:
	DrawingPolygonItem();
	DrawingPolygonItem(const DrawingPolygonItem& item);
	virtual ~DrawingPolygonItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setPolygon(const QPolygonF& polygon);
	QPolygonF polygon() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	// Slots
	virtual int insertItemPoint(DrawingItemPoint* itemPoint);
	virtual bool removeItemPoint(DrawingItemPoint* itemPoint);

private:
	QPolygonF itemPolygon() const;
	qreal distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const;
};

#endif
