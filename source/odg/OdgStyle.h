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
#include "OdgFontStyle.h"
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
    OdgFontStyle mFontStyle;
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

    void setName(const QString& name);
    void setParent(OdgStyle* parent);
    QString name() const;
    OdgStyle* parent() const;

    void setPenStyle(Qt::PenStyle style);
    void setPenWidth(double width);
    void setPenColor(const QColor& color);
    void setPenCapStyle(Qt::PenCapStyle style);
    void setPenJoinStyle(Qt::PenJoinStyle style);
    void setBrushColor(const QColor& color);
    void unsetPenStyle();
    void unsetPenWidth();
    void unsetPenColor();
    void unsetPenCapStyle();
    void unsetPenJoinStyle();
    void unsetBrushColor();
    Qt::PenStyle penStyle() const;
    double penWidth() const;
    QColor penColor() const;
    Qt::PenCapStyle penCapStyle() const;
    Qt::PenJoinStyle penJoinStyle() const;
    QColor brushColor() const;
    bool isPenStyleValid() const;
    bool isPenWidthValid() const;
    bool isPenColorValid() const;
    bool isPenCapStyleValid() const;
    bool isPenJoinStyleValid() const;
    bool isBrushColorValid() const;

    void setStartMarkerStyle(Odg::MarkerStyle style);
    void setStartMarkerSize(double size);
    void setEndMarkerStyle(Odg::MarkerStyle style);
    void setEndMarkerSize(double size);
    void unsetStartMarkerStyle();
    void unsetStartMarkerSize();
    void unsetEndMarkerStyle();
    void unsetEndMarkerSize();
    Odg::MarkerStyle startMarkerStyle() const;
    double startMarkerSize() const;
    Odg::MarkerStyle endMarkerStyle() const;
    double endMarkerSize() const;
    bool isStartMarkerStyleValid() const;
    bool isStartMarkerSizeValid() const;
    bool isEndMarkerStyleValid() const;
    bool isEndMarkerSizeValid() const;

    void setFontFamily(const QString& family);
    void setFontSize(double size);
    void setFontStyle(const OdgFontStyle& style);
    void setTextAlignment(Qt::Alignment alignment);
    void setTextPadding(const QSizeF& padding);
    void setTextColor(const QColor& color);
    void unsetFontFamily();
    void unsetFontSize();
    void unsetFontStyle();
    void unsetTextAlignment();
    void unsetTextPadding();
    void unsetTextColor();
    QString fontFamily() const;
    double fontSize() const;
    OdgFontStyle fontStyle() const;
    Qt::Alignment textAlignment() const;
    QSizeF textPadding() const;
    QColor textColor() const;
    bool isFontFamilyValid() const;
    bool isFontSizeValid() const;
    bool isFontStyleValid() const;
    bool isTextAlignmentValid() const;
    bool isTextPaddingValid() const;
    bool isTextColorValid() const;

    void clear();
};

#endif
