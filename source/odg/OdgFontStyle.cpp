// File: OdgFontStyle.cpp
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

#include "OdgFontStyle.h"

OdgFontStyle::OdgFontStyle(bool bold, bool italic, bool underline, bool strikeOut) :
    mBold(bold), mItalic(italic), mUnderline(underline), mStrikeOut(strikeOut)
{
    // Nothing more to do here.
}

OdgFontStyle::OdgFontStyle(const OdgFontStyle& other) :
    mBold(other.mBold), mItalic(other.mItalic), mUnderline(other.mUnderline), mStrikeOut(other.mStrikeOut)
{
    // Nothing more to do here.
}

//======================================================================================================================

void OdgFontStyle::setBold(bool bold)
{
    mBold = bold;
}

void OdgFontStyle::setItalic(bool italic)
{
    mItalic = italic;
}

void OdgFontStyle::setUnderline(bool underline)
{
    mUnderline = underline;
}

void OdgFontStyle::setStrikeOut(bool strikeOut)
{
    mStrikeOut = strikeOut;
}

bool OdgFontStyle::isBold() const
{
    return mBold;
}

bool OdgFontStyle::isItalic() const
{
    return mItalic;
}

bool OdgFontStyle::isUnderline() const
{
    return mUnderline;
}

bool OdgFontStyle::isStrikeOut() const
{
    return mStrikeOut;
}

//======================================================================================================================

OdgFontStyle& OdgFontStyle::operator=(const OdgFontStyle &other)
{
    mBold = other.mBold;
    mItalic = other.mItalic;
    mUnderline = other.mUnderline;
    mStrikeOut = other.mStrikeOut;
    return *this;
}

//======================================================================================================================

bool operator==(const OdgFontStyle& fontStyle1, const OdgFontStyle& fontStyle2)
{
    return (fontStyle1.isBold() == fontStyle2.isBold() &&
            fontStyle1.isItalic() == fontStyle2.isItalic() &&
            fontStyle1.isUnderline() == fontStyle2.isUnderline() &&
            fontStyle1.isStrikeOut() == fontStyle2.isStrikeOut());
}

bool operator!=(const OdgFontStyle& fontStyle1, const OdgFontStyle& fontStyle2)
{
    return (fontStyle1.isBold() != fontStyle2.isBold() ||
            fontStyle1.isItalic() != fontStyle2.isItalic() ||
            fontStyle1.isUnderline() != fontStyle2.isUnderline() ||
            fontStyle1.isStrikeOut() != fontStyle2.isStrikeOut());
}
