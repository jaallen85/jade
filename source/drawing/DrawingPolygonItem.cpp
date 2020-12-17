// DrawingPolygonItem.cpp
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

#include "DrawingPolygonItem.h"

DrawingPolygonItem::DrawingPolygonItem() : DrawingItem()
{
	for(int i = 0; i < 3; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::ControlAndConnection));

	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mBrush = Qt::white;

	QPolygonF polygon;
	polygon << QPointF(-250, -250) << QPointF(250, 0) << QPointF(-250, 250);
	setPolygon(polygon);
}

DrawingPolygonItem::DrawingPolygonItem(const DrawingPolygonItem& item) : DrawingItem(item)
{
	mPolygon = item.mPolygon;
	mPen = item.mPen;
	mBrush = item.mBrush;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingPolygonItem::~DrawingPolygonItem() { }

//==================================================================================================

QString DrawingPolygonItem::name() const
{
	return "polygon";
}

DrawingItem* DrawingPolygonItem::copy() const
{
	return new DrawingPolygonItem(*this);
}

//==================================================================================================

void DrawingPolygonItem::setPolygon(const QPolygonF& polygon)
{
	mPolygon = polygon;

	if (polygon.size() >= 3)
	{
		while (points().size() < polygon.size())
			insertPoint(1, new DrawingItemPoint(QPointF(), DrawingItemPoint::ControlAndConnection));

		while (points().size() > polygon.size())
		{
			DrawingItemPoint* point = points()[1];
			removePoint(point);
			delete point;
		}
	}

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	for(int i = 0; i < polygon.size(); i++)	points[i]->setPosition(polygon[i]);

	updateItemGeometry();
}

QPolygonF DrawingPolygonItem::polygon() const
{
	return mPolygon;
}

//==================================================================================================

void DrawingPolygonItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateItemGeometry();
}

QPen DrawingPolygonItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingPolygonItem::setBrush(const QBrush& brush)
{
	mBrush = brush;
	updateItemGeometry();
}

QBrush DrawingPolygonItem::brush() const
{
	return mBrush;
}

//==================================================================================================

void DrawingPolygonItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("pen"))
	{
		QVariant variant = properties.value("pen");
		if (variant.canConvert<QPen>()) mPen = variant.value<QPen>();
	}

	if (properties.contains("brush"))
	{
		QVariant variant = properties.value("brush");
		if (variant.canConvert<QBrush>()) mBrush = variant.value<QBrush>();
	}

	if (properties.contains("polygon"))
	{
		QVariant variant = properties.value("polygon");
		if (variant.canConvert<QPolygonF>()) setPolygon(mapFromScene(variant.value<QPolygonF>()));
		else updateItemGeometry();
	}
	else updateItemGeometry();
}

QHash<QString,QVariant> DrawingPolygonItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("pen", mPen);
	properties.insert("brush", mBrush);
	properties.insert("polygon", mapToScene(polygon()));

	return properties;
}

//==================================================================================================

QRectF DrawingPolygonItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingPolygonItem::shape() const
{
	return mShape;
}

bool DrawingPolygonItem::isValid() const
{
	QRectF rect = mPolygon.boundingRect();
	return (rect.width() != 0 || rect.height() != 0);
}

//==================================================================================================

void DrawingPolygonItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw polygon
		painter->setBrush(mBrush);
		painter->setPen(mPen);
		painter->drawPolygon(mPolygon);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingPolygonItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	QPolygonF polygon;

	if (point == nullptr && !points.isEmpty()) point = points.last();

	DrawingItem::resize(point, position);
	adjustFirstPointToOrigin();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon << (*pointIter)->position();

	setPolygon(polygon);
}

DrawingItemPoint* DrawingPolygonItem::insertNewPoint(const QPointF& position)
{
	DrawingItemPoint* point = new DrawingItemPoint(mapFromScene(position), DrawingItemPoint::ControlAndConnection);

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	qreal distance = 0;
	qreal minimumDistance = distanceFromPointToLineSegment(point->position(),
		QLineF(points[points.size()-1]->position(), points[0]->position()));
	int index = points.size();

	for(int i = 0; i < points.size() - 1; i++)
	{
		distance = distanceFromPointToLineSegment(point->position(),
			QLineF(points[i]->position(), points[i+1]->position()));
		if (distance < minimumDistance)
		{
			index = i+1;
			minimumDistance = distance;
		}
	}

	insertPoint(index, point);

	return point;
}

DrawingItemPoint* DrawingPolygonItem::removeExistingPoint(const QPointF& position, int& pointIndex)
{
	DrawingItemPoint* point = nullptr;

	QList<DrawingItemPoint*> points = DrawingPolygonItem::points();
	if (points.size() > 3) point = pointNearest(mapFromScene(position));

	if (point)
	{
		pointIndex = points.indexOf(point);
		removePoint(point);
	}

	return point;
}

//==================================================================================================

void DrawingPolygonItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		writeTransformToXml(xml, "transform");

		xml->writeAttribute("points", pointsToString(mPolygon));

		writePenToXml(xml, "pen", mPen);
		writeBrushToXml(xml, "brush", mBrush);
	}
}

void DrawingPolygonItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		readTransformFromXml(xml, "transform");

		readPenFromXml(xml, "pen", mPen);
		readBrushFromXml(xml, "brush", mBrush);

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		if (attr.hasAttribute("points"))
			setPolygon(pointsFromString(attr.value("points").toString()));

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingPolygonItem::updateItemGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QPainterPath drawPath;

		// Bounding rect
		mBoundingRect = mPolygon.boundingRect().normalized();
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		drawPath.addPolygon(polygon());
		drawPath.closeSubpath();

		mShape = strokePath(drawPath, mPen);
		if (mBrush.color().alpha() > 0) mShape = mShape.united(drawPath);
	}
}

//==================================================================================================

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

//==================================================================================================

QString DrawingPolygonItem::pointsToString(const QPolygonF& points) const
{
	QString pointsStr;

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		pointsStr += QString::number((*pointIter).x()) + "," + QString::number((*pointIter).y()) + " ";

	return pointsStr.trimmed();
}

QPolygonF DrawingPolygonItem::pointsFromString(const QString& str) const
{
	QPolygonF points;

	QStringList tokenList = str.split(" ");
	QStringList coordList;
	qreal x, y;
	bool xOk = false, yOk = false;

	for(int i = 0; i < tokenList.size(); i++)
	{
		coordList = tokenList[i].split(",");
		if (coordList.size() == 2)
		{
			x = coordList[0].toDouble(&xOk);
			y = coordList[1].toDouble(&yOk);
			if (xOk && yOk) points.append(QPointF(x, y));
		}
	}

	return points;
}
