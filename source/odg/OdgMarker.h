// File: OdgMarker.h
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

#ifndef ODGMARKER_H
#define ODGMARKER_H

#include <QPainterPath>
#include <QPen>
#include "OdgGlobal.h"

class QPainter;

class OdgMarker
{
private:
    Odg::MarkerStyle mStyle;
    double mSize;

    QPainterPath mPath;

public:
    OdgMarker(Odg::MarkerStyle style = Odg::NoMarker, double size = 0);

    void setStyle(Odg::MarkerStyle style);
    void setSize(double size);
    Odg::MarkerStyle style() const;
    double size() const;

    void paint(QPainter& painter, const QPen& pen, const QPointF& position, double angle);

private:
    void updatePath();
};

#endif
