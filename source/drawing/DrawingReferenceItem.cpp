// DrawingReferenceItem.cpp
// Copyright (C) 2021  Jason Allen
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

#include "DrawingReferenceItem.h"
#include "DrawingWidget.h"
#include <QPaintEngine>

DrawingReferenceItem::DrawingReferenceItem() : DrawingItem()
{
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::None));

	mReferenceName = "";
}

DrawingReferenceItem::DrawingReferenceItem(const DrawingReferenceItem& item) : DrawingItem(item)
{
	mReferenceName = item.mReferenceName;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingReferenceItem::~DrawingReferenceItem() { }

//==================================================================================================

QString DrawingReferenceItem::uniqueKey() const
{
	return "reference";
}

DrawingItem* DrawingReferenceItem::copy() const
{
	return new DrawingReferenceItem(*this);
}

//==================================================================================================

void DrawingReferenceItem::setReferenceName(const QString& name)
{
	mReferenceName = name;
}

QString DrawingReferenceItem::referenceName() const
{
	return mReferenceName;
}

//==================================================================================================

void DrawingReferenceItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("referenceName"))
		setReferenceName(properties.value("referenceName").toString());

	if (properties.contains("position"))
	{
		QVariant variant = properties.value("position");
		if (variant.canConvert<QPointF>()) setPosition(variant.value<QPointF>());
	}
}

QHash<QString,QVariant> DrawingReferenceItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("referenceName", mReferenceName);
	properties.insert("position", position());

	return properties;
}

//==================================================================================================

QRectF DrawingReferenceItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingReferenceItem::shape() const
{
	return mShape;
}

QPointF DrawingReferenceItem::centerPosition() const
{
	return QPointF(0, 0);
}

bool DrawingReferenceItem::isValid() const
{
	return (!mReferenceName.isEmpty());
}

//==================================================================================================

void DrawingReferenceItem::render(QPainter* painter)
{
	DrawingItem* item = nullptr;
	if (widget()) item = widget()->referenceItem(mReferenceName);

	if (item)
	{
		mBoundingRect = item->boundingRect();
		mShape = item->shape();

		painter->translate(item->position());
		painter->setTransform(item->transformInverted(), true);
		item->render(painter);
		painter->setTransform(item->transform(), true);
		painter->translate(-item->position());
	}
	else
	{
		if (!mBoundingRect.isValid())
		{
			mBoundingRect = QRectF(-200, -200, 400, 400);
			mShape.clear();
			mShape.addRect(mBoundingRect);
		}

		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		// Draw rect
		painter->setBrush(Qt::white);
		painter->setPen(QPen(Qt::black, 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		painter->drawRect(mBoundingRect);

		QFont font("Arial", mBoundingRect.height() / 2, -1, false);
		if (painter->paintEngine()->paintDevice())
			font.setPointSizeF(font.pointSizeF() * 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX());
		painter->setBrush(Qt::transparent);
		painter->setFont(font);
		painter->drawText(mBoundingRect, Qt::AlignCenter, "?");

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}
}

//==================================================================================================

void DrawingReferenceItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		writeTransformToXml(xml, "transform");

		xml->writeAttribute("referenceName", mReferenceName);
	}
}

void DrawingReferenceItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		readTransformFromXml(xml, "transform");

		mReferenceName = "";
		if (attr.hasAttribute("referenceName"))
			mReferenceName = attr.value("referenceName").toString();

		xml->skipCurrentElement();
	}
}
