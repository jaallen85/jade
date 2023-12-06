// File: OdgGlobal.h
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

#ifndef ODGGLOBAL_H
#define ODGGLOBAL_H

#include <QString>

namespace Odg
{
    enum Units { UnitsMillimeters, UnitsInches };

    double convertUnits(double value, Units units, Units newUnits);
    QString unitsToString(Units units);
    Units unitsFromString(const QStringView& str, bool* ok = nullptr);
    Units unitsFromString(const QString& str, bool* ok = nullptr);

    enum MarkerStyle { NoMarker, TriangleMarker, CircleMarker };

    enum GridStyle { GridHidden, GridLines };

    enum DrawingMode { SelectMode, ScrollMode, ZoomMode, PlaceMode };
}

#endif
