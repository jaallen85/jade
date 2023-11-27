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
#include <QFile>
#include <quazip.h>
#include <quazipfile.h>

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

#include <QDebug>

bool OdgDrawing::load(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;

    QuaZip odgArchive(&file);
    if (!odgArchive.open(QuaZip::mdUnzip)) return false;

    //for(auto& name : odgArchive.getFileNameList())
    //    qDebug() << name;

    QuaZipFile odgFile(&odgArchive);
    odgArchive.setCurrentFile("styles.xml");
    if (!odgFile.open(QFile::ReadOnly)) return false;
    qDebug() << odgFile.read(256);
    odgFile.close();

    odgArchive.setCurrentFile("content.xml");
    if (!odgFile.open(QFile::ReadOnly)) return false;
    qDebug() << odgFile.read(256);
    odgFile.close();

    return true;
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
