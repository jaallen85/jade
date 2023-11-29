// File: OdgReader.cpp
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

#include "OdgReader.h"
#include <QRegularExpression>

OdgReader::OdgReader() : QXmlStreamReader(), mUnits(Odg::UnitsMillimeters)
{

}

//======================================================================================================================

void OdgReader::setUnits(Odg::Units units)
{
    mUnits = units;
}

Odg::Units OdgReader::units() const
{
    return mUnits;
}

//======================================================================================================================

double OdgReader::lengthFromString(const QString& str) const
{
    static const QRegularExpression re(R"([-+]?(?:(?:\d*\.\d+)|(?:\d+\.?))(?:[Ee][+-]?\d+)?)");

    QRegularExpressionMatch match = re.match(str);
    if (match.hasMatch())
    {
        bool lengthOk = false;
        double length = match.captured(0).toDouble(&lengthOk);
        if (!lengthOk) return 0;

        bool unitsOk = false;
        QString unitsStr = str.last(str.size() - match.captured(0).size()).trimmed();
        Odg::Units units = Odg::unitsFromString(unitsStr, &unitsOk);

        if (!unitsOk)
        {
            // Assume the value provided is in the same units as mUnits
            return length;
        }

        return Odg::convertUnits(length, units, mUnits);
    }

    return 0;
}

double OdgReader::percentFromString(const QString& str) const
{
    if (str.endsWith("%"))
    {
        bool valueOk = false;
        double value = str.first(str.size() - 1).toDouble(&valueOk);
        return (valueOk) ? value / 100 : 0;
    }

    bool valueOk = false;
    double value = str.toDouble(&valueOk);
    return (valueOk) ? value : 0;
}

QColor OdgReader::colorFromString(const QString& str) const
{
    return QColor(str);
}
