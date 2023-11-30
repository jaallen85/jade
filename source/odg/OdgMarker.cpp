// File: OdgMarker.cpp
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

#include "OdgMarker.h"
#include <QPainter>

OdgMarker::OdgMarker(Odg::MarkerStyle style, double size) : mStyle(style), mSize(size)
{
    if (mSize < 0) mSize = 0;
}

//======================================================================================================================

void OdgMarker::setStyle(Odg::MarkerStyle style)
{
    mStyle = style;
    updatePath();
}

void OdgMarker::setSize(double size)
{
    if (size >= 0)
    {
        mSize = size;
        updatePath();
    }
}

Odg::MarkerStyle OdgMarker::style() const
{
    return mStyle;
}

double OdgMarker::size() const
{
    return mSize;
}

//======================================================================================================================

void OdgMarker::paint(QPainter& painter, const QPen& pen, const QPointF& position, double angle)
{
    if (mStyle != Odg::NoMarker && mSize > 0)
    {
        QPen markerPen = pen;
        markerPen.setStyle(Qt::SolidLine);
        painter.setPen(markerPen);
        painter.setBrush(markerPen.brush());

        switch (mStyle)
        {
        case Odg::CircleMarker:
            painter.translate(position);
            painter.drawPath(mPath);
            painter.translate(-position);
            break;
        case Odg::TriangleMarker:
            painter.translate(position);
            painter.rotate(angle);
            painter.drawPath(mPath);
            painter.rotate(-angle);
            painter.translate(-position);
            break;
        default:    // Odg::NoMarker
            break;
        }
    }
}

//======================================================================================================================

void OdgMarker::updatePath()
{
    mPath.clear();

    if (mStyle != Odg::NoMarker && mSize > 0)
    {
        switch (mStyle)
        {
            case Odg::CircleMarker:
            {
                mPath.addEllipse(QPointF(0, 0), mSize, mSize);
                break;
            }
            case Odg::TriangleMarker:
            {
                const static double angle = M_PI / 6;     // Makes the arrowhead a 60 degree angle
                const static double sqrt2 = qSqrt(2);
                const double x = mSize * 2 / sqrt2 * qCos(angle);
                const double y = mSize * 2 / sqrt2 * qSin(angle);
                mPath.moveTo(QPointF(0, 0));
                mPath.lineTo(QPointF(-x, -y));
                mPath.lineTo(QPointF(-x, y));
                mPath.closeSubpath();
                break;
            }
            default:    // Odg::NoMarker
            {
                break;
            }
        }
    }
}
