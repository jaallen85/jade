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
#include "OdgReader.h"

OdgStyle::OdgStyle(Odg::Units units, bool defaultStyle) :
    mName(), mParent(nullptr),
    mPenStyle(Qt::SolidLine), mPenWidth(0.0), mPenColor(0, 0, 0),
    mPenCapStyle(Qt::RoundCap), mPenJoinStyle(Qt::RoundJoin), mBrushColor(255, 255, 255),
    mPenStyleValid(defaultStyle), mPenWidthValid(defaultStyle), mPenColorValid(defaultStyle),
    mPenCapStyleValid(defaultStyle), mPenJoinStyleValid(defaultStyle), mBrushColorValid(defaultStyle),
    mStartMarkerStyle(Odg::NoMarker), mStartMarkerSize(0.0), mEndMarkerStyle(Odg::NoMarker), mEndMarkerSize(0.0),
    mStartMarkerStyleValid(defaultStyle), mStartMarkerSizeValid(defaultStyle),
    mEndMarkerStyleValid(defaultStyle), mEndMarkerSizeValid(defaultStyle),
    mFontFamily(), mFontSize(0.0), mFontStyle(),
    mTextAlignment(Qt::AlignCenter), mTextPadding(0.0, 0.0), mTextColor(0, 0, 0),
    mFontFamilyValid(defaultStyle), mFontSizeValid(defaultStyle), mFontStyleValid(defaultStyle),
    mTextAlignmentValid(defaultStyle), mTextPaddingValid(defaultStyle), mTextColorValid(defaultStyle)
{
    switch (units)
    {
    case Odg::UnitsInches:
        mPenWidth = 0.01;
        mStartMarkerSize = 0.1;
        mEndMarkerSize = 0.1;
        mFontSize = 0.1;
        break;
    default:    // Odg::UnitsMillimeters
        mPenWidth = 0.2;
        mStartMarkerSize = 2.0;
        mEndMarkerSize = 2.0;
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

void OdgStyle::readFromXml(OdgReader* xml, const QList<OdgStyle*>& styles)
{
    Q_ASSERT(xml != nullptr);

    clear();

    // Read attributes of <style:style> element
    const QXmlStreamAttributes attributes = xml->attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("style:name"))
            mName = attribute.value().toString();
        else if (attribute.qualifiedName() == QStringLiteral("style:parent-style-name"))
        {
            QString parentName = attribute.value().toString();
            for(auto& style : styles)
            {
                if (style->name() == parentName)
                {
                    mParent = style;
                    break;
                }
            }
        }
    }

    // Read <style:style> sub-elements
    while (xml->readNextStartElement())
    {
        if (xml->qualifiedName() == QStringLiteral("style:graphic-properties"))
            readGraphicProperties(xml);
        else if (xml->qualifiedName() == QStringLiteral("style:paragraph-properties"))
            readParagraphProperties(xml);
        else if (xml->qualifiedName() == QStringLiteral("style:text-properties"))
            readTextProperties(xml);
        else
            xml->skipCurrentElement();
    }
}

//======================================================================================================================

void OdgStyle::readGraphicProperties(OdgReader* xml)
{
    // Read attributes of <style:graphic-properties> element
    const QXmlStreamAttributes attributes = xml->attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("draw:stroke"))
        {
            if (attribute.value() == QStringLiteral("solid"))
                setPenStyle(Qt::SolidLine);
            else if (attribute.value() == QStringLiteral("dash"))
                setPenStyle(Qt::DashLine);
            else
                setPenStyle(Qt::NoPen);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:stroke-dash"))
        {
            if (attribute.value() == QStringLiteral("Dash_20__28_Rounded_29_"))
                setPenStyle(Qt::DashLine);
            else if (attribute.value() == QStringLiteral("Dot_20__28_Rounded_29_"))
                setPenStyle(Qt::DotLine);
            else if (attribute.value() == QStringLiteral("Dash_20_Dot_20__28_Rounded_29_"))
                setPenStyle(Qt::DashDotLine);
            else if (attribute.value() == QStringLiteral("Dash_20_Dot_20_Dot_20__28_Rounded_29_"))
                setPenStyle(Qt::DashDotDotLine);
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-width"))
        {
            setPenWidth(xml->lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-color"))
        {
            setPenColor(xml->colorFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-opacity"))
        {
            mPenColor.setAlphaF(xml->percentFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-linecap"))
        {
            if (attribute.value() == QStringLiteral("butt"))
                setPenCapStyle(Qt::FlatCap);
            else if (attribute.value() == QStringLiteral("square"))
                setPenCapStyle(Qt::SquareCap);
            else
                setPenCapStyle(Qt::RoundCap);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:stroke-linejoin"))
        {
            if (attribute.value() == QStringLiteral("miter"))
                setPenJoinStyle(Qt::MiterJoin);
            else if (attribute.value() == QStringLiteral("bevel"))
                setPenJoinStyle(Qt::BevelJoin);
            else
                setPenJoinStyle(Qt::RoundJoin);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:fill"))
        {
            if (attribute.value() == QStringLiteral("solid"))
                setBrushColor(QColor(0, 0, 0));
            else
                setBrushColor(QColor(0, 0, 0, 0));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:fill-color"))
        {
            setBrushColor(xml->colorFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:opacity"))
        {
            mBrushColor.setAlphaF(xml->percentFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-start"))
        {
            if (attribute.value() == QStringLiteral("Triangle"))
                setStartMarkerStyle(Odg::TriangleMarker);
            else if (attribute.value() == QStringLiteral("Circle"))
                setStartMarkerStyle(Odg::CircleMarker);
            else
                setStartMarkerStyle(Odg::NoMarker);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-start-width"))
        {
            setStartMarkerSize(xml->lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-end"))
        {
            if (attribute.value() == QStringLiteral("Triangle"))
                setEndMarkerStyle(Odg::TriangleMarker);
            else if (attribute.value() == QStringLiteral("Circle"))
                setEndMarkerStyle(Odg::CircleMarker);
            else
                setEndMarkerStyle(Odg::NoMarker);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-end-width"))
        {
            setEndMarkerSize(xml->lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:textarea-horizontal-align"))
        {
            Qt::Alignment textAlignment = (mTextAlignment & (~Qt::AlignHorizontal_Mask));
            if (attribute.value() == QStringLiteral("left"))
                setTextAlignment(textAlignment | Qt::AlignLeft);
            else if (attribute.value() == QStringLiteral("right"))
                setTextAlignment(textAlignment | Qt::AlignRight);
            else
                setTextAlignment(textAlignment | Qt::AlignHCenter);
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:textarea-vertical-align"))
        {
            Qt::Alignment textAlignment = (mTextAlignment & (~Qt::AlignVertical_Mask));
            if (attribute.value() == QStringLiteral("top"))
                setTextAlignment(textAlignment | Qt::AlignTop);
            else if (attribute.value() == QStringLiteral("bottom"))
                setTextAlignment(textAlignment | Qt::AlignBottom);
            else
                setTextAlignment(textAlignment | Qt::AlignVCenter);
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:padding-left"))
        {
            setTextPadding(QSizeF(xml->lengthFromString(attribute.value()), mTextPadding.height()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:padding-top"))
        {
            setTextPadding(QSizeF(mTextPadding.width(), xml->lengthFromString(attribute.value())));
        }
    }

    // No sub-elements for <style:graphic-properties>
    xml->skipCurrentElement();
}

void OdgStyle::readParagraphProperties(OdgReader* xml)
{
    // Currently nothing to do for <style:paragraph-properties> element
    xml->skipCurrentElement();
}

void OdgStyle::readTextProperties(OdgReader* xml)
{
    // Read attributes of <style:text-properties> element
    const QXmlStreamAttributes attributes = xml->attributes();
    for(auto& attribute : attributes)
    {
        if (attribute.qualifiedName() == QStringLiteral("style:font-name"))
        {
            setFontFamily(attribute.value().toString());
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-size"))
        {
            setFontSize(xml->lengthFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-weight"))
        {
            bool bold = (attribute.value() == QStringLiteral("bold"));
            setFontStyle(OdgFontStyle(bold, mFontStyle.isItalic(), mFontStyle.isUnderline(), mFontStyle.isStrikeOut()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-style"))
        {
            bool italic = (attribute.value() == QStringLiteral("italic"));
            setFontStyle(OdgFontStyle(mFontStyle.isBold(), italic, mFontStyle.isUnderline(), mFontStyle.isStrikeOut()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("style:text-underline-style"))
        {
            bool underline = (attribute.value() == QStringLiteral("solid"));
            setFontStyle(OdgFontStyle(mFontStyle.isBold(), mFontStyle.isItalic(), underline, mFontStyle.isStrikeOut()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("style:text-line-through-style"))
        {
            bool strikeOut = (attribute.value() == QStringLiteral("solid"));
            setFontStyle(OdgFontStyle(mFontStyle.isBold(), mFontStyle.isItalic(), mFontStyle.isUnderline(), strikeOut));
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:color"))
        {
            setTextColor(xml->colorFromString(attribute.value()));
        }
        else if (attribute.qualifiedName() == QStringLiteral("loext:opacity"))
        {
            mTextColor.setAlphaF(xml->percentFromString(attribute.value()));
        }
    }

    // No sub-elements for <style:text-properties>
    xml->skipCurrentElement();
}
