// DrawingTextRectItem.cpp
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

#include "DrawingTextRectItem.h"
#include <QPaintEngine>

DrawingTextRectItem::DrawingTextRectItem() : DrawingRectItem()
{
	setPlaceType(PlaceByMouseRelease);

	mCaption = "Label";
	mTextPen = QPen(Qt::black, 1, Qt::SolidLine);
	mFont = QFont("Arial", 100.0, -1, false);

	setRect(QRectF(-500, -250, 1000, 500));
}

DrawingTextRectItem::DrawingTextRectItem(const DrawingTextRectItem& item) : DrawingRectItem(item)
{
	mCaption = item.mCaption;
	mTextPen = item.mTextPen;
	mFont = item.mFont;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mRectBoundingRect = item.mRectBoundingRect;
	mRectShape = item.mRectShape;
	mTextBoundingRect = item.mTextBoundingRect;
}

DrawingTextRectItem::~DrawingTextRectItem() { }

//==================================================================================================

QString DrawingTextRectItem::uniqueKey() const
{
	return "textRect";
}

DrawingItem* DrawingTextRectItem::copy() const
{
	return new DrawingTextRectItem(*this);
}

//==================================================================================================

void DrawingTextRectItem::setCaption(const QString& caption)
{
	mCaption = caption;
}

QString DrawingTextRectItem::caption() const
{
	return mCaption;
}

//==================================================================================================

void DrawingTextRectItem::setTextBrush(const QBrush& brush)
{
	mTextPen.setBrush(brush);
}

QBrush DrawingTextRectItem::textBrush() const
{
	return mTextPen.brush();
}

//==================================================================================================

void DrawingTextRectItem::setFont(const QFont& font)
{
	mFont = font;
}

QFont DrawingTextRectItem::font() const
{
	return mFont;
}

//==================================================================================================

void DrawingTextRectItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("caption"))
		mCaption = properties.value("caption").toString();

	if (properties.contains("textBrush"))
	{
		QVariant variant = properties.value("textBrush");
		if (variant.canConvert<QBrush>()) mTextPen.setBrush(variant.value<QBrush>());
	}

	if (properties.contains("font"))
	{
		QVariant variant = properties.value("font");
		if (variant.canConvert<QFont>()) mFont = variant.value<QFont>();
	}

	DrawingRectItem::setProperties(properties);
}

QHash<QString,QVariant> DrawingTextRectItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert(DrawingRectItem::properties());
	properties.insert("caption", mCaption);
	properties.insert("textBrush", mTextPen.brush());
	properties.insert("font", mFont);

	return properties;
}

//==================================================================================================

QRectF DrawingTextRectItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingTextRectItem::shape() const
{
	return mShape;
}

bool DrawingTextRectItem::isValid() const
{
	return (DrawingRectItem::isValid() || !mCaption.isEmpty());
}

//==================================================================================================

void DrawingTextRectItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		// Draw rect
		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawRoundedRect(rect(), cornerRadius(), cornerRadius());

		// Draw text
		QFont font = mFont;
		if (painter->paintEngine()->paintDevice())
			font.setPointSizeF(font.pointSizeF() * 96.0 / painter->paintEngine()->paintDevice()->logicalDpiX());

		updateTextRect(font);

		painter->setBrush(Qt::transparent);
		painter->setPen(mTextPen);
		painter->setFont(font);
		painter->drawText(mTextBoundingRect, Qt::AlignCenter, mCaption);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}
}

//==================================================================================================

void DrawingTextRectItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		if (name() != "") xml->writeAttribute("name", name());

		writeTransformToXml(xml, "transform");

		QRectF rect = DrawingTextRectItem::rect();
		xml->writeAttribute("left", QString::number(rect.left()));
		xml->writeAttribute("top", QString::number(rect.top()));
		xml->writeAttribute("width", QString::number(rect.width()));
		xml->writeAttribute("height", QString::number(rect.height()));

		if (cornerRadius() > 0)
			xml->writeAttribute("corner-radius", QString::number(cornerRadius()));

		writePenToXml(xml, "pen", pen());
		writeBrushToXml(xml, "brush", brush());

		writeFontToXml(xml, "font", mFont);
		writeBrushToXml(xml, "text", mTextPen.brush());
		xml->writeCharacters(mCaption);
	}
}

void DrawingTextRectItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();
		QRectF rect;

		if (attr.hasAttribute("name")) setName(attr.value("name").toString());

		readTransformFromXml(xml, "transform");

		if (attr.hasAttribute("corner-radius"))
			setCornerRadius(attr.value("corner-radius").toDouble());
		else
			setCornerRadius(0);

		QPen pen(Qt::black, 1);
		QBrush brush = Qt::white;
		readPenFromXml(xml, "pen", pen);
		readBrushFromXml(xml, "brush", brush);
		readFontFromXml(xml, "font", mFont);
		setPen(pen);
		setBrush(brush);

		brush = Qt::black;
		readBrushFromXml(xml, "text", brush);
		mTextPen.setBrush(brush);

		if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
		if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
		if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
		if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());

		if (xml->readNext() == QXmlStreamReader::Characters)
			mCaption = xml->text().toString();

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		setRect(rect);

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingTextRectItem::updateItemGeometry()
{
	DrawingRectItem::updateItemGeometry();

	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mRectBoundingRect = QRectF();
	mRectShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = pen().widthF() / 2;
		QRectF normalizedRect = rect().normalized();

		// Bounding rect
		mRectBoundingRect = normalizedRect;
		if (pen().style() != Qt::NoPen) mRectBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		mBoundingRect = mRectBoundingRect;
		mBoundingRect = mBoundingRect.united(mTextBoundingRect);

		// Shape
		if (pen().style() != Qt::NoPen)
		{
			QPainterPath drawPath;
			drawPath.addRoundedRect(normalizedRect, cornerRadius(), cornerRadius());

			mRectShape = strokePath(drawPath, pen());
			if (brush().color().alpha() > 0) mRectShape.addPath(drawPath);
		}
		else mRectShape.addRoundedRect(normalizedRect, cornerRadius(), cornerRadius());

		mShape = mRectShape;
		mShape.addRect(mTextBoundingRect);
	}
}

void DrawingTextRectItem::updateTextRect(const QFont& font)
{
	mTextBoundingRect = QRectF();

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
		qreal textLeft = -textWidth / 2;
		qreal textTop = -textHeight / 2;

		mTextBoundingRect = QRectF(textLeft, textTop, textWidth, textHeight).translated(rect().center());

		// Bounding rect and shape
		mBoundingRect = mRectBoundingRect;
		mBoundingRect = mBoundingRect.united(mTextBoundingRect);

		mShape = mRectShape;
		mShape.addRect(mTextBoundingRect);
	}
}
