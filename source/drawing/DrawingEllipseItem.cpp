// DrawingEllipseItem.cpp
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

#include "DrawingEllipseItem.h"

DrawingEllipseItem::DrawingEllipseItem() : DrawingItem()
{
	for(int i = 0; i < 4; i++)
		addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));
	for(int i = 0; i < 4; i++)
		addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::ControlAndConnection));

	setPlaceType(PlaceByMousePressAndRelease);

	mEllipse = QRectF(0, 0, 0, 0);
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mBrush = Qt::white;

	updateItemGeometry();
}

DrawingEllipseItem::DrawingEllipseItem(const DrawingEllipseItem& item) : DrawingItem(item)
{
	mEllipse = item.mEllipse;
	mPen = item.mPen;
	mBrush = item.mBrush;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingEllipseItem::~DrawingEllipseItem() { }

//==================================================================================================

QString DrawingEllipseItem::uniqueKey() const
{
	return "ellipse";
}

DrawingItem* DrawingEllipseItem::copy() const
{
	return new DrawingEllipseItem(*this);
}

//==================================================================================================

void DrawingEllipseItem::setEllipse(const QRectF& rect)
{
	mEllipse = rect;

	// Update points
	QList<DrawingItemPoint*> points = DrawingEllipseItem::points();
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

QRectF DrawingEllipseItem::ellipse() const
{
	return mEllipse;
}

//==================================================================================================

void DrawingEllipseItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateItemGeometry();
}

QPen DrawingEllipseItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingEllipseItem::setBrush(const QBrush& brush)
{
	mBrush = brush;
	updateItemGeometry();
}

QBrush DrawingEllipseItem::brush() const
{
	return mBrush;
}

//==================================================================================================

void DrawingEllipseItem::setProperties(const QHash<QString,QVariant>& properties)
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

	if (properties.contains("ellipse"))
	{
		QVariant variant = properties.value("ellipse");
		if (variant.canConvert<QRectF>())
		{
			QRectF rect = variant.value<QRectF>();
			setEllipse(QRectF(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight())));
		}
		else updateItemGeometry();
	}
	else updateItemGeometry();
}

QHash<QString,QVariant> DrawingEllipseItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("pen", mPen);
	properties.insert("brush", mBrush);
	properties.insert("ellipse", QRectF(mapToScene(mEllipse.topLeft()), mapToScene(mEllipse.bottomRight())));

	return properties;
}

//==================================================================================================

QRectF DrawingEllipseItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingEllipseItem::shape() const
{
	return mShape;
}

bool DrawingEllipseItem::isValid() const
{
	return (mEllipse.width() != 0 && mEllipse.height() != 0);
}

//==================================================================================================

void DrawingEllipseItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw ellipse
		painter->setBrush(mBrush);
		painter->setPen(mPen);
		painter->drawEllipse(mEllipse);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingEllipseItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	QList<DrawingItemPoint*> points = DrawingEllipseItem::points();

	if (point == nullptr && points.size() > BottomRight) point = points[BottomRight];

	DrawingItem::resize(point, position);
	adjustFirstPointToOrigin();

	if (points.size() >= 8)
	{
		int pointIndex = points.indexOf(point);

		if (0 <= pointIndex && pointIndex < 8)
		{
			QRectF rect;

			rect.setTopLeft(points[TopLeft]->position());
			rect.setBottomRight(points[BottomRight]->position());

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

			setEllipse(rect);
		}
	}
}

//==================================================================================================

void DrawingEllipseItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		if (name() != "") xml->writeAttribute("name", name());

		QRectF rect(mapToScene(mEllipse.topLeft()), mapToScene(mEllipse.bottomRight()));
		xml->writeAttribute("left", QString::number(rect.left()));
		xml->writeAttribute("top", QString::number(rect.top()));
		xml->writeAttribute("width", QString::number(rect.width()));
		xml->writeAttribute("height", QString::number(rect.height()));

		writePenToXml(xml, "pen", mPen);
		writeBrushToXml(xml, "brush", mBrush);
	}
}

void DrawingEllipseItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();
		QRectF rect;

		if (attr.hasAttribute("name")) setName(attr.value("name").toString());

		readTransformFromXml(xml, "transform");

		readPenFromXml(xml, "pen", mPen);
		readBrushFromXml(xml, "brush", mBrush);

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
		if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
		if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
		if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
		setEllipse(rect.translated(-rect.topLeft()));
		setPosition(position() + rect.topLeft());

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingEllipseItem::exportToSvg(QXmlStreamWriter* xml)
{
	if (xml)
	{
		xml->writeStartElement("ellipse");

		QRectF rect(mapToScene(mEllipse.topLeft()), mapToScene(mEllipse.bottomRight()));
		rect = rect.normalized();

		xml->writeAttribute("cx", QString::number(rect.center().x()));
		xml->writeAttribute("cy", QString::number(rect.center().y()));
		xml->writeAttribute("rx", QString::number(rect.width() / 2));
		xml->writeAttribute("ry", QString::number(rect.height() / 2));

		exportStyleToSvg(xml, mBrush, mPen);

		xml->writeEndElement();
	}
}

//==================================================================================================

void DrawingEllipseItem::updateItemGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QRectF normalizedRect = mEllipse.normalized();

		// Bounding rect
		mBoundingRect = normalizedRect;
		if (mPen.style() != Qt::NoPen) mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		if (mPen.style() != Qt::NoPen)
		{
			QPainterPath drawPath;
			drawPath.addEllipse(normalizedRect);

			mShape = strokePath(drawPath, mPen);
			if (mBrush.color().alpha() > 0) mShape.addPath(drawPath);
		}
		else mShape.addEllipse(normalizedRect);
	}
}
