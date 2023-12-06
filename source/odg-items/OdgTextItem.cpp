// File: OdgTextItem.cpp
// Copyright (C) 2023  Jason Allen
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

#include "OdgTextItem.h"
#include "OdgControlPoint.h"
#include "OdgFontStyle.h"
#include <QPainter>

OdgTextItem::OdgTextItem() : OdgItem(),
    mCaption(), mFont(), mTextAlignment(Qt::AlignCenter), mTextPadding(0, 0), mTextBrush(QColor(0, 0, 0)), mTextRect()
{
    addControlPoint(new OdgControlPoint(QPointF(0, 0), false));
}

//======================================================================================================================

OdgItem* OdgTextItem::copy() const
{
	OdgTextItem* textItem = new OdgTextItem();
	textItem->setPosition(mPosition);
	textItem->setRotation(mRotation);
	textItem->setFlipped(mFlipped);
	textItem->setCaption(mCaption);
	textItem->setFont(mFont);
	textItem->setTextAlignment(mTextAlignment);
	textItem->setTextPadding(mTextPadding);
	textItem->setTextBrush(mTextBrush);
	return textItem;
}

//======================================================================================================================

void OdgTextItem::setCaption(const QString& caption)
{
    mCaption = caption;
}

QString OdgTextItem::caption() const
{
    return mCaption;
}

//======================================================================================================================

void OdgTextItem::setFont(const QFont& font)
{
    if (font.pointSizeF() > 0) mFont = font;
}

void OdgTextItem::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
}

void OdgTextItem::setTextPadding(const QSizeF& padding)
{
    if (padding.width() >= 0 && padding.height() >= 0) mTextPadding = padding;
}

void OdgTextItem::setTextBrush(const QBrush& brush)
{
    mTextBrush = brush;
}

QFont OdgTextItem::font() const
{
    return mFont;
}

Qt::Alignment OdgTextItem::textAlignment() const
{
    return mTextAlignment;
}

QSizeF OdgTextItem::textPadding() const
{
    return mTextPadding;
}

QBrush OdgTextItem::textBrush() const
{
    return mTextBrush;
}

//======================================================================================================================

void OdgTextItem::setProperty(const QString &name, const QVariant &value)
{
	if (name == "caption" && value.canConvert<QString>())
	{
		setCaption(value.toString());
	}
	else if (name == "font" && value.canConvert<QFont>())
	{
		setFont(value.value<QFont>());
	}
	else if (name == "fontFamily" && value.canConvert<QString>())
	{
		QFont font = mFont;
		font.setFamily(value.toString());
		setFont(font);
	}
	else if (name == "fontSize" && value.canConvert<double>())
	{
		QFont font = mFont;
		font.setPointSizeF(value.toDouble());
		setFont(font);
	}
	else if (name == "fontStyle" && value.canConvert<OdgFontStyle>())
	{
		QFont font = mFont;
		OdgFontStyle fontStyle = value.value<OdgFontStyle>();
		font.setBold(fontStyle.bold());
		font.setItalic(fontStyle.italic());
		font.setUnderline(fontStyle.underline());
		font.setStrikeOut(fontStyle.strikeOut());
		setFont(font);
	}
	else if (name == "textAlignment" && value.canConvert<int>())
	{
		setTextAlignment(static_cast<Qt::Alignment>(value.toInt()));
	}
	else if (name == "textPadding" && value.canConvert<QSizeF>())
	{
		setTextPadding(value.value<QSizeF>());
	}
	else if (name == "textBrush" && value.canConvert<QBrush>())
	{
		setTextBrush(value.value<QBrush>());
	}
	else if (name == "textColor" && value.canConvert<QColor>())
	{
		setTextBrush(QBrush(value.value<QColor>()));
	}
}

QVariant OdgTextItem::property(const QString &name) const
{
	if (name == "position") return mPosition;
	if (name == "caption") return mCaption;
	if (name == "font") return mFont;
	if (name == "fontFamily") return mFont.family();
	if (name == "fontSize") return mFont.pointSizeF();
	if (name == "fontStyle")
	{
		return QVariant::fromValue<OdgFontStyle>(
			OdgFontStyle(mFont.bold(), mFont.italic(), mFont.underline(), mFont.strikeOut()));
	}
	if (name == "textAlignment") return static_cast<int>(mTextAlignment);
	if (name == "textPadding") return mTextPadding;
	if (name == "textBrush") return mTextBrush;
	if (name == "textColor") return mTextBrush.color();
	return QVariant();
}

//======================================================================================================================

QRectF OdgTextItem::boundingRect() const
{
    QRectF textRect = mTextRect;
    if (textRect.isNull())
    {
        QRectF scaledTextRect;
        double scaleFactor = 1;
        calculateTextRect(QPointF(0, 0), mFont, mTextAlignment, mTextPadding, mCaption,
                          textRect, scaledTextRect, scaleFactor);
    }

    return textRect;
}

QPainterPath OdgTextItem::shape() const
{
    QPainterPath shape;
    shape.addRect(boundingRect());
    return shape;
}

bool OdgTextItem::isValid() const
{
    return (!mCaption.isEmpty());
}

//======================================================================================================================

void OdgTextItem::paint(QPainter& painter)
{
    mTextRect = drawText(painter, QPointF(0, 0), mFont, mTextAlignment, mTextPadding, mTextBrush, mCaption);
}

//======================================================================================================================

void OdgTextItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);

    mFont.setPointSizeF(mFont.pointSizeF() * scale);
    mTextPadding.setWidth(mTextPadding.width() * scale);
    mTextPadding.setHeight(mTextPadding.height() * scale);
}

//======================================================================================================================

void OdgTextItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
	setCaption("Label");
}
