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

OdgDrawing::OdgDrawing() : mUnits(Millimeters)
{

}

OdgDrawing::~OdgDrawing()
{

}

//======================================================================================================================

OdgDrawing::Units OdgDrawing::units() const
{
    return mUnits;
}

//======================================================================================================================

double OdgDrawing::convertUnits(double value, Units units, Units newUnits)
{
    // Bypass this function if both provided units are identical (no conversion necessary)
    if (units == newUnits) return value;

    double unitsConversionFactorToMeters = 1.0, newUnitsConversionFactorFromMeters = 1.0;

    if (units == Inches)
        unitsConversionFactorToMeters = 0.0254;
    else
        unitsConversionFactorToMeters = 0.001;

    if (newUnits == Inches)
        newUnitsConversionFactorFromMeters = 1 / 0.0254;
    else
        newUnitsConversionFactorFromMeters = 1000;

    return value * unitsConversionFactorToMeters * newUnitsConversionFactorFromMeters;
}
