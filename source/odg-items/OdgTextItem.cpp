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
#include <QPainter>

OdgTextItem::OdgTextItem() : OdgItem(),
    mCaption(), mFont(), mTextAlignment(Qt::AlignCenter), mTextPadding(0, 0), mTextBrush(QColor(0, 0, 0)), mTextRect()
{
    addControlPoint(new OdgControlPoint(QPointF(0, 0), false));
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
