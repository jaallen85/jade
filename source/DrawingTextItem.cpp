/* DrawingTextItem.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DrawingTextItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"

DrawingTextItem::DrawingTextItem() : DrawingItem()
{
	setCaption("Label");
	setTextColor(QColor(0, 0, 0));
	setFontFamily("Arial");
	setFontSize(100);
	setFontBold(false);
	setFontItalic(false);
	setFontUnderline(false);
	setFontOverline(false);
	setFontStrikeOut(false);
	setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);

	addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::NoFlags));
}

DrawingTextItem::DrawingTextItem(const DrawingTextItem& item) : DrawingItem(item) { }

DrawingTextItem::~DrawingTextItem() { }

//==================================================================================================

DrawingItem* DrawingTextItem::copy() const
{
	return new DrawingTextItem(*this);
}

//==================================================================================================

void DrawingTextItem::setCaption(const QString& caption)
{
	setProperty("Caption", caption);
}

QString DrawingTextItem::caption() const
{
	return property("Caption").toString();
}

//==================================================================================================

void DrawingTextItem::setTextColor(const QColor& color)
{
	setProperty("Text Color", color);
}

QColor DrawingTextItem::textColor() const
{
	return hasProperty("Text Color") ? property("Text Color").value<QColor>() : QColor(0, 0, 0);
}

//==================================================================================================

void DrawingTextItem::setFont(const QFont& font)
{
	setFontFamily(font.family());
	setFontSize(font.pointSizeF());
	setFontBold(font.bold());
	setFontItalic(font.italic());
	setFontUnderline(font.underline());
	setFontOverline(font.overline());
	setFontStrikeOut(font.strikeOut());
}

void DrawingTextItem::setFontFamily(const QString& family)
{
	setProperty("Font Family", family);
}

void DrawingTextItem::setFontSize(qreal size)
{
	setProperty("Font Size", size);
}

void DrawingTextItem::setFontBold(bool bold)
{
	setProperty("Font Bold", bold);
}

void DrawingTextItem::setFontItalic(bool italic)
{
	setProperty("Font Italic", italic);
}

void DrawingTextItem::setFontUnderline(bool underline)
{
	setProperty("Font Underline", underline);
}

void DrawingTextItem::setFontOverline(bool overline)
{
	setProperty("Font Overline", overline);
}

void DrawingTextItem::setFontStrikeOut(bool strikeOut)
{
	setProperty("Font Strike-Out", strikeOut);
}

QFont DrawingTextItem::font() const
{
	QFont font;

	font.setFamily(fontFamily());
	font.setPointSizeF(fontSize());
	font.setBold(isFontBold());
	font.setItalic(isFontItalic());
	font.setUnderline(isFontUnderline());
	font.setOverline(isFontOverline());
	font.setStrikeOut(isFontStrikeOut());

	return font;
}

QString DrawingTextItem::fontFamily() const
{
	return hasProperty("Font Family") ? property("Font Family").toString() : "Arial";
}

qreal DrawingTextItem::fontSize() const
{
	return hasProperty("Font Family") ? property("Font Size").toDouble() : 0;
}

bool DrawingTextItem::isFontBold() const
{
	return hasProperty("Font Family") ? property("Font Bold").toBool() : false;
}

bool DrawingTextItem::isFontItalic() const
{
	return hasProperty("Font Family") ? property("Font Italic").toBool() : false;
}

bool DrawingTextItem::isFontUnderline() const
{
	return hasProperty("Font Family") ? property("Font Underline").toBool() : false;
}

bool DrawingTextItem::isFontOverline() const
{
	return hasProperty("Font Family") ? property("Font Overline").toBool() : false;
}

bool DrawingTextItem::isFontStrikeOut() const
{
	return hasProperty("Font Family") ? property("Font Strike-Out").toBool() : false;
}

//==================================================================================================

void DrawingTextItem::setTextAlignmentHorizontal(Qt::Alignment alignment)
{
	setProperty("Text Horizontal Alignment", QVariant((quint32)(alignment & Qt::AlignHorizontal_Mask)));
}

void DrawingTextItem::setTextAlignmentVertical(Qt::Alignment alignment)
{
	setProperty("Text Vertical Alignment", QVariant((quint32)(alignment & Qt::AlignVertical_Mask)));
}

void DrawingTextItem::setTextAlignment(Qt::Alignment alignment)
{
	setTextAlignmentHorizontal(alignment);
	setTextAlignmentVertical(alignment);
}

Qt::Alignment DrawingTextItem::textAlignmentHorizontal() const
{
	return hasProperty("Text Horizontal Alignment") ? (Qt::Alignment)property("Text Horizontal Alignment").toUInt() : Qt::AlignHCenter;
}

Qt::Alignment DrawingTextItem::textAlignmentVertical() const
{
	return hasProperty("Text Vertical Alignment") ? (Qt::Alignment)property("Text Vertical Alignment").toUInt() : Qt::AlignVCenter;
}

Qt::Alignment DrawingTextItem::textAlignment() const
{
	return (textAlignmentHorizontal() | textAlignmentVertical());
}

//==================================================================================================

QRectF DrawingTextItem::boundingRect() const
{
	QTextLayout textLayout;
	return buildTextLayout(textLayout);
}

QPointF DrawingTextItem::centerPos() const
{
	return QPointF(0, 0);
}

bool DrawingTextItem::isValid() const
{
	return (caption() != "");
}

//==================================================================================================

void DrawingTextItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		QTextLayout textLayout;
		QRectF textRect = buildTextLayout(textLayout);

		// Draw text
		QPen textPen(textColor());
		painter->setBrush(Qt::transparent);
		painter->setPen(textPen);
		textLayout.draw(painter, textRect.topLeft());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
}

//==================================================================================================

QRectF DrawingTextItem::buildTextLayout(QTextLayout& textLayout) const
{
	QRectF textRect;

	// Build text layout
	textLayout.setText(caption());
	textLayout.setFont(font());

	textLayout.beginLayout();
	while (textLayout.createLine().isValid());
	textLayout.endLayout();

	textRect.setSize(textLayout.boundingRect().size());

	// Determine text position
	if (textAlignment() & Qt::AlignLeft) textRect.translate(0.0, 0.0);
	else if (textAlignment() & Qt::AlignRight) textRect.translate(-textRect.width(), 0.0);
	else textRect.translate(-textRect.width() / 2, 0.0);

	if (textAlignment() & Qt::AlignBottom) textRect.translate(0.0, -textRect.height());
	else if (textAlignment() & Qt::AlignTop) textRect.translate(0.0, 0.0);
	else textRect.translate(0.0, -textRect.height() / 2);

	return textRect;
}
