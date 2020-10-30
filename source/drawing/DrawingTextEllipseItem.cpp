// DrawingTextEllipseItem.cpp
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

#include "DrawingTextEllipseItem.h"
#include <QPaintEngine>

DrawingTextEllipseItem::DrawingTextEllipseItem() : DrawingEllipseItem()
{
	setPlaceType(PlaceByMouseRelease);

	mCaption = "Label";
	mTextPen = QPen(Qt::black, 1, Qt::SolidLine);
	mFont = QFont("Arial", 100.0, -1, false);

	setEllipse(QRectF(-500, -250, 1000, 500));
}

DrawingTextEllipseItem::DrawingTextEllipseItem(const DrawingTextEllipseItem& item) : DrawingEllipseItem(item)
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

DrawingTextEllipseItem::~DrawingTextEllipseItem() { }

//==================================================================================================

QString DrawingTextEllipseItem::name() const
{
	return "textEllipse";
}

DrawingItem* DrawingTextEllipseItem::copy() const
{
	return new DrawingTextEllipseItem(*this);
}

//==================================================================================================

void DrawingTextEllipseItem::setCaption(const QString& caption)
{
	mCaption = caption;
}

QString DrawingTextEllipseItem::caption() const
{
	return mCaption;
}

//==================================================================================================

void DrawingTextEllipseItem::setTextBrush(const QBrush& brush)
{
	mTextPen.setBrush(brush);
}

QBrush DrawingTextEllipseItem::textBrush() const
{
	return mTextPen.brush();
}

//==================================================================================================

void DrawingTextEllipseItem::setFont(const QFont& font)
{
	mFont = font;
}

QFont DrawingTextEllipseItem::font() const
{
	return mFont;
}

//==================================================================================================

void DrawingTextEllipseItem::setProperties(const QHash<QString,QVariant>& properties)
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

	DrawingEllipseItem::setProperties(properties);
}

QHash<QString,QVariant> DrawingTextEllipseItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert(DrawingEllipseItem::properties());
	properties.insert("caption", mCaption);
	properties.insert("textBrush", mTextPen.brush());
	properties.insert("font", mFont);

	return properties;
}

//==================================================================================================

QRectF DrawingTextEllipseItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingTextEllipseItem::shape() const
{
	return mShape;
}

bool DrawingTextEllipseItem::isValid() const
{
	return (DrawingEllipseItem::isValid() || !mCaption.isEmpty());
}

//==================================================================================================

void DrawingTextEllipseItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		// Draw ellipse
		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawEllipse(ellipse());

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

void DrawingTextEllipseItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		DrawingEllipseItem::writeToXml(xml);

		writeFontToXml(xml, "font", mFont);
		writeBrushToXml(xml, "text", mTextPen.brush());
		xml->writeCharacters(mCaption);
	}
}

void DrawingTextEllipseItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		DrawingEllipseItem::readFromXml(xml);

		QXmlStreamAttributes attr = xml->attributes();

		readFontFromXml(xml, "font", mFont);

		QBrush brush = Qt::black;
		readBrushFromXml(xml, "text", brush);
		mTextPen.setBrush(brush);

		if (xml->readNext() == QXmlStreamReader::Characters)
			mCaption = xml->text().toString();

		updateItemGeometry();
	}
}

//==================================================================================================

void DrawingTextEllipseItem::updateItemGeometry()
{
	DrawingEllipseItem::updateItemGeometry();

	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mRectBoundingRect = QRectF();
	mRectShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = pen().widthF() / 2;
		QRectF normalizedRect = ellipse().normalized();
		QPainterPath drawPath;

		// Bounding rect
		mRectBoundingRect = normalizedRect;
		mRectBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		mBoundingRect = mRectBoundingRect;
		mBoundingRect = mBoundingRect.united(mTextBoundingRect);

		// Shape
		drawPath.addEllipse(normalizedRect);

		mRectShape = strokePath(drawPath, pen());
		if (brush().color().alpha() > 0) mRectShape.addPath(drawPath);

		mShape = mRectShape;
		mShape.addRect(mTextBoundingRect);
	}
}

void DrawingTextEllipseItem::updateTextRect(const QFont& font)
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

		mTextBoundingRect = QRectF(textLeft, textTop, textWidth, textHeight).translated(ellipse().center());

		// Bounding rect and shape
		mBoundingRect = mRectBoundingRect;
		mBoundingRect = mBoundingRect.united(mTextBoundingRect);

		mShape = mRectShape;
		mShape.addRect(mTextBoundingRect);
	}
}
