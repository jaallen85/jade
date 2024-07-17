// File: OdgStyle.cpp
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

#include "OdgStyle.h"

OdgStyle::OdgStyle(Odg::Units units, bool defaultStyle) :
    mName(), mParent(nullptr),
    mPenStyle(Qt::SolidLine), mPenWidth(0.0), mPenColor(0, 0, 0),
    mPenCapStyle(Qt::RoundCap), mPenJoinStyle(Qt::RoundJoin), mBrushColor(255, 255, 255),
    mPenStyleValid(defaultStyle), mPenWidthValid(defaultStyle), mPenColorValid(defaultStyle),
    mPenCapStyleValid(defaultStyle), mPenJoinStyleValid(defaultStyle), mBrushColorValid(defaultStyle),
    mStartMarkerStyle(Odg::NoMarker), mStartMarkerSize(0.0), mEndMarkerStyle(Odg::NoMarker), mEndMarkerSize(0.0),
    mStartMarkerStyleValid(defaultStyle), mStartMarkerSizeValid(defaultStyle),
    mEndMarkerStyleValid(defaultStyle), mEndMarkerSizeValid(defaultStyle),
    mFontFamily("Aptos"), mFontSize(0.0), mFontStyle(),
    mTextAlignment(Qt::AlignCenter), mTextPadding(0.0, 0.0), mTextColor(0, 0, 0),
    mFontFamilyValid(defaultStyle), mFontSizeValid(defaultStyle), mFontStyleValid(defaultStyle),
    mTextAlignmentValid(defaultStyle), mTextPaddingValid(defaultStyle), mTextColorValid(defaultStyle)
{
    switch (units)
    {
    case Odg::UnitsInches:
        mPenWidth = 0.01;
        mStartMarkerSize = 0.04;
        mEndMarkerSize = 0.04;
        mFontSize = 0.1;
        break;
    default:    // Odg::UnitsMillimeters
        mPenWidth = 0.2;
        mStartMarkerSize = 0.8;
        mEndMarkerSize = 0.8;
        mFontSize = 2.0;
        break;
    }
}

//======================================================================================================================

void OdgStyle::setName(const QString& name)
{
    mName = name;
}

void OdgStyle::setParent(OdgStyle* parent)
{
    mParent = parent;
}

QString OdgStyle::name() const
{
    return mName;
}

OdgStyle* OdgStyle::parent() const
{
    return mParent;
}

//======================================================================================================================

void OdgStyle::setPenStyle(Qt::PenStyle style)
{
    mPenStyle = style;
    mPenStyleValid = true;
}

void OdgStyle::setPenWidth(double width)
{
    if (width >= 0)
    {
        mPenWidth = width;
        mPenWidthValid = true;
    }
}

void OdgStyle::setPenColor(const QColor& color)
{
    mPenColor = color;
    mPenColorValid = true;
}

void OdgStyle::setPenCapStyle(Qt::PenCapStyle style)
{
    mPenCapStyle = style;
    mPenCapStyleValid = true;
}

void OdgStyle::setPenJoinStyle(Qt::PenJoinStyle style)
{
    mPenJoinStyle = style;
    mPenJoinStyleValid = true;
}

void OdgStyle::setBrushColor(const QColor& color)
{
    mBrushColor = color;
    mBrushColorValid = true;
}

void OdgStyle::unsetPenStyle()
{
    mPenStyleValid = false;
}

void OdgStyle::unsetPenWidth()
{
    mPenWidthValid = false;
}

void OdgStyle::unsetPenColor()
{
    mPenColorValid = false;
}

void OdgStyle::unsetPenCapStyle()
{
    mPenCapStyleValid = false;
}

void OdgStyle::unsetPenJoinStyle()
{
    mPenJoinStyleValid = false;
}

void OdgStyle::unsetBrushColor()
{
    mBrushColorValid = false;
}

Qt::PenStyle OdgStyle::penStyle() const
{
    return mPenStyle;
}

double OdgStyle::penWidth() const
{
    return mPenWidth;
}

QColor OdgStyle::penColor() const
{
    return mPenColor;
}

Qt::PenCapStyle OdgStyle::penCapStyle() const
{
    return mPenCapStyle;
}

Qt::PenJoinStyle OdgStyle::penJoinStyle() const
{
    return mPenJoinStyle;
}

QColor OdgStyle::brushColor() const
{
    return mBrushColor;
}

bool OdgStyle::isPenStyleValid() const
{
    return mPenStyleValid;
}

bool OdgStyle::isPenWidthValid() const
{
    return mPenWidthValid;
}

bool OdgStyle::isPenColorValid() const
{
    return mPenColorValid;
}

bool OdgStyle::isPenCapStyleValid() const
{
    return mPenCapStyleValid;
}

bool OdgStyle::isPenJoinStyleValid() const
{
    return mPenJoinStyleValid;
}

bool OdgStyle::isBrushColorValid() const
{
    return mBrushColorValid;
}

//======================================================================================================================

void OdgStyle::setStartMarkerStyle(Odg::MarkerStyle style)
{
    mStartMarkerStyle = style;
    mStartMarkerStyleValid = true;
}

void OdgStyle::setStartMarkerSize(double size)
{
    if (size >= 0)
    {
        mStartMarkerSize = size;
        mStartMarkerSizeValid = true;
    }
}

void OdgStyle::setEndMarkerStyle(Odg::MarkerStyle style)
{
    mEndMarkerStyle = style;
    mEndMarkerStyleValid = true;
}

void OdgStyle::setEndMarkerSize(double size)
{
    if (size >= 0)
    {
        mEndMarkerSize = size;
        mEndMarkerSizeValid = true;
    }
}

void OdgStyle::unsetStartMarkerStyle()
{
    mStartMarkerStyleValid = false;
}

void OdgStyle::unsetStartMarkerSize()
{
    mStartMarkerSizeValid = false;
}

void OdgStyle::unsetEndMarkerStyle()
{
    mEndMarkerStyleValid = false;
}

void OdgStyle::unsetEndMarkerSize()
{
    mEndMarkerSizeValid = false;
}

Odg::MarkerStyle OdgStyle::startMarkerStyle() const
{
    return mStartMarkerStyle;
}

double OdgStyle::startMarkerSize() const
{
    return mStartMarkerSize;
}

Odg::MarkerStyle OdgStyle::endMarkerStyle() const
{
    return mEndMarkerStyle;
}

double OdgStyle::endMarkerSize() const
{
    return mEndMarkerSize;
}

bool OdgStyle::isStartMarkerStyleValid() const
{
    return mStartMarkerStyleValid;
}

bool OdgStyle::isStartMarkerSizeValid() const
{
    return mStartMarkerSizeValid;
}

bool OdgStyle::isEndMarkerStyleValid() const
{
    return mEndMarkerStyleValid;
}

bool OdgStyle::isEndMarkerSizeValid() const
{
    return mEndMarkerSizeValid;
}

//======================================================================================================================

void OdgStyle::setFontFamily(const QString& family)
{
    mFontFamily = family;
    mFontFamilyValid = true;
}

void OdgStyle::setFontSize(double size)
{
    if (size >= 0)
    {
        mFontSize = size;
        mFontSizeValid = true;
    }
}

void OdgStyle::setFontStyle(const OdgFontStyle& style)
{
    mFontStyle = style;
    mFontStyleValid = true;
}

void OdgStyle::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
    mTextAlignmentValid = true;
}

void OdgStyle::setTextPadding(const QSizeF& padding)
{
    mTextPadding = padding;
    mTextPaddingValid = true;
}

void OdgStyle::setTextColor(const QColor& color)
{
    mTextColor = color;
    mTextColorValid = true;
}

void OdgStyle::unsetFontFamily()
{
    mFontFamilyValid = false;
}

void OdgStyle::unsetFontSize()
{
    mFontSizeValid = false;
}

void OdgStyle::unsetFontStyle()
{
    mFontStyleValid = false;
}

void OdgStyle::unsetTextAlignment()
{
    mTextAlignmentValid = false;
}

void OdgStyle::unsetTextPadding()
{
    mTextPaddingValid = false;
}

void OdgStyle::unsetTextColor()
{
    mTextColorValid = false;
}

QString OdgStyle::fontFamily() const
{
    return mFontFamily;
}

double OdgStyle::fontSize() const
{
    return mFontSize;
}

OdgFontStyle OdgStyle::fontStyle() const
{
    return mFontStyle;
}

Qt::Alignment OdgStyle::textAlignment() const
{
    return mTextAlignment;
}

QSizeF OdgStyle::textPadding() const
{
    return mTextPadding;
}

QColor OdgStyle::textColor() const
{
    return mTextColor;
}

bool OdgStyle::isFontFamilyValid() const
{
    return mFontFamilyValid;
}

bool OdgStyle::isFontSizeValid() const
{
    return mFontSizeValid;
}

bool OdgStyle::isFontStyleValid() const
{
    return mFontStyleValid;
}

bool OdgStyle::isTextAlignmentValid() const
{
    return mTextAlignmentValid;
}

bool OdgStyle::isTextPaddingValid() const
{
    return mTextPaddingValid;
}

bool OdgStyle::isTextColorValid() const
{
    return mTextColorValid;
}

//======================================================================================================================

void OdgStyle::clear()
{
    mName.clear();
    mParent = nullptr;

    mPenStyleValid = false;
    mPenWidthValid = false;
    mPenColorValid = false;
    mPenCapStyleValid = false;
    mPenJoinStyleValid = false;
    mBrushColorValid = false;

    mStartMarkerStyleValid = false;
    mStartMarkerSizeValid = false;
    mEndMarkerStyleValid = false;
    mEndMarkerSizeValid = false;

    mFontFamilyValid = false;
    mFontSizeValid = false;
    mFontStyleValid = false;
    mTextAlignmentValid = false;
    mTextPaddingValid = false;
    mTextColorValid = false;
}

//======================================================================================================================

void OdgStyle::setPenStyleIfNeeded(Qt::PenStyle style)
{
    if (mParent == nullptr) setPenStyle(style);
    else if (mParent->lookupPenStyle() == style) unsetPenStyle();
    else setPenStyle(style);
}

void OdgStyle::setPenWidthIfNeeded(double width)
{
    if (mParent == nullptr) setPenWidth(width);
    else if (mParent->lookupPenWidth() == width) unsetPenWidth();
    else setPenWidth(width);
}

void OdgStyle::setPenColorIfNeeded(const QColor& color)
{
    if (mParent == nullptr) setPenColor(color);
    else if (mParent->lookupPenColor() == color) unsetPenColor();
    else setPenColor(color);
}

void OdgStyle::setPenCapStyleIfNeeded(Qt::PenCapStyle style)
{
    if (mParent == nullptr) setPenCapStyle(style);
    else if (mParent->lookupPenCapStyle() == style) unsetPenCapStyle();
    else setPenCapStyle(style);
}

void OdgStyle::setPenJoinStyleIfNeeded(Qt::PenJoinStyle style)
{
    if (mParent == nullptr) setPenJoinStyle(style);
    else if (mParent->lookupPenJoinStyle() == style) unsetPenJoinStyle();
    else setPenJoinStyle(style);
}

void OdgStyle::setBrushColorIfNeeded(const QColor& color)
{
    if (mParent == nullptr) setBrushColor(color);
    else if (mParent->lookupBrushColor() == color) unsetBrushColor();
    else setBrushColor(color);
}

//======================================================================================================================

void OdgStyle::setStartMarkerStyleIfNeeded(Odg::MarkerStyle style)
{
    if (mParent == nullptr) setStartMarkerStyle(style);
    else if (mParent->lookupStartMarkerStyle() == style) unsetStartMarkerStyle();
    else setStartMarkerStyle(style);
}

void OdgStyle::setStartMarkerSizeIfNeeded(double size)
{
    if (mParent == nullptr) setStartMarkerSize(size);
    else if (mParent->lookupStartMarkerSize() == size) unsetStartMarkerSize();
    else setStartMarkerSize(size);
}

void OdgStyle::setEndMarkerStyleIfNeeded(Odg::MarkerStyle style)
{
    if (mParent == nullptr) setEndMarkerStyle(style);
    else if (mParent->lookupEndMarkerStyle() == style) unsetEndMarkerStyle();
    else setEndMarkerStyle(style);
}

void OdgStyle::setEndMarkerSizeIfNeeded(double size)
{
    if (mParent == nullptr) setEndMarkerSize(size);
    else if (mParent->lookupEndMarkerSize() == size) unsetEndMarkerSize();
    else setEndMarkerSize(size);
}

//======================================================================================================================

void OdgStyle::setFontFamilyIfNeeded(const QString& family)
{
    if (mParent == nullptr) setFontFamily(family);
    else if (mParent->lookupFontFamily() == family) unsetFontFamily();
    else setFontFamily(family);
}

void OdgStyle::setFontSizeIfNeeded(double size)
{
    if (mParent == nullptr) setFontSize(size);
    else if (mParent->lookupFontSize() == size) unsetFontSize();
    else setFontSize(size);
}

void OdgStyle::setFontStyleIfNeeded(const OdgFontStyle& style)
{
    if (mParent == nullptr) setFontStyle(style);
    else if (mParent->lookupFontStyle() == style) unsetFontStyle();
    else setFontStyle(style);
}

void OdgStyle::setTextAlignmentIfNeeded(Qt::Alignment alignment)
{
    if (mParent == nullptr) setTextAlignment(alignment);
    else if (mParent->lookupTextAlignment() == alignment) unsetTextAlignment();
    else setTextAlignment(alignment);
}

void OdgStyle::setTextPaddingIfNeeded(const QSizeF& padding)
{
    if (mParent == nullptr) setTextPadding(padding);
    else if (mParent->lookupTextPadding() == padding) unsetTextPadding();
    else setTextPadding(padding);
}

void OdgStyle::setTextColorIfNeeded(const QColor& color)
{
    if (mParent == nullptr) setTextColor(color);
    else if (mParent->lookupTextColor() == color) unsetTextColor();
    else setTextColor(color);
}

//======================================================================================================================

QPen OdgStyle::lookupPen() const
{
    return QPen(QBrush(lookupPenColor()), lookupPenWidth(), lookupPenStyle(), lookupPenCapStyle(), lookupPenJoinStyle());
}

QBrush OdgStyle::lookupBrush() const
{
    return QBrush(lookupBrushColor());
}

Qt::PenStyle OdgStyle::lookupPenStyle() const
{
    if (mPenStyleValid)
        return mPenStyle;
    if (mParent)
        return mParent->lookupPenStyle();
    return mPenStyle;
}

double OdgStyle::lookupPenWidth() const
{
    if (mPenWidthValid)
        return mPenWidth;
    if (mParent)
        return mParent->lookupPenWidth();
    return mPenWidth;
}

QColor OdgStyle::lookupPenColor() const
{
    if (mPenColorValid)
        return mPenColor;
    if (mParent)
        return mParent->lookupPenColor();
    return mPenColor;
}

Qt::PenCapStyle OdgStyle::lookupPenCapStyle() const
{
    if (mPenCapStyleValid)
        return mPenCapStyle;
    if (mParent)
        return mParent->lookupPenCapStyle();
    return mPenCapStyle;
}

Qt::PenJoinStyle OdgStyle::lookupPenJoinStyle() const
{
    if (mPenJoinStyleValid)
        return mPenJoinStyle;
    if (mParent)
        return mParent->lookupPenJoinStyle();
    return mPenJoinStyle;
}

QColor OdgStyle::lookupBrushColor() const
{
    if (mBrushColorValid)
        return mBrushColor;
    if (mParent)
        return mParent->lookupBrushColor();
    return mBrushColor;
}

//======================================================================================================================

OdgMarker OdgStyle::lookupStartMarker() const
{
    return OdgMarker(lookupStartMarkerStyle(), lookupStartMarkerSize());
}

OdgMarker OdgStyle::lookupEndMarker() const
{
    return OdgMarker(lookupEndMarkerStyle(), lookupEndMarkerSize());
}

Odg::MarkerStyle OdgStyle::lookupStartMarkerStyle() const
{
    if (mStartMarkerStyleValid)
        return mStartMarkerStyle;
    if (mParent)
        return mParent->lookupStartMarkerStyle();
    return mStartMarkerStyle;
}

double OdgStyle::lookupStartMarkerSize() const
{
    if (mStartMarkerSizeValid)
        return mStartMarkerSize;
    if (mParent)
        return mParent->lookupStartMarkerSize();
    return mStartMarkerSize;
}

Odg::MarkerStyle OdgStyle::lookupEndMarkerStyle() const
{
    if (mEndMarkerStyleValid)
        return mEndMarkerStyle;
    if (mParent)
        return mParent->lookupEndMarkerStyle();
    return mEndMarkerStyle;
}

double OdgStyle::lookupEndMarkerSize() const
{
    if (mEndMarkerSizeValid)
        return mEndMarkerSize;
    if (mParent)
        return mParent->lookupEndMarkerSize();
    return mEndMarkerSize;
}

//======================================================================================================================

QFont OdgStyle::lookupFont() const
{
    QFont font(lookupFontFamily());
    font.setPointSizeF(lookupFontSize());

    OdgFontStyle fontStyle = lookupFontStyle();
    font.setBold(fontStyle.bold());
    font.setItalic(fontStyle.italic());
    font.setUnderline(fontStyle.underline());
    font.setStrikeOut(fontStyle.strikeOut());

    return font;
}

QBrush OdgStyle::lookupTextBrush() const
{
    return QBrush(lookupTextColor());
}

QString OdgStyle::lookupFontFamily() const
{
    if (mFontFamilyValid)
        return mFontFamily;
    if (mParent)
        return mParent->lookupFontFamily();
    return mFontFamily;
}

double OdgStyle::lookupFontSize() const
{
    if (mFontSizeValid)
        return mFontSize;
    if (mParent)
        return mParent->lookupFontSize();
    return mFontSize;
}

OdgFontStyle OdgStyle::lookupFontStyle() const
{
    if (mFontStyleValid)
        return mFontStyle;
    if (mParent)
        return mParent->lookupFontStyle();
    return mFontStyle;
}

Qt::Alignment OdgStyle::lookupTextAlignment() const
{
    if (mTextAlignmentValid)
        return mTextAlignment;
    if (mParent)
        return mParent->lookupTextAlignment();
    return mTextAlignment;
}

QSizeF OdgStyle::lookupTextPadding() const
{
    if (mTextPaddingValid)
        return mTextPadding;
    if (mParent)
        return mParent->lookupTextPadding();
    return mTextPadding;
}

QColor OdgStyle::lookupTextColor() const
{
    if (mTextColorValid)
        return mTextColor;
    if (mParent)
        return mParent->lookupTextColor();
    return mTextColor;
}

//======================================================================================================================

bool OdgStyle::isEquivalentTo(OdgStyle* other) const
{
    if (other)
    {
        return (mParent == other->mParent &&
                mPenStyleValid == other->mPenStyleValid &&
                mPenWidthValid == other->mPenWidthValid &&
                mPenColorValid == other->mPenColorValid &&
                mPenCapStyleValid == other->mPenCapStyleValid &&
                mPenJoinStyleValid == other->mPenJoinStyleValid &&
                mBrushColorValid == other->mBrushColorValid &&
                mStartMarkerStyleValid == other->mStartMarkerStyleValid &&
                mStartMarkerSizeValid == other->mStartMarkerSizeValid &&
                mEndMarkerStyleValid == other->mEndMarkerStyleValid &&
                mEndMarkerSizeValid == other->mEndMarkerSizeValid &&
                mFontFamilyValid == other->mFontFamilyValid &&
                mFontSizeValid == other->mFontSizeValid &&
                mFontStyleValid == other->mFontStyleValid &&
                mTextAlignmentValid == other->mTextAlignmentValid &&
                mTextPaddingValid == other->mTextPaddingValid &&
                mTextColorValid == other->mTextColorValid &&
                (!mPenStyleValid || mPenStyle == other->mPenStyle) &&
                (!mPenWidthValid || mPenWidth == other->mPenWidth) &&
                (!mPenColorValid || mPenColor == other->mPenColor) &&
                (!mPenCapStyleValid || mPenCapStyle == other->mPenCapStyle) &&
                (!mPenJoinStyleValid || mPenJoinStyle == other->mPenJoinStyle) &&
                (!mBrushColorValid || mBrushColor == other->mBrushColor) &&
                (!mStartMarkerStyleValid || mStartMarkerStyle == other->mStartMarkerStyle) &&
                (!mStartMarkerSizeValid || mStartMarkerSize == other->mStartMarkerSize) &&
                (!mEndMarkerStyleValid || mEndMarkerStyle == other->mEndMarkerStyle) &&
                (!mEndMarkerSizeValid || mEndMarkerSize == other->mEndMarkerSize) &&
                (!mFontFamilyValid || mFontFamily == other->mFontFamily) &&
                (!mFontSizeValid || mFontSize == other->mFontSize) &&
                (!mFontStyleValid || mFontStyle == other->mFontStyle) &&
                (!mTextAlignmentValid || mTextAlignment == other->mTextAlignment) &&
                (!mTextPaddingValid || mTextPadding == other->mTextPadding) &&
                (!mTextColorValid || mTextColor == other->mTextColor));
    }

    return false;
}
