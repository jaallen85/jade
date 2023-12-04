// File: OdgDrawing.h
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

#ifndef ODGDRAWING_H
#define ODGDRAWING_H

#include <QColor>
#include <QList>
#include <QMarginsF>
#include <QObject>
#include <QRectF>
#include "OdgGlobal.h"

class QPainter;
class OdgPage;

class OdgDrawing
{
protected:
    Odg::Units mUnits;
    QSizeF mPageSize;
    QMarginsF mPageMargins;
    QColor mBackgroundColor;

    double mGrid;
    Odg::GridStyle mGridStyle;
    QColor mGridColor;
    int mGridSpacingMajor;
    int mGridSpacingMinor;

    QList<OdgPage*> mPages;

public:
    OdgDrawing();
    virtual ~OdgDrawing();

    virtual void setUnits(Odg::Units units);
    virtual void setPageSize(const QSizeF& size);
    virtual void setPageMargins(const QMarginsF& margins);
    virtual void setBackgroundColor(const QColor& color);
    Odg::Units units() const;
    QSizeF pageSize() const;
    QMarginsF pageMargins() const;
    QColor backgroundColor() const;
    QRectF pageRect() const;
    QRectF contentRect() const;

    virtual void setGrid(double grid);
    virtual void setGridStyle(Odg::GridStyle style);
    virtual void setGridColor(const QColor& color);
    virtual void setGridSpacingMajor(int spacing);
    virtual void setGridSpacingMinor(int spacing);
    double grid() const;
    Odg::GridStyle gridStyle() const;
    QColor gridColor() const;
    int gridSpacingMajor() const;
    int gridSpacingMinor() const;
    double roundToGrid(double value) const;
    QPointF roundToGrid(const QPointF& position) const;

    virtual void setProperty(const QString& name, const QVariant& value);
    virtual QVariant property(const QString& name) const;

    void addPage(OdgPage* page);
    virtual void insertPage(int index, OdgPage* page);
    virtual void removePage(OdgPage* page);
    void clearPages();
    QList<OdgPage*> pages() const;
};

#endif
