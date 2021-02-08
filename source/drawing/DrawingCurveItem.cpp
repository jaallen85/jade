// DrawingCurveItem.cpp
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

#include "DrawingCurveItem.h"

DrawingCurveItem::DrawingCurveItem() : DrawingItem()
{
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::FreeControlAndConnection));
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::FreeControlAndConnection));
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));

	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mStartArrow = DrawingArrow(Drawing::ArrowNone, 100.0, mPen);
	mEndArrow = DrawingArrow(Drawing::ArrowNone, 100.0, mPen);

	setCurve(QPointF(-250, -250), QPointF(0, -250), QPointF(0, 250), QPointF(250, 250));
}

DrawingCurveItem::DrawingCurveItem(const DrawingCurveItem& item) : DrawingItem(item)
{
	mStartPos = item.mStartPos;
	mStartControlPos = item.mStartControlPos;
	mEndControlPos = item.mEndControlPos;
	mEndPos = item.mEndPos;

	mPen = item.mPen;
	mStartArrow = item.mStartArrow;
	mEndArrow = item.mEndArrow;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mStrokeRect = item.mStrokeRect;
	mStrokeShape = item.mStrokeShape;
}

DrawingCurveItem::~DrawingCurveItem() { }

//==================================================================================================

QString DrawingCurveItem::uniqueKey() const
{
	return "curve";
}

DrawingItem* DrawingCurveItem::copy() const
{
	return new DrawingCurveItem(*this);
}

//==================================================================================================

void DrawingCurveItem::setCurve(const QPointF& p1, const QPointF& controlP1,
	const QPointF& controlP2, const QPointF& p2)
{
	mStartPos = p1;
	mStartControlPos = controlP1;
	mEndControlPos = controlP2;
	mEndPos = p2;

	// Update points
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();
	if (points.size() >= 4)
	{
		points[StartPoint]->setPosition(p1);
		points[StartControlPoint]->setPosition(controlP1);
		points[EndControlPoint]->setPosition(controlP2);
		points[EndPoint]->setPosition(p2);
	}

	// Update arrows
	qreal length = qSqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) +
		(p2.y() - p1.y()) * (p2.y() - p1.y()));

	mStartArrow.setVector(p1, length, startArrowAngle());
	mEndArrow.setVector(p2, length, endArrowAngle());

	updateItemGeometry();
}

QPointF DrawingCurveItem::curveStartPosition() const
{
	return mStartPos;
}

QPointF DrawingCurveItem::curveStartControlPosition() const
{
	return mStartControlPos;
}

QPointF DrawingCurveItem::curveEndControlPosition() const
{
	return mEndControlPos;
}

QPointF DrawingCurveItem::curveEndPosition() const
{
	return mEndPos;
}

//==================================================================================================

void DrawingCurveItem::setPen(const QPen& pen)
{
	mPen = pen;
	mStartArrow.setPen(mPen);
	mEndArrow.setPen(mPen);

	updateItemGeometry();
}

QPen DrawingCurveItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingCurveItem::setStartArrow(const DrawingArrow& arrow)
{
	mStartArrow.setStyle(arrow.style());
	mStartArrow.setSize(arrow.size());
	updateItemGeometry();
}

void DrawingCurveItem::setEndArrow(const DrawingArrow& arrow)
{
	mEndArrow.setStyle(arrow.style());
	mEndArrow.setSize(arrow.size());
	updateItemGeometry();
}

DrawingArrow DrawingCurveItem::startArrow() const
{
	return mStartArrow;
}

DrawingArrow DrawingCurveItem::endArrow() const
{
	return mEndArrow;
}

//==================================================================================================

void DrawingCurveItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("pen"))
	{
		QVariant variant = properties.value("pen");
		if (variant.canConvert<QPen>())
		{
			mPen = variant.value<QPen>();
			mStartArrow.setPen(mPen);
			mEndArrow.setPen(mPen);
		}
	}

	if (properties.contains("startArrow"))
	{
		QVariant variant = properties.value("startArrow");
		if (variant.canConvert<DrawingArrow>()) mStartArrow = variant.value<DrawingArrow>();
	}

	if (properties.contains("endArrow"))
	{
		QVariant variant = properties.value("endArrow");
		if (variant.canConvert<DrawingArrow>()) mEndArrow = variant.value<DrawingArrow>();
	}

	if (properties.contains("curve"))
	{
		QVariant variant = properties.value("curve");
		if (variant.canConvert<QPolygonF>())
		{
			QPolygonF curve = variant.value<QPolygonF>();
			if (curve.size() >= 4)
			{
				setCurve(mapFromScene(curve[0]), mapFromScene(curve[1]),
					mapFromScene(curve[2]), mapFromScene(curve[3]));
			}
		}
	}
	else updateItemGeometry();
}

QHash<QString,QVariant> DrawingCurveItem::properties() const
{
	QHash<QString,QVariant> properties;
	QVariant startArrowVariant, endArrowVariant;
	QPolygonF curve;

	startArrowVariant.setValue(mStartArrow);
	endArrowVariant.setValue(mEndArrow);
	curve.append(mapToScene(mStartPos));
	curve.append(mapToScene(mStartControlPos));
	curve.append(mapToScene(mEndControlPos));
	curve.append(mapToScene(mEndPos));

	properties.insert("pen", mPen);
	properties.insert("startArrow", startArrowVariant);
	properties.insert("endArrow", endArrowVariant);
	properties.insert("curve", curve);

	return properties;
}

//==================================================================================================

QRectF DrawingCurveItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingCurveItem::shape() const
{
	return mShape;
}

bool DrawingCurveItem::isValid() const
{
	return (mStrokeRect.width() != 0 || mStrokeRect.height() != 0);
}

//==================================================================================================

void DrawingCurveItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw curve
		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->drawPath(mStrokeShape);

		// Draw arrows
		mStartArrow.render(painter, sceneBrush);
		mEndArrow.render(painter, sceneBrush);

		// Draw control lines
		QList<DrawingItemPoint*> points = DrawingCurveItem::points();

		if (isSelected() && points.size() >= 4)
		{
			QPen pen = mPen;

			pen.setStyle(Qt::DotLine);
			pen.setWidthF(pen.widthF() * 0.75);

			painter->setBrush(Qt::transparent);
			painter->setPen(pen);
			painter->drawLine(mStartPos, mStartControlPos);
			painter->drawLine(mEndPos, mEndControlPos);
		}

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingCurveItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	QList<DrawingItemPoint*> points = DrawingCurveItem::points();

	if (point == nullptr && points.size() > EndPoint) point = points[EndPoint];

	DrawingItem::resize(point, position);
	adjustFirstPointToOrigin();

	if (points.size() >= 4)
	{
		QPointF p1 = points[StartPoint]->position();
		QPointF controlP1 = points[StartControlPoint]->position();
		QPointF controlP2 = points[EndControlPoint]->position();
		QPointF p2 = points[EndPoint]->position();

		int pointIndex = points.indexOf(point);
		if (pointIndex == StartPoint)
			controlP1 = p1 + (mStartControlPos - mStartPos);
		else if (pointIndex == EndPoint)
			controlP2 = p2 + (mEndControlPos - mEndPos);

		setCurve(p1, controlP1, controlP2, p2);
	}
}

//==================================================================================================

void DrawingCurveItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		if (name() != "") xml->writeAttribute("name", name());

		QPointF startPos = mapToScene(mStartPos);
		QPointF startControlPos = mapToScene(mStartControlPos);
		QPointF endControlPos = mapToScene(mEndControlPos);
		QPointF endPos = mapToScene(mEndPos);
		xml->writeAttribute("x1", QString::number(startPos.x()));
		xml->writeAttribute("y1", QString::number(startPos.y()));
		xml->writeAttribute("cx1", QString::number(startControlPos.x()));
		xml->writeAttribute("cy1", QString::number(startControlPos.y()));
		xml->writeAttribute("cx2", QString::number(endControlPos.x()));
		xml->writeAttribute("cy2", QString::number(endControlPos.y()));
		xml->writeAttribute("x2", QString::number(endPos.x()));
		xml->writeAttribute("y2", QString::number(endPos.y()));

		writePenToXml(xml, "pen", mPen);
		writeArrowToXml(xml, "start-arrow", mStartArrow);
		writeArrowToXml(xml, "end-arrow", mEndArrow);
	}
}

void DrawingCurveItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();
		QPointF p1, cp1, cp2, p2;

		if (attr.hasAttribute("name")) setName(attr.value("name").toString());

		readTransformFromXml(xml, "transform");

		readPenFromXml(xml, "pen", mPen);
		readArrowFromXml(xml, "start-arrow", mPen, mStartArrow);
		readArrowFromXml(xml, "end-arrow", mPen, mEndArrow);

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		if (attr.hasAttribute("x1")) p1.setX(attr.value("x1").toDouble());
		if (attr.hasAttribute("y1")) p1.setY(attr.value("y1").toDouble());
		if (attr.hasAttribute("cx1")) cp1.setX(attr.value("cx1").toDouble());
		if (attr.hasAttribute("cy1")) cp1.setY(attr.value("cy1").toDouble());
		if (attr.hasAttribute("cx2")) cp2.setX(attr.value("cx2").toDouble());
		if (attr.hasAttribute("cy2")) cp2.setY(attr.value("cy2").toDouble());
		if (attr.hasAttribute("x2")) p2.setX(attr.value("x2").toDouble());
		if (attr.hasAttribute("y2")) p2.setY(attr.value("y2").toDouble());
		setCurve(QPointF(0,0), cp1 - p1, cp2 - p1, p2 - p1);
		setPosition(position() + p1);

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingCurveItem::updateItemGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mStrokeRect = QRectF();
	mStrokeShape = QPainterPath();

	// Stroke path and rect
	mStrokeShape.moveTo(mStartPos);
	mStrokeShape.cubicTo(mStartControlPos, mEndControlPos, mEndPos);

	mStrokeRect = mStrokeShape.boundingRect().normalized();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;

		// Bounding rect
		mBoundingRect = mStrokeRect;
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		mShape = strokePath(mStrokeShape, mPen);

		if (mPen.style() != Qt::NoPen)
		{
			mShape = mShape.united(mStartArrow.shape());
			mShape = mShape.united(mEndArrow.shape());
		}
	}
}

//==================================================================================================

QPointF DrawingCurveItem::pointFromRatio(qreal ratio) const
{
	QPointF position;

	position.setX((1 - ratio)*(1 - ratio)*(1 - ratio) * mStartPos.x() +
		3*ratio*(1 - ratio)*(1 - ratio) * mStartControlPos.x() +
		3*ratio*ratio*(1 - ratio) * mEndControlPos.x() +
		ratio*ratio*ratio * mEndPos.x());

	position.setY((1 - ratio)*(1 - ratio)*(1 - ratio) * mStartPos.y() +
		3*ratio*(1 - ratio)*(1 - ratio) * mStartControlPos.y() +
		3*ratio*ratio*(1 - ratio) * mEndControlPos.y() +
		ratio*ratio*ratio * mEndPos.y());

	return position;
}

qreal DrawingCurveItem::startArrowAngle() const
{
	QLineF startLine(mStartPos, pointFromRatio(0.05));
	return -startLine.angle();
}

qreal DrawingCurveItem::endArrowAngle() const
{
	QLineF endLine(mEndPos, pointFromRatio(0.95));
	return -endLine.angle();
}
