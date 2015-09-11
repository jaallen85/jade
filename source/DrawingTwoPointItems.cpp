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

#include "DrawingTwoPointItems.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"

DrawingTwoPointItem::DrawingTwoPointItem() : DrawingItem()
{
	setPen(QPen(Qt::black, 12.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	setStartArrowStyle(ArrowNone);
	setStartArrowSize(100);
	setEndArrowStyle(ArrowNone);
	setEndArrowSize(100);

	DrawingItemPoint::Flags flags =
		DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free;

	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));
	addPoint(new DrawingItemPoint(QPointF(0, 0), flags));
}

DrawingTwoPointItem::DrawingTwoPointItem(const DrawingTwoPointItem& item) : DrawingItem(item) { }

DrawingTwoPointItem::~DrawingTwoPointItem() { }

//==================================================================================================

void DrawingTwoPointItem::setPen(const QPen& pen)
{
	setPenColor(pen.brush().color());
	setPenWidth(pen.widthF());
	setPenStyle(pen.style());
	setPenCapStyle(pen.capStyle());
	setPenJoinStyle(pen.joinStyle());
}

void DrawingTwoPointItem::setPenColor(const QColor& color)
{
	setProperty("Pen Color", color);
}

void DrawingTwoPointItem::setPenWidth(qreal width)
{
	setProperty("Pen Width", width);
}

void DrawingTwoPointItem::setPenStyle(Qt::PenStyle style)
{
	setProperty("Pen Style", (unsigned int)style);
}

void DrawingTwoPointItem::setPenCapStyle(Qt::PenCapStyle style)
{
	setProperty("Pen Cap Style", (unsigned int)style);
}

void DrawingTwoPointItem::setPenJoinStyle(Qt::PenJoinStyle style)
{
	setProperty("Pen Join Style", (unsigned int)style);
}

QPen DrawingTwoPointItem::pen() const
{
	return QPen(penColor(), penWidth(), penStyle(), penCapStyle(), penJoinStyle());
}

QColor DrawingTwoPointItem::penColor() const
{
	return property("Pen Color").value<QColor>();
}

qreal DrawingTwoPointItem::penWidth() const
{
	return property("Pen Width").toDouble();
}

Qt::PenStyle DrawingTwoPointItem::penStyle() const
{
	return (Qt::PenStyle)property("Pen Style").toUInt();
}

Qt::PenCapStyle DrawingTwoPointItem::penCapStyle() const
{
	return (Qt::PenCapStyle)property("Pen Cap Style").toUInt();
}

Qt::PenJoinStyle DrawingTwoPointItem::penJoinStyle() const
{
	return (Qt::PenJoinStyle)property("Pen Join Style").toUInt();
}

//==================================================================================================

void DrawingTwoPointItem::setStartArrowStyle(DrawingArrowStyle style)
{
	setProperty("Start Arrow Style", (unsigned int)style);
}

void DrawingTwoPointItem::setStartArrowSize(qreal size)
{
	setProperty("Start Arrow Size", size);
}

DrawingArrowStyle DrawingTwoPointItem::startArrowStyle() const
{
	return (DrawingArrowStyle)property("Start Arrow Style").toUInt();
}

qreal DrawingTwoPointItem::startArrowSize() const
{
	return property("Start Arrow Size").toDouble();
}

//==================================================================================================

void DrawingTwoPointItem::setEndArrowStyle(DrawingArrowStyle style)
{
	setProperty("End Arrow Style", (unsigned int)style);
}

void DrawingTwoPointItem::setEndArrowSize(qreal size)
{
	setProperty("End Arrow Size", size);
}

DrawingArrowStyle DrawingTwoPointItem::endArrowStyle() const
{
	return (DrawingArrowStyle)property("End Arrow Style").toUInt();
}

qreal DrawingTwoPointItem::endArrowSize() const
{
	return property("End Arrow Size").toDouble();
}

//==================================================================================================

DrawingItemPoint* DrawingTwoPointItem::startPoint() const
{
	QList<DrawingItemPoint*> points = DrawingTwoPointItem::points();
	return (!points.isEmpty()) ? points.first() : nullptr;
}

DrawingItemPoint* DrawingTwoPointItem::endPoint() const
{
	QList<DrawingItemPoint*> points = DrawingTwoPointItem::points();
	return (!points.isEmpty()) ? points.last() : nullptr;
}

//==================================================================================================

bool DrawingTwoPointItem::newItemCopyEvent()
{
	QList<DrawingItemPoint*> points = DrawingTwoPointItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos(0, 0);

	return true;
}

void DrawingTwoPointItem::newMousePressEvent(DrawingMouseEvent* event)
{
	DrawingItem::newMousePressEvent(event);
}

void DrawingTwoPointItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	if ((event->buttons() & Qt::LeftButton) && endPoint())
	{
		QPointF newPos = event->scenePos();
		if (drawing()) newPos = drawing()->roundToGrid(newPos);

		resizeItem(endPoint(), newPos);
	}
	else DrawingItem::newMouseMoveEvent(event);
}

bool DrawingTwoPointItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
	return isValid();
}

//==================================================================================================

void DrawingTwoPointItem::drawArrow(QPainter* painter, DrawingArrowStyle style, qreal size,
	const QPointF& pos, qreal direction, const QPen& pen, const QBrush& backgroundBrush)
{
	if (style != ArrowNone)
	{
		QPolygonF polygon = calculateArrowPoints(style, size, pos, direction);
		QPen arrowPen = pen;

		if (arrowPen.style() != Qt::NoPen) arrowPen.setStyle(Qt::SolidLine);
		painter->setPen(arrowPen);

		switch (style)
		{
		case ArrowTriangleFilled:
		case ArrowCircleFilled:
		case ArrowDiamondFilled:
		case ArrowConcaveFilled:
			painter->setBrush(pen.brush());
			break;
		case ArrowTriangle:
		case ArrowCircle:
		case ArrowDiamond:
		case ArrowConcave:
			painter->setBrush(backgroundBrush);
			break;
		default:
			painter->setBrush(Qt::transparent);
			break;
		}

		switch (style)
		{
		case ArrowNormal:
		case ArrowReverse:
			painter->drawLine(polygon[0], polygon[1]);
			painter->drawLine(polygon[0], polygon[2]);
			break;
		case ArrowCircle:
		case ArrowCircleFilled:
			painter->drawEllipse(pos, size / 2, size / 2);
			break;
		case ArrowHarpoon:
		case ArrowHarpoonMirrored:
			painter->drawLine(polygon[0], polygon[1]);
			break;
		case ArrowX:
			painter->drawLine(polygon[0], polygon[1]);
			painter->drawLine(polygon[2], polygon[3]);
			break;
		default:
			painter->drawPolygon(polygon);
			break;
		}
	}
}

//==================================================================================================

QPainterPath DrawingTwoPointItem::arrowShape(DrawingArrowStyle style, qreal size,
	const QPointF& pos, qreal direction) const
{
	QPainterPath path;
	QPolygonF polygon;

	if (style != ArrowNone)
	{
		switch (style)
		{
		case ArrowCircle:
		case ArrowCircleFilled:
			path.addEllipse(pos, size / 2, size / 2);
			break;
		case ArrowHarpoon:
		case ArrowHarpoonMirrored:
			polygon = calculateArrowPoints(style, size, pos, direction);
			path.moveTo(polygon[0]);
			path.lineTo(polygon[1]);
			break;
		case ArrowX:
			polygon = calculateArrowPoints(style, size, pos, direction);
			path.moveTo(polygon[0]);
			path.lineTo(polygon[1]);
			path.moveTo(polygon[2]);
			path.lineTo(polygon[3]);
			break;
		default:
			polygon = calculateArrowPoints(style, size, pos, direction);
			path.moveTo(polygon[0]);
			path.addPolygon(polygon);
			path.closeSubpath();
			break;
		}
	}

	return path;
}

QPolygonF DrawingTwoPointItem::calculateArrowPoints(DrawingArrowStyle style, qreal size,
	const QPointF& pos, qreal direction) const
{
	QPolygonF polygon;
	const qreal sqrt2 = qSqrt(2);
	qreal angle = 0;

	direction = direction * 3.141592654 / 180;

	switch (style)
	{
	case ArrowNormal:
	case ArrowTriangle:
	case ArrowTriangleFilled:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction - angle),
							   pos.y() + size / sqrt2 * qSin(direction - angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowDiamond:
	case ArrowDiamondFilled:
		angle = 3.141592654;
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction),
							   pos.y() + size / 2 * qSin(direction)));
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction - angle / 2),
							   pos.y() + size / 2 * qSin(direction - angle / 2)));
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction - angle),
							   pos.y() + size / 2 * qSin(direction - angle)));
		polygon.append(QPointF(pos.x() + size / 2 * qCos(direction + angle / 2),
							   pos.y() + size / 2 * qSin(direction + angle / 2)));
		break;
	case ArrowHarpoon:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction - angle),
							   pos.y() + size / sqrt2 * qSin(direction - angle)));
		break;
	case ArrowHarpoonMirrored:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowConcave:
	case ArrowConcaveFilled:
		angle = 3.141592654 / 6;
		polygon.append(pos);
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction - angle),
							   pos.y() + size / sqrt2 * qSin(direction - angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 / 2 * qCos(direction),
							   pos.y() + size / sqrt2 / 2 * qSin(direction)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowReverse:
		angle = 3.141592654 / 6;
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction),
							   pos.y() + size / sqrt2 * qSin(direction)));
		polygon.append(QPointF(polygon[0].x() - size / sqrt2 * qCos(direction - angle),
							   polygon[0].y() - size / sqrt2 * qSin(direction - angle)));
		polygon.append(QPointF(polygon[0].x() - size / sqrt2 * qCos(direction + angle),
							   polygon[0].y() - size / sqrt2 * qSin(direction + angle)));
		break;
	case ArrowX:
		angle = 3.141592654 / 4;
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + angle),
							   pos.y() + size / sqrt2 * qSin(direction + angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + 5 * angle),
							   pos.y() + size / sqrt2 * qSin(direction + 5 * angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + 3 * angle),
							   pos.y() + size / sqrt2 * qSin(direction + 3 * angle)));
		polygon.append(QPointF(pos.x() + size / sqrt2 * qCos(direction + 7 * angle),
							   pos.y() + size / sqrt2 * qSin(direction + 7 * angle)));
		break;
	default:
		break;
	}

	return polygon;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingLineItem::DrawingLineItem() : DrawingTwoPointItem()
{
	insertPoint(1, new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Connection));
}

DrawingLineItem::DrawingLineItem(const DrawingLineItem& item) : DrawingTwoPointItem(item) { }

DrawingLineItem::~DrawingLineItem() { }

//==================================================================================================

DrawingItem* DrawingLineItem::copy() const
{
	return new DrawingLineItem(*this);
}

//==================================================================================================

void DrawingLineItem::setLine(const QLineF& line)
{
	QList<DrawingItemPoint*> points = DrawingLineItem::points();

	if (points.size() >= 2)
	{
		points.first()->setPos(mapFromScene(line.p1()));
		points.last()->setPos(mapFromScene(line.p2()));
	}

	if (points.size() >= 2) points[1]->setPos(mapFromScene((line.p1() + line.p2()) / 2));
}

void DrawingLineItem::setLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
	setLine(QLineF(x1, y1, x2, y2));
}

QLineF DrawingLineItem::line() const
{
	QLineF line;

	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	if (points.size() >= 2)
	{
		line.setP1(mapToScene(points.first()->pos()));
		line.setP2(mapToScene(points.last()->pos()));
	}

	return line;
}

//==================================================================================================

QRectF DrawingLineItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();

		rect = QRectF(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p1.x() - p2.x()), qAbs(p1.y() - p2.y()));
	}

	return rect;
}

QPainterPath DrawingLineItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		qreal lineAngle = 180 * qAtan2(p2.y() - p1.y(), p2.x() - p1.x()) / 3.141592654;
		QPen pen = DrawingLineItem::pen();

		// Add line
		drawPath.moveTo(p1);
		drawPath.lineTo(p2);

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize())
				drawPath.addPath(arrowShape(startArrowStyle(), startArrowSize(), p1, lineAngle));
			if (lineLength > endArrowSize())
				drawPath.addPath(arrowShape(endArrowStyle(), endArrowSize(), p2, 180 + lineAngle));
		}

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), qAbs(points.first()->itemRect().width())));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingLineItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	return (points.size() >= 2 && points.first()->pos() != points.last()->pos());
}

//==================================================================================================

void DrawingLineItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingLineItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		qreal lineAngle = 180 * qAtan2(p2.y() - p1.y(), p2.x() - p1.x()) / 3.141592654;
		QPen pen = DrawingLineItem::pen();

		// Draw line
		painter->setBrush(Qt::transparent);
		painter->setPen(pen);
		painter->drawLine(p1, p2);

		// Draw arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize())
				drawArrow(painter, startArrowStyle(), startArrowSize(), p1, lineAngle, pen, sceneBrush);
			if (lineLength > endArrowSize())
				drawArrow(painter, endArrowStyle(), endArrowSize(), p2, 180 + lineAngle, pen, sceneBrush);
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

void DrawingLineItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingTwoPointItem::resizeItem(itemPoint, scenePos);

	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	if (points.size() >= 2) points[1]->setPos((points.first()->pos() + points.last()->pos()) / 2);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingArcItem::DrawingArcItem() : DrawingTwoPointItem() { }

DrawingArcItem::DrawingArcItem(const DrawingArcItem& item) : DrawingTwoPointItem(item) { }

DrawingArcItem::~DrawingArcItem() { }

//==================================================================================================

DrawingItem* DrawingArcItem::copy() const
{
	return new DrawingArcItem(*this);
}

//==================================================================================================

void DrawingArcItem::setArc(const QLineF& line)
{
	QList<DrawingItemPoint*> points = DrawingArcItem::points();

	if (points.size() >= 2)
	{
		points.first()->setPos(mapFromScene(line.p1()));
		points.last()->setPos(mapFromScene(line.p2()));
	}
}

void DrawingArcItem::setArc(qreal x1, qreal y1, qreal x2, qreal y2)
{
	setArc(QLineF(x1, y1, x2, y2));
}

QLineF DrawingArcItem::arc() const
{
	QLineF line;

	QList<DrawingItemPoint*> points = DrawingArcItem::points();
	if (points.size() >= 2)
	{
		line.setP1(mapToScene(points.first()->pos()));
		line.setP2(mapToScene(points.last()->pos()));
	}

	return line;
}

//==================================================================================================

QRectF DrawingArcItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QList<DrawingItemPoint*> points = DrawingArcItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();

		rect = QRectF(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p1.x() - p2.x()), qAbs(p1.y() - p2.y()));
	}

	return rect;
}

QPainterPath DrawingArcItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		QList<DrawingItemPoint*> points = DrawingArcItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		QPen pen = DrawingArcItem::pen();

		// Add line
		drawPath.arcMoveTo(arcRect(), arcStartAngle());
		drawPath.arcTo(arcRect(), arcStartAngle(), 90);

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize())
				drawPath.addPath(arrowShape(startArrowStyle(), startArrowSize(), p1, startArrowAngle()));
			if (lineLength > endArrowSize())
				drawPath.addPath(arrowShape(endArrowStyle(), endArrowSize(), p2, endArrowAngle()));
		}

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), qAbs(points.first()->itemRect().width())));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingArcItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingArcItem::points();
	return (points.size() >= 2 && points.first()->pos() != points.last()->pos());
}

//==================================================================================================

void DrawingArcItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingArcItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		QPen pen = DrawingArcItem::pen();

		// Draw arc
		QPainterPath drawPath;
		drawPath.arcMoveTo(arcRect(), arcStartAngle());
		drawPath.arcTo(arcRect(), arcStartAngle(), 90);

		painter->setBrush(Qt::transparent);
		painter->setPen(pen);
		painter->drawPath(drawPath);

		// Draw arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize())
				drawArrow(painter, startArrowStyle(), startArrowSize(), p1, startArrowAngle(), pen, sceneBrush);
			if (lineLength > endArrowSize())
				drawArrow(painter, endArrowStyle(), endArrowSize(), p2, endArrowAngle(), pen, sceneBrush);
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

QLineF DrawingArcItem::itemArc() const
{
	QLineF line;

	QList<DrawingItemPoint*> points = DrawingArcItem::points();
	if (points.size() >= 2)
	{
		line.setP1(points.first()->pos());
		line.setP2(points.last()->pos());
	}

	return line;
}

QRectF DrawingArcItem::arcRect() const
{
	QLineF line = itemArc();

	qreal width = 2 * qAbs(line.x2() - line.x1());
	qreal height = 2 * qAbs(line.y2() - line.y1());

	qreal left = qMin(line.x1(), line.x2());
	qreal top = qMin(line.y1(), line.y2());
	if (line.y1() >= line.y2()) left -= width / 2;
	if (line.x1() < line.x2()) top -= height / 2;

	return QRectF(left, top, width, height);
}

qreal DrawingArcItem::arcStartAngle() const
{
	qreal startAngle = 0;
	QLineF line = itemArc();

	if (line.x1() < line.x2())
	{
		if (line.y1() < line.y2()) startAngle = 180;
		else startAngle = 270;
	}
	else
	{
		if (line.y1() < line.y2()) startAngle = 90;
		else startAngle = 0;
	}

	return startAngle;
}

//==================================================================================================

QPointF DrawingArcItem::pointFromAngle(qreal angle) const
{
	QPointF ellipsePoint;

	QRectF rect = arcRect();
	QLineF line = itemArc();

	angle *= 3.141592654 / 180;

	ellipsePoint.setX(qAbs(rect.width() / 2) * qCos(angle) + rect.center().x());
	ellipsePoint.setY(-qAbs(rect.height() / 2) * qSin(angle) + rect.center().y());

	return ellipsePoint;
}

qreal DrawingArcItem::startArrowAngle() const
{
	QLineF startLine(itemArc().p1(), pointFromAngle(arcStartAngle() + 20));
	return -startLine.angle();
}

qreal DrawingArcItem::endArrowAngle() const
{
	QLineF endLine(itemArc().p2(), pointFromAngle(arcStartAngle() + 70));
	return -endLine.angle();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingCurveItem::DrawingCurveItem() : DrawingTwoPointItem()
{
	insertPoint(1, new DrawingItemPoint(QPointF(), DrawingItemPoint::Control));
	insertPoint(2, new DrawingItemPoint(QPointF(), DrawingItemPoint::Control));
	setCurve(QPointF(-200, -200), QPointF(0, -200), QPointF(0, 200), QPointF(200, 200));
}

DrawingCurveItem::DrawingCurveItem(const DrawingCurveItem& item) : DrawingTwoPointItem(item) { }

DrawingCurveItem::~DrawingCurveItem() { }

//==================================================================================================

DrawingItem* DrawingCurveItem::copy() const
{
	return new DrawingCurveItem(*this);
}

//==================================================================================================

void DrawingCurveItem::setCurve(const QPointF& p1, const QPointF& controlP1, const QPointF& controlP2, const QPointF& p2)
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	if (points.size() >= 4)
	{
		points[0]->setPos(mapFromScene(p1));
		points[1]->setPos(mapFromScene(controlP1));
		points[2]->setPos(mapFromScene(controlP2));
		points[3]->setPos(mapFromScene(p2));
	}
}

QPointF DrawingCurveItem::curveStartPos() const
{
	return mapToScene(points()[0]->pos());
}

QPointF DrawingCurveItem::curveStartControlPos() const
{
	return mapToScene(points()[1]->pos());
}

QPointF DrawingCurveItem::curveEndControlPos() const
{
	return mapToScene(points()[2]->pos());
}

QPointF DrawingCurveItem::curveEndPos() const
{
	return mapToScene(points()[3]->pos());
}

//==================================================================================================

DrawingItemPoint* DrawingCurveItem::startControlPoint() const
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	return (points.size() >= 4) ? points[1] : nullptr;
}

DrawingItemPoint* DrawingCurveItem::endControlPoint() const
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	return (points.size() >= 4) ? points[2] : nullptr;
}

//==================================================================================================

QRectF DrawingCurveItem::boundingRect() const
{
	QPainterPath drawPath;
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	drawPath.moveTo(points[0]->pos());
	drawPath.cubicTo(points[1]->pos(), points[2]->pos(), points[3]->pos());

	return drawPath.boundingRect();
}

QPainterPath DrawingCurveItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		QList<DrawingItemPoint*> points = DrawingCurveItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		QPen pen = DrawingCurveItem::pen();

		// Add arc
		drawPath.moveTo(points[0]->pos());
		drawPath.cubicTo(points[1]->pos(), points[2]->pos(), points[3]->pos());

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize())
				drawPath.addPath(arrowShape(startArrowStyle(), startArrowSize(), p1, startArrowAngle()));
			if (lineLength > endArrowSize())
				drawPath.addPath(arrowShape(endArrowStyle(), endArrowSize(), p2, endArrowAngle()));
		}

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), qAbs(points.first()->itemRect().width())));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingCurveItem::isValid() const
{
	return !boundingRect().isNull();
}

//==================================================================================================

void DrawingCurveItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QList<DrawingItemPoint*> points = DrawingCurveItem::points();
		QPointF p1 = points.first()->pos();
		QPointF p2 = points.last()->pos();
		qreal lineLength = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
		QPen pen = DrawingCurveItem::pen();

		// Draw arc
		QPainterPath drawPath;
		drawPath.moveTo(points[0]->pos());
		drawPath.cubicTo(points[1]->pos(), points[2]->pos(), points[3]->pos());

		painter->setBrush(Qt::transparent);
		painter->setPen(pen);
		painter->drawPath(drawPath);

		// Draw arrows
		if (pen.style() != Qt::NoPen)
		{
			if (lineLength > startArrowSize())
				drawArrow(painter, startArrowStyle(), startArrowSize(), p1, startArrowAngle(), pen, sceneBrush);
			if (lineLength > endArrowSize())
				drawArrow(painter, endArrowStyle(), endArrowSize(), p2, endArrowAngle(), pen, sceneBrush);
		}

		// Draw control lines
		if (isSelected())
		{
			pen.setStyle(Qt::DotLine);
			pen.setWidthF(pen.widthF() * 0.75);
			painter->setBrush(Qt::transparent);
			painter->setPen(pen);
			painter->drawLine(points[0]->pos(), points[1]->pos());
			painter->drawLine(points[3]->pos(), points[2]->pos());
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(Qt::magenta);
	//painter->setPen(QPen(Qt::magenta, 1));
	//painter->drawPath(shape());
}

//==================================================================================================

void DrawingCurveItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	int pointIndex = points.indexOf(itemPoint);

	if (pointIndex == 0)
	{
		QPointF difference = points[1]->pos() - points[0]->pos();
		points[1]->setPos(mapFromScene(scenePos) + difference);

	}
	else if (pointIndex == 3)
	{
		QPointF difference = points[2]->pos() - points[3]->pos();
		points[2]->setPos(mapFromScene(scenePos) + difference);
	}

	DrawingTwoPointItem::resizeItem(itemPoint, scenePos);
}

//==================================================================================================

bool DrawingCurveItem::newItemCopyEvent()
{
	return DrawingItem::newItemCopyEvent();
}

void DrawingCurveItem::newMousePressEvent(DrawingMouseEvent* event)
{
	DrawingItem::newMousePressEvent(event);
}

void DrawingCurveItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	DrawingItem::newMouseMoveEvent(event);
}

bool DrawingCurveItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	return DrawingItem::newMouseReleaseEvent(event);
}

//==================================================================================================

QPointF DrawingCurveItem::pointFromRatio(qreal ratio) const
{
	QPointF pos;
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	QPointF p0 = points[0]->pos();
	QPointF p1 = points[1]->pos();
	QPointF p2 = points[2]->pos();
	QPointF p3 = points[3]->pos();

	pos.setX((1 - ratio)*(1 - ratio)*(1 - ratio) * p0.x() +
		3*ratio*(1 - ratio)*(1 - ratio) * p1.x() +
		3*ratio*ratio*(1 - ratio) * p2.x() +
		ratio*ratio*ratio * p3.x());

	pos.setY((1 - ratio)*(1 - ratio)*(1 - ratio) * p0.y() +
		3*ratio*(1 - ratio)*(1 - ratio) * p1.y() +
		3*ratio*ratio*(1 - ratio) * p2.y() +
		ratio*ratio*ratio * p3.y());

	return pos;
}

qreal DrawingCurveItem::startArrowAngle() const
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	QLineF startLine(points[0]->pos(), pointFromRatio(0.05));
	return -startLine.angle();
}

qreal DrawingCurveItem::endArrowAngle() const
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	QLineF endLine(points[3]->pos(), pointFromRatio(0.95));
	return -endLine.angle();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingPolylineItem::DrawingPolylineItem() : DrawingTwoPointItem() { }

DrawingPolylineItem::DrawingPolylineItem(const DrawingPolylineItem& item) : DrawingTwoPointItem(item) { }

DrawingPolylineItem::~DrawingPolylineItem() { }

//==================================================================================================

DrawingItem* DrawingPolylineItem::copy() const
{
	return new DrawingPolylineItem(*this);
}

//==================================================================================================

void DrawingPolylineItem::setPolyline(const QPolygonF& polygon)
{
	if (polygon.size() >= 2)
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

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	for(int i = 0; i < polygon.size(); i++)
		points[i]->setPos(mapFromScene(polygon[i]));
}

QPolygonF DrawingPolylineItem::polyline() const
{
	QPolygonF polygon;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	for(int i = 0; i < points.size(); i++)
		polygon.append(mapToScene(points[i]->pos()));

	return polygon;
}

//==================================================================================================

QRectF DrawingPolylineItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		QPainterPath path;
		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();

		path.moveTo(points.first()->pos());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
			path.lineTo((*pointIter)->pos());

		rect = path.boundingRect();
	}

	return rect;
}

QPainterPath DrawingPolylineItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;
		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
		QPointF p0 = points[0]->pos();
		QPointF p1 = points[1]->pos();
		QPointF p2 = points[points.size()-2]->pos();
		QPointF p3 = points[points.size()-1]->pos();
		qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
		qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
		qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
		qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;
		QPen pen = DrawingPolylineItem::pen();

		// Add line
		drawPath.moveTo(points.first()->pos());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
		{
			drawPath.lineTo((*pointIter)->pos());
			drawPath.moveTo((*pointIter)->pos());
		}

		// Add arrows
		if (pen.style() != Qt::NoPen)
		{
			if (firstLineLength > startArrowSize())
				drawPath.addPath(arrowShape(startArrowStyle(), startArrowSize(), p0, firstLineAngle));
			if (lastLineLength > endArrowSize())
				drawPath.addPath(arrowShape(endArrowStyle(), endArrowSize(), p3, 180 + lastLineAngle));
		}


		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), qAbs(points.first()->itemRect().width())));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingPolylineItem::isValid() const
{
	bool superfluous = true;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	QPointF pos = points.first()->pos();

	for(auto pointIter = points.begin() + 1; superfluous && pointIter != points.end(); pointIter++)
		superfluous = (pos == (*pointIter)->pos());

	return !superfluous;
}

//==================================================================================================

void DrawingPolylineItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		QPainterPath drawPath;
		QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
		QPointF p0 = points[0]->pos();
		QPointF p1 = points[1]->pos();
		QPointF p2 = points[points.size()-2]->pos();
		QPointF p3 = points[points.size()-1]->pos();
		qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
		qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
		qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
		qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;
		QPen pen = DrawingPolylineItem::pen();

		// Draw line
		drawPath.moveTo(points.first()->pos());
		for(auto pointIter = points.begin() + 1; pointIter != points.end(); pointIter++)
		{
			drawPath.lineTo((*pointIter)->pos());
			drawPath.moveTo((*pointIter)->pos());
		}

		painter->setBrush(Qt::transparent);
		painter->setPen(pen);
		painter->drawPath(drawPath);

		// Draw arrows
		if (pen.style() != Qt::NoPen)
		{
			if (firstLineLength > startArrowSize())
				drawArrow(painter, startArrowStyle(), startArrowSize(), p0, firstLineAngle, pen, sceneBrush);
			if (lastLineLength > endArrowSize())
				drawArrow(painter, endArrowStyle(), endArrowSize(), p3, 180 + lastLineAngle, pen, sceneBrush);
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

int DrawingPolylineItem::insertItemPoint(DrawingItemPoint* itemPoint)
{
	int pointIndex = -1;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	if (itemPoint && points.size() >= 2 && !points.contains(itemPoint))
	{
		pointIndex = 1;
		qreal minimumDistance = distanceFromPointToLineSegment(itemPoint->pos(),
			QLineF(points[0]->pos(), points[1]->pos()));
		qreal distance = 0;

		for(int i = 1; i < points.size() - 1; i++)
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

bool DrawingPolylineItem::removeItemPoint(DrawingItemPoint* itemPoint)
{
	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	return (itemPoint && points.size() > 2 && points.contains(itemPoint) &&
		itemPoint != points.first() && itemPoint != points.last() &&
		itemPoint->connections().isEmpty());
}

//==================================================================================================

qreal DrawingPolylineItem::distanceFromPointToLineSegment(const QPointF& point, const QLineF& line) const
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
