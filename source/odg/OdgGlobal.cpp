// File: OdgGlobal.cpp
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

#include "OdgGlobal.h"

namespace Odg
{

double convertUnits(double value, Units units, Units newUnits)
{
    // Bypass this function if both provided units are identical (no conversion necessary)
    if (units == newUnits) return value;

    double unitsConversionFactorToMeters = 1.0, newUnitsConversionFactorFromMeters = 1.0;

    switch (units)
    {
    case UnitsInches:
        unitsConversionFactorToMeters = 0.0254;
        break;
    default:    // UnitsMillimeters
        unitsConversionFactorToMeters = 0.001;
        break;
    }

    switch (newUnits)
    {
    case UnitsInches:
        newUnitsConversionFactorFromMeters = 1 / 0.0254;
        break;
    default:    // UnitsMillimeters
        newUnitsConversionFactorFromMeters = 1000;
        break;
    }

    return value * unitsConversionFactorToMeters * newUnitsConversionFactorFromMeters;
}

QString unitsToString(Units units)
{
    switch (units)
    {
    case UnitsInches:
        return "in";
    default:    // UnitsMillimeters
        break;
    }

    return "mm";
}

Units unitsFromString(const QStringView& str, bool* ok)
{
    if (str.compare(QStringLiteral("mm"), Qt::CaseInsensitive) == 0)
    {
        if (ok) *ok = true;
        return UnitsMillimeters;
    }
    if (str.compare(QStringLiteral("in"), Qt::CaseInsensitive) == 0)
    {
        if (ok) *ok = true;
        return UnitsInches;
    }

    if (ok) *ok = false;
    return UnitsMillimeters;
}

Units unitsFromString(const QString& str, bool* ok)
{
    return unitsFromString(QStringView(str), ok);
}

}
