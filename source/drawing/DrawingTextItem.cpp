// DrawingTextItem.cpp
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

#include "DrawingTextItem.h"
#include <QPaintEngine>

DrawingTextItem::DrawingTextItem() : DrawingItem()
{
	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::None));

	mCaption = "Label";
	mPen = QPen(Qt::black, 1, Qt::SolidLine);
	mFont = QFont("Arial", 100.0, -1, false);
	mAlignment = Qt::AlignCenter;
}

DrawingTextItem::DrawingTextItem(const DrawingTextItem& item) : DrawingItem(item)
{
	mCaption = item.mCaption;
	mPen = item.mPen;
	mFont = item.mFont;
	mAlignment = item.mAlignment;

	mTextRect = item.mTextRect;
	mTextShape = item.mTextShape;
}

DrawingTextItem::~DrawingTextItem() { }

//==================================================================================================

QString DrawingTextItem::uniqueKey() const
{
	return "text";
}

DrawingItem* DrawingTextItem::copy() const
{
	return new DrawingTextItem(*this);
}

//==================================================================================================

void DrawingTextItem::setCaption(const QString& caption)
{
	mCaption = caption;
}

QString DrawingTextItem::caption() const
{
	return mCaption;
}

//==================================================================================================

void DrawingTextItem::setTextBrush(const QBrush& brush)
{
	mPen.setBrush(brush);
}

QBrush DrawingTextItem::textBrush() const
{
	return mPen.brush();
}

//==================================================================================================

void DrawingTextItem::setFont(const QFont& font)
{
	mFont = font;
}

QFont DrawingTextItem::font() const
{
	return mFont;
}

//==================================================================================================

void DrawingTextItem::setAlignment(Qt::Alignment alignment)
{
	mAlignment = alignment;
}

Qt::Alignment DrawingTextItem::alignment() const
{
	return mAlignment;
}

//==================================================================================================

void DrawingTextItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("caption"))
		mCaption = properties.value("caption").toString();

	if (properties.contains("textBrush"))
	{
		QVariant variant = properties.value("textBrush");
		if (variant.canConvert<QBrush>()) mPen.setBrush(variant.value<QBrush>());
	}

	if (properties.contains("font"))
	{
		QVariant variant = properties.value("font");
		if (variant.canConvert<QFont>()) mFont = variant.value<QFont>();
	}

	if (properties.contains("alignment"))
	{
		QVariant variant = properties.value("alignment");
		if (variant.canConvert<uint>()) mAlignment = (Qt::Alignment)variant.value<uint>();
	}

	if (properties.contains("position"))
	{
		QVariant variant = properties.value("position");
		if (variant.canConvert<QPointF>()) setPosition(variant.value<QPointF>());
	}
}

QHash<QString,QVariant> DrawingTextItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("caption", mCaption);
	properties.insert("textBrush", mPen.brush());
	properties.insert("font", mFont);
	properties.insert("alignment", (uint)mAlignment);
	properties.insert("position", position());

	return properties;
}

//==================================================================================================

QRectF DrawingTextItem::boundingRect() const
{
	return mTextRect;
}

QPainterPath DrawingTextItem::shape() const
{
	return mTextShape;
}

QPointF DrawingTextItem::centerPosition() const
{
	return QPointF(0, 0);
}

bool DrawingTextItem::isValid() const
{
	return (!mCaption.isEmpty());
}

//==================================================================================================

void DrawingTextItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		// Draw text
		QFont font = mFont;
		if (painter->paintEngine()->paintDevice())
			font.setPointSizeF(font.pointSizeF() * 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX());

		updateTextRect(font);

		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->setFont(font);
		painter->drawText(mTextRect, mAlignment, mCaption);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}
}

//==================================================================================================

void DrawingTextItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		writeTransformToXml(xml, "transform");

		writeFontToXml(xml, "font", mFont);
		writeBrushToXml(xml, "text", mPen.brush());
		writeAlignmentToXml(xml, "text-alignment", mAlignment);

		xml->writeCharacters(mCaption);
	}
}

void DrawingTextItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		readTransformFromXml(xml, "transform");

		readFontFromXml(xml, "font", mFont);

		QBrush brush = Qt::black;
		readBrushFromXml(xml, "text", brush);
		mPen.setBrush(brush);

		readAlignmentFromXml(xml, "text-alignment", mAlignment);

		if (xml->readNext() == QXmlStreamReader::Characters)
			mCaption = xml->text().toString();

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingTextItem::updateTextRect(const QFont& font)
{
	mTextRect = QRectF();
	mTextShape = QPainterPath();

	if (isValid())
	{
		// Bounding rect
		qreal textWidth = 0, textHeight = 0;

		QFontMetricsF fontMetrics(font);
		QStringList lines = mCaption.split("\n");

		for(auto lineIter = lines.begin(); lineIter != lines.end(); lineIter++)
		{
			textWidth = qMax(textWidth, fontMetrics.width(*lineIter));
			textHeight += fontMetrics.lineSpacing();
		}

		textHeight -= fontMetrics.leading();

		// Determine text position
		qreal textLeft = 0, textTop = 0;

		if (mAlignment & Qt::AlignLeft) textLeft = 0;
		else if (mAlignment & Qt::AlignRight) textLeft = -textWidth;
		else textLeft = -textWidth / 2;

		if (mAlignment & Qt::AlignBottom) textTop = -textHeight;
		else if (mAlignment & Qt::AlignTop) textTop = 0;
		else textTop = -textHeight / 2;

		mTextRect = QRectF(textLeft, textTop, textWidth, textHeight);

		// Shape
		mTextShape.addRect(mTextRect);
	}
}
