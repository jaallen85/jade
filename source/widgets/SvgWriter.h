// File: SvgWriter.h
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

#ifndef SVGWRITER_H
#define SVGWRITER_H

#include <QPainterPath>

class OdgItem;

class SvgWriter
{
private:
    QRectF mRect;
    double mScale;

public:
    SvgWriter(const QRectF& rect, double scale);

    bool write(const QString& path, const QList<OdgItem*>& items);

private:
    QString lengthToString(double length) const;
    QString colorToString(const QColor& color) const;

    QString transformToString(const QPointF& position, bool flipped, int rotation) const;
    QString viewBoxToString(const QRectF& viewBox) const;
    QString pointsToString(const QPolygonF& points) const;
    QString pathToString(const QPainterPath& path) const;

};

#endif
