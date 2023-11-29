// File: OdgReader.h
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

#ifndef ODGREADER_H
#define ODGREADER_H

#include <QColor>
#include <QMarginsF>
#include <QSizeF>
#include <QXmlStreamReader>
#include "OdgGlobal.h"

class OdgReader : public QXmlStreamReader
{
private:
    Odg::Units mUnits;
    QSizeF mPageSize;
    QMarginsF mPageMargins;

public:
    OdgReader();

    void setUnits(Odg::Units units);
    void setPageSize(const QSizeF& size);
    void setPageMargins(const QMarginsF& margins);
    Odg::Units units() const;
    QSizeF pageSize() const;
    QMarginsF pageMargins() const;

    double lengthFromString(const QStringView& str) const;
    double lengthFromString(const QString& str) const;
    double percentFromString(const QStringView& str) const;
    double percentFromString(const QString& str) const;
    QColor colorFromString(const QStringView& str) const;
    QColor colorFromString(const QString& str) const;
};

#endif
