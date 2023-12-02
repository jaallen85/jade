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
    Units unitsFromString(const QStringView& str, bool* ok = nullptr);
    Units unitsFromString(const QString& str, bool* ok = nullptr);

    enum GluePointEscapeDirection { EscapeNone = 0x00, EscapeLeft = 0x01, EscapeRight = 0x02, EscapeHoriztional = 0x03,
                                    EscapeUp = 0x04, EscapeDown = 0x08, EscapeVertical = 0x0C, EscapeAll = 0x0F };
    Q_DECLARE_FLAGS(GluePointEscapeDirections, GluePointEscapeDirection)

    enum MarkerStyle { NoMarker, TriangleMarker, CircleMarker };

    enum GridStyle { GridHidden, GridLines };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Odg::GluePointEscapeDirections)

#endif
