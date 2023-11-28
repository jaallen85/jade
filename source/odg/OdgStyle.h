// File: OdgStyle.h
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

#ifndef ODGSTYLE_H
#define ODGSTYLE_H

#include <QColor>
#include <QSizeF>
#include "OdgGlobal.h"

class OdgReader;

class OdgStyle
{
private:
    QString mName;
    OdgStyle* mParent;

    // Pen / brush style options
    Qt::PenStyle mPenStyle;
    double mPenWidth;
    QColor mPenColor;
    Qt::PenCapStyle mPenCapStyle;
    Qt::PenJoinStyle mPenJoinStyle;
    QColor mBrushColor;

    bool mPenStyleValid;
    bool mPenWidthValid;
    bool mPenColorValid;
    bool mPenCapStyleValid;
    bool mPenJoinStyleValid;
    bool mBrushColorValid;

    // Marker options
    Odg::MarkerStyle mStartMarkerStyle;
    double mStartMarkerSize;
    Odg::MarkerStyle mEndMarkerStyle;
    double mEndMarkerSize;

    bool mStartMarkerStyleValid;
    bool mStartMarkerSizeValid;
    bool mEndMarkerStyleValid;
    bool mEndMarkerSizeValid;

    // Text options
    QString mFontFamily;
    double mFontSize;
    bool mFontBold;
    bool mFontItalic;
    bool mFontUnderline;
    bool mFontStrikeOut;
    Qt::Alignment mTextAlignment;
    QSizeF mTextPadding;
    QColor mTextColor;

    bool mFontFamilyValid;
    bool mFontSizeValid;
    bool mFontStyleValid;
    bool mTextAlignmentValid;
    bool mTextPaddingValid;
    bool mTextColorValid;

public:
    OdgStyle(Odg::Units units, bool defaultStyle = false);

    QString name() const;

    void clear();

    void readFromXml(OdgReader* xml, const QList<OdgStyle*>& styles);

private:
    void readGraphicProperties(OdgReader* xml);
    void readParagraphProperties(OdgReader* xml);
    void readTextProperties(OdgReader* xml);
};

#endif
