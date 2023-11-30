// File: OdgDrawing.cpp
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

#include "OdgDrawing.h"
#include "OdgPage.h"

OdgDrawing::OdgDrawing() :
    mUnits(Odg::UnitsInches), mPageSize(8.2, 6.2), mPageMargins(0.1, 0.1, 0.1, 0.1), mBackgroundColor(255, 255, 255),
    mGrid(0.05), mGridStyle(Odg::GridLines), mGridColor(77, 153, 153), mGridSpacingMajor(8), mGridSpacingMinor(2),
    mPages()
{
    // Nothing more to do here.
}

OdgDrawing::~OdgDrawing()
{
    clearPages();
}

//======================================================================================================================

void OdgDrawing::setUnits(Odg::Units units)
{
    if (mUnits != units)
    {
        double scaleFactor = Odg::convertUnits(1, mUnits, units);

        mUnits = units;

        mPageSize.setWidth(mPageSize.width() * scaleFactor);
        mPageSize.setHeight(mPageSize.height() * scaleFactor);
        mPageMargins.setLeft(mPageMargins.left() * scaleFactor);
        mPageMargins.setTop(mPageMargins.top() * scaleFactor);
        mPageMargins.setRight(mPageMargins.right() * scaleFactor);
        mPageMargins.setBottom(mPageMargins.bottom() * scaleFactor);

        mGrid *= scaleFactor;

        for(auto& page : qAsConst(mPages))
            page->scaleBy(scaleFactor);
    }
}

void OdgDrawing::setPageSize(const QSizeF& size)
{
    if (size.width() > 0 && size.height() > 0) mPageSize = size;
}

void OdgDrawing::setPageMargins(const QMarginsF& margins)
{
    if (margins.left() >= 0 && margins.top() >= 0 && margins.right() >= 0 && margins.bottom() >= 0)
        mPageMargins = margins;
}

void OdgDrawing::setBackgroundColor(const QColor& color)
{
    mBackgroundColor = color;
}

Odg::Units OdgDrawing::units() const
{
    return mUnits;
}

QSizeF OdgDrawing::pageSize() const
{
    return mPageSize;
}

QMarginsF OdgDrawing::pageMargins() const
{
    return mPageMargins;
}

QColor OdgDrawing::backgroundColor() const
{
    return mBackgroundColor;
}

QRectF OdgDrawing::pageRect() const
{
    return QRectF(-mPageMargins.left(), -mPageMargins.top(), mPageSize.width(), mPageSize.height());
}

QRectF OdgDrawing::contentRect() const
{
    return QRectF(0, 0, mPageSize.width() - mPageMargins.left() - mPageMargins.right(),
                  mPageSize.height() - mPageMargins.top() - mPageMargins.bottom());
}

//======================================================================================================================

void OdgDrawing::setGrid(double grid)
{
    if (grid >= 0) mGrid = grid;
}

void OdgDrawing::setGridStyle(Odg::GridStyle style)
{
    mGridStyle = style;
}

void OdgDrawing::setGridColor(const QColor& color)
{
    mGridColor = color;
}

void OdgDrawing::setGridSpacingMajor(int spacing)
{
    if (spacing >= 0) mGridSpacingMajor = spacing;
}

void OdgDrawing::setGridSpacingMinor(int spacing)
{
    if (spacing >= 0) mGridSpacingMinor = spacing;
}

double OdgDrawing::grid() const
{
    return mGrid;
}

Odg::GridStyle OdgDrawing::gridStyle() const
{
    return mGridStyle;
}

QColor OdgDrawing::gridColor() const
{
    return mGridColor;
}

int OdgDrawing::gridSpacingMajor() const
{
    return mGridSpacingMajor;
}

int OdgDrawing::gridSpacingMinor() const
{
    return mGridSpacingMinor;
}

double OdgDrawing::roundToGrid(double value) const
{
    qreal result = value;

    if (mGrid > 0)
    {
        qreal mod = fmod(value, mGrid);
        result = value - mod;
        if (mod >= mGrid/2) result += mGrid;
        else if (mod <= -mGrid/2) result -= mGrid;
    }

    return result;
}

QPointF OdgDrawing::roundToGrid(const QPointF& position) const
{
    return QPointF(roundToGrid(position.x()), roundToGrid(position.y()));
}

//======================================================================================================================

void OdgDrawing::addPage(OdgPage* page)
{
    if (page) mPages.append(page);
}

void OdgDrawing::insertPage(int index, OdgPage* page)
{
    if (page) mPages.insert(index, page);
}

void OdgDrawing::removePage(OdgPage* page)
{
    if (page) mPages.removeAll(page);
}

void OdgDrawing::clearPages()
{
    qDeleteAll(mPages);
    mPages.clear();
}

QList<OdgPage*> OdgDrawing::pages() const
{
    return mPages;
}

//======================================================================================================================

void OdgDrawing::paint(QPainter& painter, bool isExport)
{

}
