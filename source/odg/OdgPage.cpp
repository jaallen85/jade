// File: OdgPage.cpp
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

#include "OdgPage.h"
#include "OdgItem.h"

OdgPage::OdgPage(const QString& name) : mName(name), mItems()
{
    // Nothing more to do here.
}

OdgPage::~OdgPage()
{
    clearItems();
}

//======================================================================================================================

void OdgPage::setName(const QString& name)
{
    mName = name;
}

QString OdgPage::name() const
{
    return mName;
}

//======================================================================================================================

void OdgPage::addItem(OdgItem* item)
{
    if (item) mItems.append(item);
}

void OdgPage::insertItem(int index, OdgItem* item)
{
    if (item) mItems.insert(index, item);
}

void OdgPage::removeItem(OdgItem* item)
{
    if (item) mItems.removeAll(item);
}

void OdgPage::clearItems()
{
    qDeleteAll(mItems);
    mItems.clear();
}

QList<OdgItem*> OdgPage::items() const
{
    return mItems;
}

//======================================================================================================================

void OdgPage::setProperty(const QString& name, const QVariant& value)
{
    if (name == "name" && value.canConvert<QString>())
        setName(value.toString());
}

QVariant OdgPage::property(const QString& name) const
{
    if (name == "name")
        return mName;
    return QVariant();
}
