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

#include "DrawingShapeItems.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"

DrawingShapeItem::DrawingShapeItem() : DrawingItem()
{
	setPen(QPen(Qt::black, 12.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	setBrush(Qt::white);
}

DrawingShapeItem::DrawingShapeItem(const DrawingShapeItem& item) : DrawingItem(item) { }

DrawingShapeItem::~DrawingShapeItem() { }

//==================================================================================================

void DrawingShapeItem::setPen(const QPen& pen)
{
	setPenColor(pen.brush().color());
	setPenWidth(pen.widthF());
	setPenStyle(pen.style());
	setPenCapStyle(pen.capStyle());
	setPenJoinStyle(pen.joinStyle());
}

void DrawingShapeItem::setPenColor(const QColor& color)
{
	setProperty("Pen Color", color);
}

void DrawingShapeItem::setPenWidth(qreal width)
{
	setProperty("Pen Width", width);
}

void DrawingShapeItem::setPenStyle(Qt::PenStyle style)
{
	setProperty("Pen Style", (unsigned int)style);
}

void DrawingShapeItem::setPenCapStyle(Qt::PenCapStyle style)
{
	setProperty("Pen Cap Style", (unsigned int)style);
}

void DrawingShapeItem::setPenJoinStyle(Qt::PenJoinStyle style)
{
	setProperty("Pen Join Style", (unsigned int)style);
}

QPen DrawingShapeItem::pen() const
{
	return QPen(penColor(), penWidth(), penStyle(), penCapStyle(), penJoinStyle());
}

QColor DrawingShapeItem::penColor() const
{
	return property("Pen Color").value<QColor>();
}

qreal DrawingShapeItem::penWidth() const
{
	return property("Pen Width").toDouble();
}

Qt::PenStyle DrawingShapeItem::penStyle() const
{
	return (Qt::PenStyle)property("Pen Style").toUInt();
}

Qt::PenCapStyle DrawingShapeItem::penCapStyle() const
{
	return (Qt::PenCapStyle)property("Pen Cap Style").toUInt();
}

Qt::PenJoinStyle DrawingShapeItem::penJoinStyle() const
{
	return (Qt::PenJoinStyle)property("Pen Join Style").toUInt();
}

//==================================================================================================

void DrawingShapeItem::setBrush(const QBrush& brush)
{
	setBrushColor(brush.color());
}

void DrawingShapeItem::setBrushColor(const QColor& color)
{
	setProperty("Brush Color", color);
}

QBrush DrawingShapeItem::brush() const
{
	return QBrush(brushColor());
}

QColor DrawingShapeItem::brushColor() const
{
	return property("Brush Color").value<QColor>();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingRectResizeItem::DrawingRectResizeItem() : DrawingShapeItem()
{
	DrawingItemPoint::Flags flags = (DrawingItemPoint::Control | DrawingItemPoint::Connection);
	for(int i = 0; i < 8; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), flags));
}

DrawingRectResizeItem::DrawingRectResizeItem(const DrawingRectResizeItem& item) : DrawingShapeItem(item) { }

DrawingRectResizeItem::~DrawingRectResizeItem() { }

//==================================================================================================

void DrawingRectResizeItem::setRect(const QRectF& rect)
{
	QRectF itemRect = mapFromScene(rect);
	QList<DrawingItemPoint*> points = DrawingRectResizeItem::points();

	if (points.size() >= 8)
	{
		QRectF mapRect = rect;

		points[0]->setPos(itemRect.left(), itemRect.top());
		points[1]->setPos(itemRect.center().x(), itemRect.top());
		points[2]->setPos(itemRect.right(), itemRect.top());
		points[3]->setPos(itemRect.right(), itemRect.center().y());
		points[4]->setPos(itemRect.right(), itemRect.bottom());
		points[5]->setPos(itemRect.center().x(), itemRect.bottom());
		points[6]->setPos(itemRect.left(), itemRect.bottom());
		points[7]->setPos(itemRect.left(), itemRect.center().y());
	}
}

void DrawingRectResizeItem::setRect(qreal left, qreal top, qreal width, qreal height)
{
	setRect(QRectF(left, top, width, height));
}

QRectF DrawingRectResizeItem::rect() const
{
	return mapToScene(itemRect());
}

//==================================================================================================

QRectF DrawingRectResizeItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QRectF itemRect = DrawingRectResizeItem::itemRect();
		QPointF p1 = itemRect.topLeft();
		QPointF p2 = itemRect.bottomRight();

		rect = QRectF(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p1.x() - p2.x()), qAbs(p1.y() - p2.y()));
	}

	return rect;
}

bool DrawingRectResizeItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingRectResizeItem::points();
	return (points.size() >= 8 && points[0]->pos() != points[4]->pos());
}

//==================================================================================================

void DrawingRectResizeItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingShapeItem::resizeItem(itemPoint, scenePos);

	QList<DrawingItemPoint*> points = DrawingShapeItem::points();
	if (points.size() >= 8)
	{
		QRectF rect = DrawingRectResizeItem::itemRect();
		int pointIndex = points.indexOf(itemPoint);

		if (0 <= pointIndex && pointIndex < 8)
		{
			switch (pointIndex)
			{
			case 0:	rect.setTopLeft(itemPoint->pos()); break;
			case 1:	rect.setTop(itemPoint->y()); break;
			case 2:	rect.setTopRight(itemPoint->pos()); break;
			case 3:	rect.setRight(itemPoint->x()); break;
			case 4:	rect.setBottomRight(itemPoint->pos()); break;
			case 5:	rect.setBottom(itemPoint->y()); break;
			case 6:	rect.setBottomLeft(itemPoint->pos()); break;
			case 7:	rect.setLeft(itemPoint->x()); break;
			default: break;
			}

			points[0]->setPos(rect.left(), rect.top());
			points[1]->setPos(rect.center().x(), rect.top());
			points[2]->setPos(rect.right(), rect.top());
			points[3]->setPos(rect.right(), rect.center().y());
			points[4]->setPos(rect.right(), rect.bottom());
			points[5]->setPos(rect.center().x(), rect.bottom());
			points[6]->setPos(rect.left(), rect.bottom());
			points[7]->setPos(rect.left(), rect.center().y());
		}
	}
}

//==================================================================================================

bool DrawingRectResizeItem::newItemCopyEvent()
{
	QList<DrawingItemPoint*> points = DrawingShapeItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos(0, 0);

	return true;
}

void DrawingRectResizeItem::newMousePressEvent(DrawingMouseEvent* event)
{
	DrawingItem::newMousePressEvent(event);
}

void DrawingRectResizeItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	QList<DrawingItemPoint*> points = DrawingRectResizeItem::points();

	if ((event->buttons() & Qt::LeftButton) && points[4])
	{
		QPointF newPos = event->scenePos();
		if (drawing()) newPos = drawing()->roundToGrid(newPos);

		resizeItem(points[4], newPos);
	}
	else DrawingItem::newMouseMoveEvent(event);
}

bool DrawingRectResizeItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
	return isValid();
}

//==================================================================================================

QRectF DrawingRectResizeItem::itemRect() const
{
	QList<DrawingItemPoint*> points = DrawingShapeItem::points();
	return (isValid()) ? QRectF(points[0]->pos(), points[4]->pos()) : QRectF();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingRectItem::DrawingRectItem() : DrawingRectResizeItem()
{
	setCornerRadii(0, 0);
}

DrawingRectItem::DrawingRectItem(const DrawingRectItem& item) : DrawingRectResizeItem(item) { }

DrawingRectItem::~DrawingRectItem() { }

//==================================================================================================

DrawingItem* DrawingRectItem::copy() const
{
	return new DrawingRectItem(*this);
}

//==================================================================================================

void DrawingRectItem::setCornerRadii(qreal cornerRadiusX, qreal cornerRadiusY)
{
	setProperty("Corner Radius X", cornerRadiusX);
	setProperty("Corner Radius Y", cornerRadiusY);
}

qreal DrawingRectItem::cornerRadiusX() const
{
	return property("Corner Radius X").toReal();
}

qreal DrawingRectItem::cornerRadiusY() const
{
	return property("Corner Radius Y").toReal();
}

//==================================================================================================

QPainterPath DrawingRectItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;
		QPen pen = DrawingRectItem::pen();

		// Add rect
		drawPath.addRoundedRect(itemRect(), cornerRadiusX(), cornerRadiusY());

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), qAbs(points().first()->itemRect().width())));
		shape = strokePath(drawPath, pen);

		if (brushColor().alpha() > 0) shape.addPath(drawPath);
	}

	return shape;
}

//==================================================================================================

void DrawingRectItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw rect
		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawRoundedRect(itemRect(), cornerRadiusX(), cornerRadiusY());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingEllipseItem::DrawingEllipseItem() : DrawingRectResizeItem()
{
	QList<DrawingItemPoint*> points = DrawingEllipseItem::points();
	for(int i = 0; i < points.size(); i += 2) points[i]->setFlags(DrawingItemPoint::Control);
}

DrawingEllipseItem::DrawingEllipseItem(const DrawingEllipseItem& item) : DrawingRectResizeItem(item) { }

DrawingEllipseItem::~DrawingEllipseItem() { }

//==================================================================================================

DrawingItem* DrawingEllipseItem::copy() const
{
	return new DrawingEllipseItem(*this);
}

//==================================================================================================

QPainterPath DrawingEllipseItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;
		QPen pen = DrawingEllipseItem::pen();

		// Add rect
		drawPath.addEllipse(itemRect());

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), qAbs(points().first()->itemRect().width())));
		shape = strokePath(drawPath, pen);

		if (brushColor().alpha() > 0) shape.addPath(drawPath);
	}

	return shape;
}

//==================================================================================================

void DrawingEllipseItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw rect
		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawEllipse(itemRect());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingPolygonItem::DrawingPolygonItem() : DrawingShapeItem()
{
	addPoint(new DrawingItemPoint(QPointF(-200, -200), DrawingItemPoint::Control | DrawingItemPoint::Connection));
	addPoint(new DrawingItemPoint(QPointF(200, 0), DrawingItemPoint::Control | DrawingItemPoint::Connection));
	addPoint(new DrawingItemPoint(QPointF(-200, 200), DrawingItemPoint::Control | DrawingItemPoint::Connection));
}

DrawingPolygonItem::DrawingPolygonItem(const DrawingPolygonItem& item) : DrawingShapeItem(item) { }

DrawingPolygonItem::~DrawingPolygonItem() { }

//==================================================================================================

DrawingItem* DrawingPolygonItem::copy() const
{
	return new DrawingPolygonItem(*this);
}

//==================================================================================================

void DrawingPolygonItem::setPolygon(const QPolygonF& polygon)
{
	if (polygon.size() >= 3)
	{
		while (points().size() < polygon.size())
			insertPoint(1, new DrawingItemPoint(QPointF(), DrawingItemPoint::Control | DrawingItemPoint::Connection));

		while (points().size() > polygon.size())
		{
			DrawingItemPoint* point = points()[1];
			removePoint(point);
			delete point;
		}
	}

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	for(int i = 0; i < polygon.size(); i++)
		points[i]->setPos(mapFromScene(polygon[i]));
}

QPolygonF DrawingPolygonItem::polygon() const
{
	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	for(int i = 0; i < points.size(); i++)
		polygon.append(mapToScene(points[i]->pos()));

	return polygon;
}

//==================================================================================================

QRectF DrawingPolygonItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QPainterPath path;
		QList<DrawingItemPoint*> points = DrawingPolygonItem::points();

		path.moveTo(points.first()->pos());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
			path.lineTo((*pointIter)->pos());
		path.closeSubpath();

		rect = path.boundingRect();
	}

	return rect;
}

QPainterPath DrawingPolygonItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		// Add polygon
		drawPath.addPolygon(itemPolygon());
		drawPath.closeSubpath();

		// Determine outline path
		QPen pen = DrawingPolygonItem::pen();
		pen.setWidthF(qMax(pen.widthF(), qAbs(points().first()->itemRect().width())));
		shape = strokePath(drawPath, pen);
		if (brushColor().alpha() > 0) shape = shape.united(drawPath);
	}

	return shape;
}

bool DrawingPolygonItem::isValid() const
{
	bool superfluous = true;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	QPointF pos = points.first()->pos();

	for(auto pointIter = points.begin() + 1; superfluous && pointIter != points.end(); pointIter++)
		superfluous = (pos == (*pointIter)->pos());

	return !superfluous;
}

//==================================================================================================

void DrawingPolygonItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw polygon
		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawPolygon(itemPolygon());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

int DrawingPolygonItem::insertItemPoint(DrawingItemPoint* itemPoint)
{
	int pointIndex = -1;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	if (itemPoint && points.size() >= 3 && !points.contains(itemPoint))
	{
		pointIndex = points.size();
		qreal minimumDistance = distanceFromPointToLineSegment(itemPoint->pos(),
			QLineF(points[points.size()-1]->pos(), points[0]->pos()));
		qreal distance = 0;

		for(int i = 0; i < points.size() - 1; i++)
		{
			distance = distanceFromPointToLineSegment(itemPoint->pos(),
				QLineF(points[i]->pos(), points[i+1]->pos()));
			if (distance < minimumDistance)
			{
				pointIndex = i+1;
				minimumDistance = distance;
			}
		}
	}

	return pointIndex;
}

bool DrawingPolygonItem::removeItemPoint(DrawingItemPoint* itemPoint)
{
	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	return (itemPoint && points.size() > 3 && points.contains(itemPoint) &&
		itemPoint->connections().isEmpty());
}

//==================================================================================================

QPolygonF DrawingPolygonItem::itemPolygon() const
{
	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	for(int i = 0; i < points.size(); i++)
		polygon.append(points[i]->pos());

	return polygon;
}

qreal DrawingPolygonItem::distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const
{
	qreal distance = 1E10;
	qreal dotAbBc, dotBaAc, crossABC, distanceAB, distanceAC, distanceBC;

	// Let A = line point 0, B = line point 1, and C = point
	dotAbBc = (line.x2() - line.x1()) * (point.x() - line.x2()) +
			  (line.y2() - line.y1()) * (point.y() - line.y2());
	dotBaAc = (line.x1() - line.x2()) * (point.x() - line.x1()) +
			  (line.y1() - line.y2()) * (point.y() - line.y1());
	crossABC = (line.x2() - line.x1()) * (point.y() - line.y1()) -
			   (line.y2() - line.y1()) * (point.x() - line.x1());
	distanceAB = qSqrt( (line.x2() - line.x1()) * (line.x2() - line.x1()) +
						(line.y2() - line.y1()) * (line.y2() - line.y1()) );
	distanceAC = qSqrt( (point.x() - line.x1()) * (point.x() - line.x1()) +
						(point.y() - line.y1()) * (point.y() - line.y1()) );
	distanceBC = qSqrt( (point.x() - line.x2()) * (point.x() - line.x2()) +
						(point.y() - line.y2()) * (point.y() - line.y2()) );

	if (distanceAB != 0)
	{
		if (dotAbBc > 0) distance = distanceBC;
		else if (dotBaAc > 0) distance = distanceAC;
		else distance = qAbs(crossABC) / distanceAB;
	}

	return distance;
}
