// File: OdgTextEllipseItem.cpp
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

#include "OdgTextEllipseItem.h"
#include "OdgFontStyle.h"
#include <QPainter>

OdgTextEllipseItem::OdgTextEllipseItem() : OdgEllipseItem(),
    mCaption(), mFont(), mTextAlignment(Qt::AlignCenter), mTextPadding(0, 0), mTextBrush(QColor(0, 0, 0)), mTextRect()
{
    // Nothing more to do here.
}

//======================================================================================================================

OdgItem* OdgTextEllipseItem::copy() const
{
	OdgTextEllipseItem* textEllipseItem = new OdgTextEllipseItem();
	textEllipseItem->setPosition(mPosition);
	textEllipseItem->setRotation(mRotation);
	textEllipseItem->setFlipped(mFlipped);
	textEllipseItem->setRect(mRect);
	textEllipseItem->setBrush(mBrush);
	textEllipseItem->setPen(mPen);
	textEllipseItem->setCaption(mCaption);
	textEllipseItem->setFont(mFont);
	textEllipseItem->setTextAlignment(mTextAlignment);
	textEllipseItem->setTextPadding(mTextPadding);
	textEllipseItem->setTextBrush(mTextBrush);
	return textEllipseItem;
}

//======================================================================================================================

void OdgTextEllipseItem::setCaption(const QString& caption)
{
    mCaption = caption;
}

QString OdgTextEllipseItem::caption() const
{
    return mCaption;
}

//======================================================================================================================

void OdgTextEllipseItem::setFont(const QFont& font)
{
    if (font.pointSizeF() > 0) mFont = font;
}

void OdgTextEllipseItem::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
}

void OdgTextEllipseItem::setTextPadding(const QSizeF& padding)
{
    if (padding.width() >= 0 && padding.height() >= 0) mTextPadding = padding;
}

void OdgTextEllipseItem::setTextBrush(const QBrush& brush)
{
    mTextBrush = brush;
}

QFont OdgTextEllipseItem::font() const
{
    return mFont;
}

Qt::Alignment OdgTextEllipseItem::textAlignment() const
{
    return mTextAlignment;
}

QSizeF OdgTextEllipseItem::textPadding() const
{
    return mTextPadding;
}

QBrush OdgTextEllipseItem::textBrush() const
{
    return mTextBrush;
}

//======================================================================================================================

void OdgTextEllipseItem::setProperty(const QString &name, const QVariant &value)
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
	else OdgEllipseItem::setProperty(name, value);
}

QVariant OdgTextEllipseItem::property(const QString &name) const
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
	return OdgEllipseItem::property(name);
}

//======================================================================================================================

QRectF OdgTextEllipseItem::boundingRect() const
{
    QRectF textRect = mTextRect;
    if (textRect.isNull())
    {
        QRectF scaledTextRect;
        double scaleFactor = 1;
        calculateTextRect(calculateAnchorPoint(mTextAlignment), mFont, mTextAlignment, mTextPadding, mCaption,
                          textRect, scaledTextRect, scaleFactor);
    }

    return OdgEllipseItem::boundingRect().united(textRect);
}

QPainterPath OdgTextEllipseItem::shape() const
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
    return OdgEllipseItem::shape().united(textPath);
}

bool OdgTextEllipseItem::isValid() const
{
    return (OdgEllipseItem::isValid() || !mCaption.isEmpty());
}

//======================================================================================================================

void OdgTextEllipseItem::paint(QPainter& painter)
{
    OdgEllipseItem::paint(painter);

    mTextRect = drawText(painter, calculateAnchorPoint(mTextAlignment), mFont, mTextAlignment, mTextPadding,
                         mTextBrush, mCaption);
}

//======================================================================================================================

void OdgTextEllipseItem::scaleBy(double scale)
{
    OdgEllipseItem::scaleBy(scale);

    mFont.setPointSizeF(mFont.pointSizeF() * scale);
    mTextPadding.setWidth(mTextPadding.width() * scale);
    mTextPadding.setHeight(mTextPadding.height() * scale);
}

//======================================================================================================================

void OdgTextEllipseItem::placeCreateEvent(const QRectF& contentRect, double grid)
{
	OdgEllipseItem::placeCreateEvent(contentRect, grid);
	setCaption("Label");
}

//======================================================================================================================

QPointF OdgTextEllipseItem::calculateAnchorPoint(Qt::Alignment alignment) const
{
    const QRectF rect = this->rect();
    const QPointF center = rect.center();

    if ((alignment & Qt::AlignLeft) && (alignment & Qt::AlignTop))
    {
        double angle = qDegreesToRadians(-135);
        return QPointF(rect.width() / 2 * qCos(angle), rect.height() / 2 * qSin(angle));
    }
    if ((alignment & Qt::AlignHCenter) && (alignment & Qt::AlignTop))
    {
        return QPointF(center.x(), rect.top());
    }
    if ((alignment & Qt::AlignRight) && (alignment & Qt::AlignTop))
    {
        double angle = qDegreesToRadians(-45);
        return QPointF(rect.width() / 2 * qCos(angle), rect.height() / 2 * qSin(angle));
    }
    if ((alignment & Qt::AlignRight) && (alignment & Qt::AlignVCenter))
    {
        return QPointF(rect.right(), center.y());
    }
    if ((alignment & Qt::AlignRight) && (alignment & Qt::AlignBottom))
    {
        double angle = qDegreesToRadians(45);
        return QPointF(rect.width() / 2 * qCos(angle), rect.height() / 2 * qSin(angle));
    }
    if ((alignment & Qt::AlignHCenter) && (alignment & Qt::AlignBottom))
    {
        return QPointF(center.x(), rect.bottom());
    }
    if ((alignment & Qt::AlignLeft) && (alignment & Qt::AlignBottom))
    {
        double angle = qDegreesToRadians(135);
        return QPointF(rect.width() / 2 * qCos(angle), rect.height() / 2 * qSin(angle));
    }
    if ((alignment & Qt::AlignLeft) && (alignment & Qt::AlignVCenter))
    {
        return QPointF(rect.left(), center.y());
    }

    return rect.center();
}
