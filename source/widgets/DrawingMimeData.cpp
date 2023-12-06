// File: DrawingMimeData.cpp
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

#include "DrawingMimeData.h"
#include "OdgItem.h"

DrawingMimeData::DrawingMimeData() : mItems()
{
    // Nothing more to do here.
}

DrawingMimeData::~DrawingMimeData()
{
    qDeleteAll(mItems);
}

//======================================================================================================================

void DrawingMimeData::setItems(const QList<OdgItem*>& items)
{
    qDeleteAll(mItems);
    mItems = items;
}

QList<OdgItem*> DrawingMimeData::items() const
{
    return mItems;
}

//======================================================================================================================

bool DrawingMimeData::hasFormat(const QString& mimeType) const
{
    return (mimeType == format());
}

QStringList DrawingMimeData::formats() const
{
    QStringList formats;
    formats << format();
    return formats;
}

//======================================================================================================================

QVariant DrawingMimeData::retrieveData(const QString& mimeType, QMetaType type) const
{
    if (hasFormat(mimeType)) return QVariant::fromValue< QList<OdgItem*> >(mItems);
    return QVariant();
}

//======================================================================================================================

QString DrawingMimeData::format()
{
    return "application/jade-items";
}
