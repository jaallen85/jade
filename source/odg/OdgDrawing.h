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
#include <QMarginsF>
#include <QRectF>
#include "OdgGlobal.h"

class QPainter;
class OdgPage;
class OdgReader;
class OdgStyle;

class OdgDrawing
{
private:
    Odg::Units mUnits;
    QSizeF mPageSize;
    QMarginsF mPageMargins;
    QColor mBackgroundColor;

    double mGrid;
    bool mGridVisible;
    QColor mGridColor;
    int mGridSpacingMajor;
    int mGridSpacingMinor;

    QList<OdgStyle*> mStyles;

    QList<OdgPage*> mPages;

public:
    OdgDrawing();
    virtual ~OdgDrawing();

    void setUnits(Odg::Units units);
    void setPageSize(const QSizeF& size);
    void setPageMargins(const QMarginsF& margins);
    void setBackgroundColor(const QColor& color);
    Odg::Units units() const;
    QSizeF pageSize() const;
    QMarginsF pageMargins() const;
    QColor backgroundColor() const;
    QRectF pageRect() const;
    QRectF contentRect() const;

    void setGrid(double grid);
    void setGridVisible(bool visible);
    void setGridColor(const QColor& color);
    void setGridSpacingMajor(int spacing);
    void setGridSpacingMinor(int spacing);
    double grid() const;
    bool isGridVisible() const;
    QColor gridColor() const;
    int gridSpacingMajor() const;
    int gridSpacingMinor() const;
    double roundToGrid(double value) const;
    QPointF roundToGrid(const QPointF& position) const;

    void addStyle(OdgStyle* style);
    void insertStyle(int index, OdgStyle* style);
    void removeStyle(OdgStyle* style);
    void clearStyles();
    QList<OdgStyle*> styles() const;

    void addPage(OdgPage* page);
    void insertPage(int index, OdgPage* page);
    void removePage(OdgPage* page);
    void clearPages();
    QList<OdgPage*> pages() const;

    void paint(QPainter* painter, bool isExport = false);

    virtual bool save(const QString& fileName);
    virtual bool load(const QString& fileName);

private:
    void readDocumentSettings(OdgReader* xml);
    void readSettings(OdgReader* xml);
    void readConfigItemSet(OdgReader* xml);
    void readConfigItem(OdgReader* xml);

    void readDocumentStyles(OdgReader* xml);
    void readStyles(OdgReader* xml);
    void readAutomaticPageStyles(OdgReader* xml);
    void readMasterPageStyles(OdgReader* xml);
    void readPageLayout(OdgReader* xml);
    void readPageStyle(OdgReader* xml);
    void readMasterPage(OdgReader* xml);

    void readDocumentContent(OdgReader* xml);
    void readContentStyles(OdgReader* xml, QList<OdgStyle*>& contentStyles);
    void readBody(OdgReader* xml);
    void readDrawing(OdgReader* xml);
    void readPage(OdgReader* xml);
    void readItems(OdgReader* xml);
};

#endif
