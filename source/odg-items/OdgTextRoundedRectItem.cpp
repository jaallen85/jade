// File: OdgTextRoundedRectItem.cpp
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

#include "OdgTextRoundedRectItem.h"
#include "OdgFontStyle.h"
#include <QPainter>

OdgTextRoundedRectItem::OdgTextRoundedRectItem() : OdgRoundedRectItem(),
    mCaption(), mFont(), mTextAlignment(Qt::AlignCenter), mTextPadding(0, 0), mTextBrush(QColor(0, 0, 0)), mTextRect()
{
    // Nothing more to do here.
}

//======================================================================================================================

OdgItem* OdgTextRoundedRectItem::copy() const
{
	OdgTextRoundedRectItem* textRectItem = new OdgTextRoundedRectItem();
	textRectItem->setPosition(mPosition);
	textRectItem->setRotation(mRotation);
	textRectItem->setFlipped(mFlipped);
	textRectItem->setRect(mRect);
	textRectItem->setCornerRadius(mCornerRadius);
	textRectItem->setBrush(mBrush);
	textRectItem->setPen(mPen);
	textRectItem->setCaption(mCaption);
	textRectItem->setFont(mFont);
	textRectItem->setTextAlignment(mTextAlignment);
	textRectItem->setTextPadding(mTextPadding);
	textRectItem->setTextBrush(mTextBrush);
	return textRectItem;
}

//======================================================================================================================

void OdgTextRoundedRectItem::setCaption(const QString& caption)
{
    mCaption = caption;
}

QString OdgTextRoundedRectItem::caption() const
{
    return mCaption;
}

//======================================================================================================================

void OdgTextRoundedRectItem::setFont(const QFont& font)
{
    if (font.pointSizeF() > 0) mFont = font;
}

void OdgTextRoundedRectItem::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
}

void OdgTextRoundedRectItem::setTextPadding(const QSizeF& padding)
{
    if (padding.width() >= 0 && padding.height() >= 0) mTextPadding = padding;
}

void OdgTextRoundedRectItem::setTextBrush(const QBrush& brush)
{
    mTextBrush = brush;
}

QFont OdgTextRoundedRectItem::font() const
{
    return mFont;
}

Qt::Alignment OdgTextRoundedRectItem::textAlignment() const
{
    return mTextAlignment;
}

QSizeF OdgTextRoundedRectItem::textPadding() const
{
    return mTextPadding;
}

QBrush OdgTextRoundedRectItem::textBrush() const
{
    return mTextBrush;
}

//======================================================================================================================

void OdgTextRoundedRectItem::setProperty(const QString &name, const QVariant &value)
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
	else OdgRoundedRectItem::setProperty(name, value);
}

QVariant OdgTextRoundedRectItem::property(const QString &name) const
{
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
	return OdgRoundedRectItem::property(name);
}

//======================================================================================================================

QRectF OdgTextRoundedRectItem::boundingRect() const
{
    QRectF textRect = mTextRect;
    if (textRect.isNull())
    {
        QRectF scaledTextRect;
        double scaleFactor = 1;
        calculateTextRect(calculateAnchorPoint(mTextAlignment), mFont, mTextAlignment, mTextPadding, mCaption,
                          textRect, scaledTextRect, scaleFactor);
    }

    return OdgRoundedRectItem::boundingRect().united(textRect);
}

QPainterPath OdgTextRoundedRectItem::shape() const
{
    QRectF textRect = mTextRect;
    if (textRect.isNull())
    {
        QRectF scaledTextRect;
        double scaleFactor = 1;
        calculateTextRect(calculateAnchorPoint(mTextAlignment), mFont, mTextAlignment, mTextPadding, mCaption,
                          textRect, scaledTextRect, scaleFactor);
    }

    QPainterPath textPath;
    textPath.addRect(textRect);
    return OdgRoundedRectItem::shape().united(textPath);
}

bool OdgTextRoundedRectItem::isValid() const
{
    return (OdgRoundedRectItem::isValid() || !mCaption.isEmpty());
}

//======================================================================================================================

void OdgTextRoundedRectItem::paint(QPainter& painter)
{
    OdgRoundedRectItem::paint(painter);

    mTextRect = drawText(painter, calculateAnchorPoint(mTextAlignment), mFont, mTextAlignment, mTextPadding,
                         mTextBrush, mCaption);
}

//======================================================================================================================

void OdgTextRoundedRectItem::scaleBy(double scale)
{
    OdgRoundedRectItem::scaleBy(scale);

    mFont.setPointSizeF(mFont.pointSizeF() * scale);
    mTextPadding.setWidth(mTextPadding.width() * scale);
    mTextPadding.setHeight(mTextPadding.height() * scale);
}

//======================================================================================================================

void OdgTextRoundedRectItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
	OdgRoundedRectItem::placeCreateEvent(contentRect, grid);
	setCaption("Label");
}

//======================================================================================================================

QPointF OdgTextRoundedRectItem::calculateAnchorPoint(Qt::Alignment alignment) const
{
    const QRectF rect = this->rect();
    const QPointF center = rect.center();

    double anchorX = 0, anchorY = 0;

    if (alignment & Qt::AlignLeft)
        anchorX = rect.left();
    else if (alignment & Qt::AlignRight)
        anchorX = rect.right();
    else
        anchorX = center.x();

    if (alignment & Qt::AlignTop)
        anchorY = rect.top();
    else if (alignment & Qt::AlignBottom)
        anchorY = rect.bottom();
    else
        anchorY = center.y();

    return QPointF(anchorX, anchorY);
}
