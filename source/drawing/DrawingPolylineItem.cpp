// DrawingPolylineItem.cpp
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

#include "DrawingPolylineItem.h"

DrawingPolylineItem::DrawingPolylineItem() : DrawingItem()
{
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::FreeControlAndConnection));
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::FreeControlAndConnection));

	setPlaceType(PlaceByMousePressAndRelease);

	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mStartArrow = DrawingArrow(Drawing::ArrowNone, 100.0, mPen);
	mEndArrow = DrawingArrow(Drawing::ArrowNone, 100.0, mPen);

	QPolygonF polyline;
	polyline << QPointF(0, 0) << QPointF(0, 0);
	setPolyline(polyline);
}

DrawingPolylineItem::DrawingPolylineItem(const DrawingPolylineItem& item) : DrawingItem(item)
{
	mPolyline = item.mPolyline;
	mPen = item.mPen;
	mStartArrow = item.mStartArrow;
	mEndArrow = item.mEndArrow;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mStrokePath = item.mStrokePath;
}

DrawingPolylineItem::~DrawingPolylineItem() { }

//==================================================================================================

QString DrawingPolylineItem::uniqueKey() const
{
	return "polyline";
}

DrawingItem* DrawingPolylineItem::copy() const
{
	return new DrawingPolylineItem(*this);
}

//==================================================================================================

void DrawingPolylineItem::setPolyline(const QPolygonF& polyline)
{
	mPolyline = polyline;

	// Update points
	if (polyline.size() >= 2)
	{
		while (points().size() < polyline.size())
			insertPoint(1, new DrawingItemPoint(QPointF(), DrawingItemPoint::ControlAndConnection));

		while (points().size() > polyline.size())
		{
			DrawingItemPoint* point = points()[1];
			removePoint(point);
			delete point;
		}
	}

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	for(int i = 0; i < polyline.size(); i++) points[i]->setPosition(polyline[i]);

	// Update arrows
	QPointF p0 = mPolyline[0];
	QPointF p1 = mPolyline[1];
	QPointF p2 = mPolyline[mPolyline.size()-2];
	QPointF p3 = mPolyline[mPolyline.size()-1];
	qreal firstLineLength = qSqrt((p1.x() - p0.x()) * (p1.x() - p0.x()) + (p1.y() - p0.y()) * (p1.y() - p0.y()));
	qreal lastLineLength = qSqrt((p3.x() - p2.x()) * (p3.x() - p2.x()) + (p3.y() - p2.y()) * (p3.y() - p2.y()));
	qreal firstLineAngle = 180 * qAtan2(p1.y() - p0.y(), p1.x() - p0.x()) / 3.141592654;
	qreal lastLineAngle = 180 * qAtan2(p3.y() - p2.y(), p3.x() - p2.x()) / 3.141592654;

	mStartArrow.setVector(p0, firstLineLength, firstLineAngle);
	mEndArrow.setVector(p3, lastLineLength, 180 + lastLineAngle);

	updateItemGeometry();
}

QPolygonF DrawingPolylineItem::polyline() const
{
	return mPolyline;
}

//==================================================================================================

void DrawingPolylineItem::setPen(const QPen& pen)
{
	mPen = pen;
	mStartArrow.setPen(mPen);
	mEndArrow.setPen(mPen);

	updateItemGeometry();
}

QPen DrawingPolylineItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingPolylineItem::setStartArrow(const DrawingArrow& arrow)
{
	mStartArrow.setStyle(arrow.style());
	mStartArrow.setSize(arrow.size());
	updateItemGeometry();
}

void DrawingPolylineItem::setEndArrow(const DrawingArrow& arrow)
{
	mEndArrow.setStyle(arrow.style());
	mEndArrow.setSize(arrow.size());
	updateItemGeometry();
}

DrawingArrow DrawingPolylineItem::startArrow() const
{
	return mStartArrow;
}

DrawingArrow DrawingPolylineItem::endArrow() const
{
	return mEndArrow;
}

//==================================================================================================

void DrawingPolylineItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("pen"))
	{
		QVariant variant = properties.value("pen");
		{
			mPen = variant.value<QPen>();
			mStartArrow.setPen(mPen);
			mEndArrow.setPen(mPen);
		}
	}

	if (properties.contains("startArrow"))
	{
		QVariant variant = properties.value("startArrow");
		if (variant.canConvert<DrawingArrow>())
		{
			mStartArrow = variant.value<DrawingArrow>();
			mStartArrow.setPen(mPen);
		}
	}

	if (properties.contains("endArrow"))
	{
		QVariant variant = properties.value("endArrow");
		if (variant.canConvert<DrawingArrow>())
		{
			mEndArrow = variant.value<DrawingArrow>();
			mEndArrow.setPen(mPen);
		}
	}

	if (properties.contains("polyline"))
	{
		QVariant variant = properties.value("polyline");
		if (variant.canConvert<QPolygonF>()) setPolyline(mapFromScene(variant.value<QPolygonF>()));
		else updateItemGeometry();
	}
	else updateItemGeometry();
}

QHash<QString,QVariant> DrawingPolylineItem::properties() const
{
	QHash<QString,QVariant> properties;
	QVariant startArrowVariant, endArrowVariant;

	startArrowVariant.setValue(mStartArrow);
	endArrowVariant.setValue(mEndArrow);

	properties.insert("pen", mPen);
	properties.insert("startArrow", startArrowVariant);
	properties.insert("endArrow", endArrowVariant);
	properties.insert("polyline", mapToScene(polyline()));

	return properties;
}

//==================================================================================================

QRectF DrawingPolylineItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingPolylineItem::shape() const
{
	return mShape;
}

bool DrawingPolylineItem::isValid() const
{
	QRectF rect = mPolyline.boundingRect();
	return (rect.width() != 0 || rect.height() != 0);
}

//==================================================================================================

void DrawingPolylineItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw line
		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->drawPath(mStrokePath);

		// Draw arrows
		mStartArrow.render(painter, sceneBrush);
		mEndArrow.render(painter, sceneBrush);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingPolylineItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	QPolygonF polyline;

	if (point == nullptr && !points.isEmpty()) point = points.last();

	DrawingItem::resize(point, position);
	adjustFirstPointToOrigin();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polyline << (*pointIter)->position();

	setPolyline(polyline);
}

DrawingItemPoint* DrawingPolylineItem::insertNewPoint(const QPointF& position)
{
	DrawingItemPoint* point = new DrawingItemPoint(mapFromScene(position), DrawingItemPoint::ControlAndConnection);

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	qreal distance = 0;
	qreal minimumDistance = distanceFromPointToLineSegment(point->position(),
		QLineF(points[0]->position(), points[1]->position()));
	int index = 1;

	for(int i = 1; i < points.size() - 1; i++)
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

DrawingItemPoint* DrawingPolylineItem::removeExistingPoint(const QPointF& position, int& pointIndex)
{
	DrawingItemPoint* point = nullptr;

	QList<DrawingItemPoint*> points = DrawingPolylineItem::points();
	if (points.size() > 2)
	{
		point = pointNearest(mapFromScene(position));

		if (point && (point == points.first() || point == points.last()))
			point = nullptr;
	}

	if (point)
	{
		pointIndex = points.indexOf(point);
		removePoint(point);
	}

	return point;
}

//==================================================================================================

void DrawingPolylineItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		if (name() != "") xml->writeAttribute("name", name());

		writeTransformToXml(xml, "transform");

		xml->writeAttribute("points", pointsToString(mPolyline));

		writePenToXml(xml, "pen", mPen);
		writeArrowToXml(xml, "start-arrow", mStartArrow);
		writeArrowToXml(xml, "end-arrow", mEndArrow);
	}
}

void DrawingPolylineItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();
		QPolygonF polyline;

		if (attr.hasAttribute("name")) setName(attr.value("name").toString());

		readTransformFromXml(xml, "transform");

		readPenFromXml(xml, "pen", mPen);
		readArrowFromXml(xml, "start-arrow", mPen, mStartArrow);
		readArrowFromXml(xml, "end-arrow", mPen, mEndArrow);

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		if (attr.hasAttribute("points"))
			polyline = pointsFromString(attr.value("points").toString());
		setPolyline(polyline);

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingPolylineItem::updateItemGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mStrokePath = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;

		// Bounding rect
		mBoundingRect = mPolyline.boundingRect().normalized();
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		mStrokePath.moveTo(mPolyline.first());
		for(auto pointIter = mPolyline.begin() + 1; pointIter != mPolyline.end(); pointIter++)
		{
			mStrokePath.lineTo(*pointIter);
			mStrokePath.moveTo(*pointIter);
		}

		mShape = strokePath(mStrokePath, mPen);

		if (mPen.style() != Qt::NoPen)
		{
			mShape = mShape.united(mStartArrow.shape());
			mShape = mShape.united(mEndArrow.shape());
		}
	}
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

//==================================================================================================

QString DrawingPolylineItem::pointsToString(const QPolygonF& points) const
{
	QString pointsStr;

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		pointsStr += QString::number((*pointIter).x()) + "," + QString::number((*pointIter).y()) + " ";

	return pointsStr.trimmed();
}

QPolygonF DrawingPolylineItem::pointsFromString(const QString& str) const
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
