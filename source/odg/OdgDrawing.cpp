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
#include "OdgItem.h"
#include <QPainter>

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
    double result = value;

    if (mGrid > 0)
    {
        double mod = fmod(value, mGrid);
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

void OdgDrawing::drawBackground(QPainter& painter, bool drawBorder, bool drawGrid)
{
    const QColor pageBorderColor(255 - mBackgroundColor.red(), 255 - mBackgroundColor.green(),
                                 255 - mBackgroundColor.blue());
    const QColor contentBorderColor(128, 128, 128);

    // Fill background
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, false);
    painter.setBackground(QBrush(mBackgroundColor));
    painter.setBrush(QBrush(mBackgroundColor));
    if (drawBorder)
        painter.setPen(QPen(Qt::NoPen));
    else
        painter.setPen(QPen(QBrush(pageBorderColor), 0));
    painter.drawRect(pageRect());

    // Draw content border
    if (drawBorder)
    {
        painter.setBrush(QBrush(Qt::transparent));
        painter.setPen(QPen(QBrush(contentBorderColor), 0));
        painter.drawRect(contentRect());
    }

    // Draw grid
    if (drawGrid && mGrid > 0)
    {
        QColor minorGridColor = QColor::fromHslF(mGridColor.hslHueF(), mGridColor.hslSaturationF(),
                                                 mGridColor.lightnessF() + (1 - mGridColor.lightnessF()) * 0.7);

        switch (mGridStyle)
        {
        case Odg::GridLines:
            // Minor and Major grid lines
            drawGridLines(painter, minorGridColor, mGridSpacingMinor);
            drawGridLines(painter, mGridColor, mGridSpacingMajor);

            // Draw content border again
            painter.setBrush(QBrush(Qt::transparent));
            painter.setPen(QPen(QBrush(mGridColor), 0));
            painter.drawRect(contentRect());
            break;
        default:    // Odg::GridHidden
            break;
        }
    }
}

void OdgDrawing::drawItems(QPainter& painter, const QList<OdgItem*>& items)
{
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    for(auto& item : items)
    {
        painter.setTransform(item->transform(), true);
        item->paint(painter);
        painter.setTransform(item->transformInverse(), true);
    }
}

//======================================================================================================================

void OdgDrawing::drawGridLines(QPainter& painter, const QColor& color, int spacing)
{
    if (mGrid > 0 && spacing > 0)
    {
        const QRectF gridRect = contentRect();

        painter.setPen(QPen(QBrush(color), 0, Qt::SolidLine));

        const double gridInterval = mGrid * spacing;
        const int gridLeftIndex = qCeil(gridRect.left() / gridInterval);
        const int gridRightIndex = qFloor(gridRect.right() / gridInterval);
        const int gridTopIndex = qCeil(gridRect.top() / gridInterval);
        const int gridBottomIndex = qFloor(gridRect.bottom() / gridInterval);
        double x, y;
        for(int xIndex = gridLeftIndex; xIndex <= gridRightIndex; xIndex++)
        {
            x = xIndex * gridInterval;
            painter.drawLine(QLineF(x, gridRect.top(), x, gridRect.bottom()));
        }
        for(int yIndex = gridTopIndex; yIndex <= gridBottomIndex; yIndex++)
        {
            y = yIndex * gridInterval;
            painter.drawLine(QLineF(gridRect.left(), y, gridRect.right(), y));
        }
    }
}
