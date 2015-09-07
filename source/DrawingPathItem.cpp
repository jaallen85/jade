/* DrawingPathItem.cpp
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

#include "DrawingPathItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"

DrawingPathItem::DrawingPathItem() : DrawingShapeItem()
{
	setName("Path");

	for(int i = 0; i < 8; i++)
		addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));
}

DrawingPathItem::DrawingPathItem(const DrawingPathItem& item) : DrawingShapeItem(item)
{
	mName = item.mName;
	mPath = item.mPath;
	mPathRect = item.mPathRect;
	setItemRect(item.itemRect());

	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	QList<DrawingItemPoint*> otherItemPoints = item.points();
	for(int i = 8; i < points.size(); i++)
		mPathConnectionPoints[points[i]] = item.mPathConnectionPoints[otherItemPoints[i]];
}

DrawingPathItem::~DrawingPathItem() { }

//==================================================================================================

DrawingItem* DrawingPathItem::copy() const
{
	return new DrawingPathItem(*this);
}

//==================================================================================================

void DrawingPathItem::setName(const QString& name)
{
	mName = name;
}

QString DrawingPathItem::name() const
{
	return mName;
}

//==================================================================================================

void DrawingPathItem::setRect(const QRectF& rect)
{
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	if (points.size() >= 8)
	{
		QRectF mapRect = mapFromScene(rect);

		points[0]->setPos(mapRect.left(), mapRect.top());
		points[1]->setPos(mapRect.center().x(), mapRect.top());
		points[2]->setPos(mapRect.right(), mapRect.top());
		points[3]->setPos(mapRect.right(), mapRect.center().y());
		points[4]->setPos(mapRect.right(), mapRect.bottom());
		points[5]->setPos(mapRect.center().x(), mapRect.bottom());
		points[6]->setPos(mapRect.left(), mapRect.bottom());
		points[7]->setPos(mapRect.left(), mapRect.center().y());
	}
}

void DrawingPathItem::setRect(qreal left, qreal top, qreal width, qreal height)
{
	setRect(QRectF(left, top, width, height));
}

QRectF DrawingPathItem::rect() const
{
	QRectF rect;

	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	if (points.size() >= 5)
	{
		QPointF p1 = mapToScene(points[0]->pos());
		QPointF p2 = mapToScene(points[4]->pos());

		rect = QRectF(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p1.x() - p2.x()), qAbs(p1.y() - p2.y()));
	}

	return rect;
}

//==================================================================================================

void DrawingPathItem::setPath(const QPainterPath& path, const QRectF& rect)
{
	mPath = path;
	mPathRect = rect;
}

QPainterPath DrawingPathItem::path() const
{
	return mPath;
}

QRectF DrawingPathItem::pathRect() const
{
	return mPathRect;
}

//==================================================================================================

void DrawingPathItem::addConnectionPoint(const QPointF& pathPos)
{
	bool existingPointFound = false;
	QPointF itemPos = mapFromPath(pathPos);
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	for(auto pointIter = points.begin(); !existingPointFound && pointIter != points.end(); pointIter++)
	{
		existingPointFound = ((*pointIter)->pos() == itemPos);
		if (existingPointFound) (*pointIter)->setFlags((*pointIter)->flags() | DrawingItemPoint::Connection);
	}

	if (!existingPointFound)
	{
		DrawingItemPoint* newPoint = new DrawingItemPoint(itemPos, DrawingItemPoint::Connection);
		mPathConnectionPoints[newPoint] = pathPos;
		addPoint(newPoint);
	}
}

void DrawingPathItem::addConnectionPoints(const QPolygonF& pathPos)
{
	for(auto posIter = pathPos.begin(); posIter != pathPos.end(); posIter++)
		addConnectionPoint(*posIter);
}

QPolygonF DrawingPathItem::connectionPoints() const
{
	QPolygonF pathPos;
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
	{
		if ((*pointIter)->flags() & DrawingItemPoint::Connection)
			pathPos.append(mapToPath((*pointIter)->pos()));
	}

	return pathPos;
}

//==================================================================================================

QPointF DrawingPathItem::mapToPath(const QPointF& itemPos) const
{
	QPointF pathPos;
	QRectF itemRect = DrawingPathItem::itemRect();

	pathPos.setX((itemPos.x() - itemRect.left()) / itemRect.width() * mPathRect.width() + mPathRect.left());
	pathPos.setY((itemPos.y() - itemRect.top()) / itemRect.height() * mPathRect.height() + mPathRect.top());

	return pathPos;
}

QRectF DrawingPathItem::mapToPath(const QRectF& itemRect) const
{
	return QRectF(mapToPath(itemRect.topLeft()), mapToPath(itemRect.bottomRight()));
}

QPointF DrawingPathItem::mapFromPath(const QPointF& pathPos) const
{
	QPointF itemPos;
	QRectF itemRect = DrawingPathItem::itemRect();

	itemPos.setX((pathPos.x() - mPathRect.left()) / mPathRect.width() * itemRect.width() + itemRect.left());
	itemPos.setY((pathPos.y() - mPathRect.top()) / mPathRect.height() * itemRect.height() + itemRect.top());

	return itemPos;
}

QRectF DrawingPathItem::mapFromPath(const QRectF& pathRect) const
{
	return QRectF(mapFromPath(pathRect.topLeft()), mapFromPath(pathRect.bottomRight()));
}

QPainterPath DrawingPathItem::transformedPath() const
{
	QPainterPath transformedPath;
	QList<QPointF> curveDataPoints;

	for(int i = 0; i < mPath.elementCount(); i++)
	{
		QPainterPath::Element element = mPath.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			transformedPath.moveTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::LineToElement:
			transformedPath.lineTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToElement:
			curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToDataElement:
			if (curveDataPoints.size() >= 2)
			{
				transformedPath.cubicTo(curveDataPoints[0], curveDataPoints[1],
					mapFromPath(QPointF(element.x, element.y)));
				curveDataPoints.pop_front();
				curveDataPoints.pop_front();
			}
			else curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		}
	}

	return transformedPath;
}

//==================================================================================================

QRectF DrawingPathItem::boundingRect() const
{
	QRectF rect;
	if (isValid()) rect = itemRect();
	return rect;
}

QPainterPath DrawingPathItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		// Add path
		QPainterPath drawPath = transformedPath();

		// Determine outline path
		QPen pen = DrawingPathItem::pen();
		pen.setWidthF(qMax(pen.widthF(), qAbs(points().first()->itemRect().width())));
		shape = strokePath(drawPath, pen);
	}

	return shape;
}

bool DrawingPathItem::isValid() const
{
	return (!itemRect().isNull() && !mPathRect.isNull() && !mPath.isEmpty());
}

//==================================================================================================

void DrawingPathItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw path
		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawPath(transformedPath());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

void DrawingPathItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& pos)
{
	DrawingItem::resizeItem(itemPoint, pos);

	QList<DrawingItemPoint*> points = DrawingShapeItem::points();
	if (points.size() >= 8)
	{
		QRectF rect(points[0]->pos(), points[4]->pos());
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

	for(auto keyIter = mPathConnectionPoints.begin(); keyIter != mPathConnectionPoints.end(); keyIter++)
		keyIter.key()->setPos(mapFromPath(keyIter.value()));
}

//==================================================================================================

void DrawingPathItem::setItemRect(const QRectF& rect)
{
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	if (points.size() >= 8)
	{
		QRectF mapRect = rect;

		points[0]->setPos(mapRect.left(), mapRect.top());
		points[1]->setPos(mapRect.center().x(), mapRect.top());
		points[2]->setPos(mapRect.right(), mapRect.top());
		points[3]->setPos(mapRect.right(), mapRect.center().y());
		points[4]->setPos(mapRect.right(), mapRect.bottom());
		points[5]->setPos(mapRect.center().x(), mapRect.bottom());
		points[6]->setPos(mapRect.left(), mapRect.bottom());
		points[7]->setPos(mapRect.left(), mapRect.center().y());
	}
}

QRectF DrawingPathItem::itemRect() const
{
	QRectF rect;

	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	if (points.size() >= 5)
	{
		QPointF p1 = points[0]->pos();
		QPointF p2 = points[4]->pos();

		rect = QRectF(p1, p2);
	}

	return rect;
}
