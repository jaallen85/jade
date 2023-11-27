// File: OdgDrawing.h
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

#ifndef ODGDRAWING_H
#define ODGDRAWING_H

#include <QString>

class OdgDrawing
{
public:
    enum Units { Millimeters, Inches };

private:
    Units mUnits;

public:
    OdgDrawing();
    virtual ~OdgDrawing();

    Units units() const;

    virtual bool load(const QString& fileName);

public:
    static double convertUnits(double value, Units units, Units newUnits);
};

#endif
