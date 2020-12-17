// DrawingRectItem.cpp
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

#include "DrawingRectItem.h"

DrawingRectItem::DrawingRectItem() : DrawingItem()
{
	for(int i = 0; i < 8; i++)
		addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::ControlAndConnection));

	setPlaceType(PlaceByMousePressAndRelease);

	mRect = QRectF(0, 0, 0, 0);
	mCornerRadius = 0;
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mBrush = Qt::white;

	updateItemGeometry();
}

DrawingRectItem::DrawingRectItem(const DrawingRectItem& item) : DrawingItem(item)
{
	mRect = item.mRect;
	mCornerRadius = item.mCornerRadius;
	mPen = item.mPen;
	mBrush = item.mBrush;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingRectItem::~DrawingRectItem() { }

//==================================================================================================

QString DrawingRectItem::name() const
{
	return "rect";
}

DrawingItem* DrawingRectItem::copy() const
{
	return new DrawingRectItem(*this);
}

//==================================================================================================

void DrawingRectItem::setRect(const QRectF& rect)
{
	mRect = rect;

	// Update points
	QList<DrawingItemPoint*> points = DrawingRectItem::points();
	if (points.size() >= 8)
	{
		points[TopLeft]->setPosition(rect.left(), rect.top());
		points[TopMiddle]->setPosition(rect.center().x(), rect.top());
		points[TopRight]->setPosition(rect.right(), rect.top());
		points[MiddleRight]->setPosition(rect.right(), rect.center().y());
		points[BottomRight]->setPosition(rect.right(), rect.bottom());
		points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
		points[BottomLeft]->setPosition(rect.left(), rect.bottom());
		points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
	}

	updateItemGeometry();
}

QRectF DrawingRectItem::rect() const
{
	return mRect;
}

//==================================================================================================

void DrawingRectItem::setCornerRadius(qreal radius)
{
	mCornerRadius = radius;
	updateItemGeometry();
}

qreal DrawingRectItem::cornerRadius() const
{
	return mCornerRadius;
}

//==================================================================================================

void DrawingRectItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateItemGeometry();
}

QPen DrawingRectItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingRectItem::setBrush(const QBrush& brush)
{
	mBrush = brush;
	updateItemGeometry();
}

QBrush DrawingRectItem::brush() const
{
	return mBrush;
}

//==================================================================================================

void DrawingRectItem::setProperties(const QHash<QString,QVariant>& properties)
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

	if (properties.contains("cornerRadius"))
	{
		QVariant variant = properties.value("cornerRadius");
		if (variant.canConvert<qreal>()) mCornerRadius = variant.value<qreal>();
	}

	if (properties.contains("rect"))
	{
		QVariant variant = properties.value("rect");
		if (variant.canConvert<QRectF>())
		{
			QRectF rect = variant.value<QRectF>();
			setRect(QRectF(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight())));
		}
		else updateItemGeometry();
	}
	else updateItemGeometry();
}

QHash<QString,QVariant> DrawingRectItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("pen", mPen);
	properties.insert("brush", mBrush);
	properties.insert("cornerRadius", mCornerRadius);
	properties.insert("rect", QRectF(mapToScene(mRect.topLeft()), mapToScene(mRect.bottomRight())));

	return properties;
}

//==================================================================================================

QRectF DrawingRectItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingRectItem::shape() const
{
	return mShape;
}

bool DrawingRectItem::isValid() const
{
	return (mRect.width() != 0 && mRect.height() != 0);
}

//==================================================================================================

void DrawingRectItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw rect
		painter->setBrush(mBrush);
		painter->setPen(mPen);
		painter->drawRoundedRect(mRect, mCornerRadius, mCornerRadius);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingRectItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	QList<DrawingItemPoint*> points = DrawingRectItem::points();

	if (point == nullptr && points.size() > BottomRight) point = points[BottomRight];

	DrawingItem::resize(point, position);
	adjustFirstPointToOrigin();

	if (points.size() >= 8)
	{
		int pointIndex = points.indexOf(point);

		if (0 <= pointIndex && pointIndex < 8)
		{
			QRectF rect(points[TopLeft]->position(), points[BottomRight]->position());

			switch (pointIndex)
			{
			case TopLeft: rect.setTopLeft(point->position()); break;
			case TopMiddle:	rect.setTop(point->y()); break;
			case TopRight: rect.setTopRight(point->position()); break;
			case MiddleRight: rect.setRight(point->x()); break;
			case BottomRight: rect.setBottomRight(point->position()); break;
			case BottomMiddle: rect.setBottom(point->y()); break;
			case BottomLeft: rect.setBottomLeft(point->position()); break;
			case MiddleLeft: rect.setLeft(point->x()); break;
			default: break;
			}

			setRect(rect);
		}
	}
}

//==================================================================================================

void DrawingRectItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		writeTransformToXml(xml, "transform");

		xml->writeAttribute("left", QString::number(mRect.left()));
		xml->writeAttribute("top", QString::number(mRect.top()));
		xml->writeAttribute("width", QString::number(mRect.width()));
		xml->writeAttribute("height", QString::number(mRect.height()));

		if (mCornerRadius > 0)
			xml->writeAttribute("corner-radius", QString::number(mCornerRadius));

		writePenToXml(xml, "pen", mPen);
		writeBrushToXml(xml, "brush", mBrush);
	}
}

void DrawingRectItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();
		QRectF rect;

		readTransformFromXml(xml, "transform");

		mCornerRadius = 0;
		if (attr.hasAttribute("corner-radius"))
			mCornerRadius = attr.value("corner-radius").toDouble();

		readPenFromXml(xml, "pen", mPen);
		readBrushFromXml(xml, "brush", mBrush);

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
		if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
		if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
		if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
		setRect(rect);

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingRectItem::updateItemGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QRectF normalizedRect = mRect.normalized();
		QPainterPath drawPath;

		// Bounding rect
		mBoundingRect = normalizedRect;
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		drawPath.addRoundedRect(normalizedRect, mCornerRadius, mCornerRadius);

		mShape = strokePath(drawPath, mPen);
		if (mBrush.color().alpha() > 0) mShape.addPath(drawPath);
	}
}
