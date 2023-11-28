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

OdgStyle::OdgStyle(Odg::Units units, bool defaultStyle) : mName(), mParent(nullptr),
    mPenStyle(Qt::SolidLine), mPenWidth(0.0), mPenColor(0, 0, 0),
    mPenCapStyle(Qt::RoundCap), mPenJoinStyle(Qt::RoundJoin), mBrushColor(255, 255, 255),
    mPenStyleValid(defaultStyle), mPenWidthValid(defaultStyle), mPenColorValid(defaultStyle),
    mPenCapStyleValid(defaultStyle), mPenJoinStyleValid(defaultStyle), mBrushColorValid(defaultStyle),
    mStartMarkerStyle(Odg::NoMarker), mStartMarkerSize(0.0), mEndMarkerStyle(Odg::NoMarker), mEndMarkerSize(0.0),
    mStartMarkerStyleValid(defaultStyle), mStartMarkerSizeValid(defaultStyle),
    mEndMarkerStyleValid(defaultStyle), mEndMarkerSizeValid(defaultStyle),
    mFontFamily(), mFontSize(0.0), mFontBold(false), mFontItalic(false), mFontUnderline(false), mFontStrikeOut(false),
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

QString OdgStyle::name() const
{
    return mName;
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

#include <QDebug>

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

    qDebug() << mName << (mParent ? mParent->name() : "none") << mPenColor << mPenColorValid;
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
                mPenStyle = Qt::SolidLine;
            else if (attribute.value() == QStringLiteral("dash"))
                mPenStyle = Qt::DashLine;
            else
                mPenStyle = Qt::NoPen;
            mPenStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:stroke-dash"))
        {
            if (attribute.value() == QStringLiteral("Dash_20__28_Rounded_29_"))
                mPenStyle = Qt::DashLine;
            else if (attribute.value() == QStringLiteral("Dot_20__28_Rounded_29_"))
                mPenStyle = Qt::DotLine;
            else if (attribute.value() == QStringLiteral("Dash_20_Dot_20__28_Rounded_29_"))
                mPenStyle = Qt::DashDotLine;
            else if (attribute.value() == QStringLiteral("Dash_20_Dot_20_Dot_20__28_Rounded_29_"))
                mPenStyle = Qt::DashDotDotLine;
            mPenStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-width"))
        {
            mPenWidth = xml->lengthFromString(attribute.value().toString());
            mPenWidthValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-color"))
        {
            mPenColor = QColor(attribute.value().toString());
            mPenColorValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-opacity"))
        {
            mPenColor.setAlphaF(xml->percentFromString(attribute.value().toString()));
            mPenColorValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("svg:stroke-linecap"))
        {
            if (attribute.value() == QStringLiteral("butt"))
                mPenCapStyle = Qt::FlatCap;
            else if (attribute.value() == QStringLiteral("square"))
                mPenCapStyle = Qt::SquareCap;
            else
                mPenCapStyle = Qt::RoundCap;
            mPenCapStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:stroke-linejoin"))
        {
            if (attribute.value() == QStringLiteral("miter"))
                mPenJoinStyle = Qt::MiterJoin;
            else if (attribute.value() == QStringLiteral("bevel"))
                mPenJoinStyle = Qt::BevelJoin;
            else
                mPenJoinStyle = Qt::RoundJoin;
            mPenJoinStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:fill"))
        {
            if (attribute.value() == QStringLiteral("solid"))
                mBrushColor = QColor(0, 0, 0);
            else
                mBrushColor = QColor(0, 0, 0, 0);
            mBrushColorValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:fill-color"))
        {
            mBrushColor = QColor(attribute.value().toString());
            mBrushColorValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:opacity"))
        {
            mBrushColor.setAlphaF(xml->percentFromString(attribute.value().toString()));
            mBrushColorValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-start"))
        {
            if (attribute.value() == QStringLiteral("Triangle"))
                mStartMarkerStyle = Odg::TriangleMarker;
            else if (attribute.value() == QStringLiteral("Circle"))
                mStartMarkerStyle = Odg::CircleMarker;
            else
                mStartMarkerStyle = Odg::NoMarker;
            mStartMarkerStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-start-width"))
        {
            mStartMarkerSize = xml->lengthFromString(attribute.value().toString());
            mStartMarkerSizeValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-end"))
        {
            if (attribute.value() == QStringLiteral("Triangle"))
                mEndMarkerStyle = Odg::TriangleMarker;
            else if (attribute.value() == QStringLiteral("Circle"))
                mEndMarkerStyle = Odg::CircleMarker;
            else
                mEndMarkerStyle = Odg::NoMarker;
            mEndMarkerStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:marker-end-width"))
        {
            mEndMarkerSize = xml->lengthFromString(attribute.value().toString());
            mEndMarkerSizeValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:textarea-horizontal-align"))
        {
            mTextAlignment = (mTextAlignment & (~Qt::AlignHorizontal_Mask));
            if (attribute.value() == QStringLiteral("left"))
                mTextAlignment = (mTextAlignment | Qt::AlignLeft);
            else if (attribute.value() == QStringLiteral("right"))
                mTextAlignment = (mTextAlignment | Qt::AlignRight);
            else
                mTextAlignment = (mTextAlignment | Qt::AlignHCenter);
            mTextAlignmentValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("draw:textarea-vertical-align"))
        {
            mTextAlignment = (mTextAlignment & (~Qt::AlignVertical_Mask));
            if (attribute.value() == QStringLiteral("top"))
                mTextAlignment = (mTextAlignment | Qt::AlignTop);
            else if (attribute.value() == QStringLiteral("bottom"))
                mTextAlignment = (mTextAlignment | Qt::AlignBottom);
            else
                mTextAlignment = (mTextAlignment | Qt::AlignVCenter);
            mTextAlignmentValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:padding-left"))
        {
            mTextPadding.setWidth(xml->lengthFromString(attribute.value().toString()));
            mTextPaddingValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:padding-top"))
        {
            mTextPadding.setHeight(xml->lengthFromString(attribute.value().toString()));
            mTextPaddingValid = true;
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
            mFontFamily = attribute.value().toString();
            mFontFamilyValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-size"))
        {
            mFontSize = xml->lengthFromString(attribute.value().toString());
            mFontSizeValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-weight"))
        {
            mFontBold = (attribute.value() == QStringLiteral("bold"));
            mFontStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:font-style"))
        {
            mFontItalic = (attribute.value() == QStringLiteral("italic"));
            mFontStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("style:text-underline-style"))
        {
            mFontUnderline = (attribute.value() == QStringLiteral("solid"));
            mFontStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("style:text-line-through-style"))
        {
            mFontStrikeOut = (attribute.value() == QStringLiteral("solid"));
            mFontStyleValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("fo:color"))
        {
            mTextColor = QColor(attribute.value().toString());
            mTextColorValid = true;
        }
        else if (attribute.qualifiedName() == QStringLiteral("loext:opacity"))
        {
            mTextColor.setAlphaF(xml->percentFromString(attribute.value().toString()));
            mTextColorValid = true;
        }
    }

    // No sub-elements for <style:text-properties>
    xml->skipCurrentElement();
}
