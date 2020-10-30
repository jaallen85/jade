// DrawingLineItem.cpp
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

#include "DrawingLineItem.h"

DrawingLineItem::DrawingLineItem() : DrawingItem()
{
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::FreeControlAndConnection));
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::FreeControlAndConnection));
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Connection));

	setPlaceType(PlaceByMousePressAndRelease);

	mLine = QLineF(0, 0, 0, 0);
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mStartArrow = DrawingArrow(Drawing::ArrowNone, 100.0, mPen);
	mEndArrow = DrawingArrow(Drawing::ArrowNone, 100.0, mPen);

	updateItemGeometry();
}

DrawingLineItem::DrawingLineItem(const DrawingLineItem& item) : DrawingItem(item)
{
	mLine = item.mLine;
	mPen = item.mPen;
	mStartArrow = item.mStartArrow;
	mEndArrow = item.mEndArrow;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingLineItem::~DrawingLineItem() { }

//==================================================================================================

QString DrawingLineItem::name() const
{
	return "line";
}

DrawingItem* DrawingLineItem::copy() const
{
	return new DrawingLineItem(*this);
}

//==================================================================================================

void DrawingLineItem::setLine(const QLineF& line)
{
	mLine = line;

	// Update points
	QList<DrawingItemPoint*> points = DrawingLineItem::points();
	if (points.size() > EndPoint)
	{
		points[StartPoint]->setPosition(line.p1());
		points[EndPoint]->setPosition(line.p2());
	}
	if (points.size() > MidPoint) points[MidPoint]->setPosition((line.p1() + line.p2()) / 2);

	// Update arrows
	qreal length = qSqrt((line.x2() - line.x1()) * (line.x2() - line.x1()) +
		(line.y2() - line.y1()) * (line.y2() - line.y1()));
	qreal angle = 180 * qAtan2(line.y2() - line.y1(), line.x2() - line.x1()) / 3.141592654;

	mStartArrow.setVector(line.p1(), length, angle);
	mEndArrow.setVector(line.p2(), length, 180 + angle);

	updateItemGeometry();
}

QLineF DrawingLineItem::line() const
{
	return mLine;
}

//==================================================================================================

void DrawingLineItem::setPen(const QPen& pen)
{
	mPen = pen;
	mStartArrow.setPen(mPen);
	mEndArrow.setPen(mPen);

	updateItemGeometry();
}

QPen DrawingLineItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingLineItem::setStartArrow(const DrawingArrow& arrow)
{
	mStartArrow.setStyle(arrow.style());
	mStartArrow.setSize(arrow.size());
	updateItemGeometry();
}

void DrawingLineItem::setEndArrow(const DrawingArrow& arrow)
{
	mEndArrow.setStyle(arrow.style());
	mEndArrow.setSize(arrow.size());
	updateItemGeometry();
}

DrawingArrow DrawingLineItem::startArrow() const
{
	return mStartArrow;
}

DrawingArrow DrawingLineItem::endArrow() const
{
	return mEndArrow;
}

//==================================================================================================

void DrawingLineItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("pen"))
	{
		QVariant variant = properties.value("pen");
		if (variant.canConvert<QPen>()) mPen = variant.value<QPen>();
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

	if (properties.contains("line"))
	{
		QVariant variant = properties.value("line");
		if (variant.canConvert<QLineF>())
		{
			QLineF line = variant.value<QLineF>();
			setLine(QLineF(mapFromScene(line.p1()), mapFromScene(line.p2())));
		}
		else updateItemGeometry();
	}
	else updateItemGeometry();
}

QHash<QString,QVariant> DrawingLineItem::properties() const
{
	QHash<QString,QVariant> properties;
	QVariant startArrowVariant, endArrowVariant;

	startArrowVariant.setValue(mStartArrow);
	endArrowVariant.setValue(mEndArrow);

	properties.insert("pen", mPen);
	properties.insert("startArrow", startArrowVariant);
	properties.insert("endArrow", endArrowVariant);
	properties.insert("line", QLineF(mapToScene(mLine.p1()), mapToScene(mLine.p2())));

	return properties;
}

//==================================================================================================

QRectF DrawingLineItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingLineItem::shape() const
{
	return mShape;
}

bool DrawingLineItem::isValid() const
{
	return (mLine.x1() != mLine.x2() || mLine.y1() != mLine.y2());
}

//==================================================================================================

void DrawingLineItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw line
		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->drawLine(mLine);

		// Draw arrows
		mStartArrow.render(painter, sceneBrush);
		mEndArrow.render(painter, sceneBrush);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingLineItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	QList<DrawingItemPoint*> points = DrawingLineItem::points();

	if (point == nullptr && points.size() > EndPoint) point = points[EndPoint];

	DrawingItem::resize(point, position);
	adjustFirstPointToOrigin();

	if (points.size() > EndPoint)
	{
		QLineF line;

		line.setP1(points[StartPoint]->position());
		line.setP2(points[EndPoint]->position());

		setLine(line);
	}
}

//==================================================================================================

void DrawingLineItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		writeTransformToXml(xml, "transform");

		xml->writeAttribute("x1", QString::number(mLine.x1()));
		xml->writeAttribute("y1", QString::number(mLine.y1()));
		xml->writeAttribute("x2", QString::number(mLine.x2()));
		xml->writeAttribute("y2", QString::number(mLine.y2()));

		writePenToXml(xml, "pen", mPen);
		writeArrowToXml(xml, "start-arrow", mStartArrow);
		writeArrowToXml(xml, "end-arrow", mEndArrow);
	}
}

void DrawingLineItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();
		QPointF p1, p2;

		readTransformFromXml(xml, "transform");

		readPenFromXml(xml, "pen", mPen);
		readArrowFromXml(xml, "start-arrow", mPen, mStartArrow);
		readArrowFromXml(xml, "end-arrow", mPen, mEndArrow);

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		if (attr.hasAttribute("x1")) p1.setX(attr.value("x1").toDouble());
		if (attr.hasAttribute("y1")) p1.setY(attr.value("y1").toDouble());
		if (attr.hasAttribute("x2")) p2.setX(attr.value("x2").toDouble());
		if (attr.hasAttribute("y2")) p2.setY(attr.value("y2").toDouble());
		setLine(QLineF(p1, p2));
	}
}

//==================================================================================================

void DrawingLineItem::updateItemGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QPainterPath drawPath;

		// Bounding rect (doesn't include arrows)
		mBoundingRect = QRectF(mLine.p1(), mLine.p2()).normalized();
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape (includes arrows)
		drawPath.moveTo(mLine.p1());
		drawPath.lineTo(mLine.p2());

		mShape = strokePath(drawPath, mPen);

		mShape = mShape.united(mStartArrow.shape());
		mShape = mShape.united(mEndArrow.shape());
	}
}
