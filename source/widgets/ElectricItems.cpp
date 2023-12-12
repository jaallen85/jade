// File: ElectricItems.cpp
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

#include "ElectricItems.h"
#include <OdgPathItem.h>

QList<OdgPathItem*> ElectricItems::items()
{
    QList<OdgPathItem*> items;
    items.append(createNmosFetItem());
    return items;
}

QStringList ElectricItems::icons()
{
    QStringList icons;
    icons.append(":/icons/items/nmos_fet.png");
    return icons;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createNmosFetItem()
{
    QPainterPath path;
    path.addEllipse(QRectF(-3.0, -3.0, 6.0, 6.0));
    path.closeSubpath();

    path.moveTo(2.0, -4.0); path.lineTo(2.0, -1.5);
    path.moveTo(2.0, -1.5); path.lineTo(0.0, -1.5);
    path.moveTo(0.0, 0.0); path.lineTo(2.0, 0.0);
    path.moveTo(2.0, 0.0); path.lineTo(2.0, 4.0);
    path.moveTo(0.0, 1.5); path.lineTo(2.0, 1.5);
    path.moveTo(0.0, 2.0); path.lineTo(0.0, 1.0);
    path.moveTo(0.0, 0.5); path.lineTo(0.0, -0.5);
    path.moveTo(0.0, -1.0); path.lineTo(0.0, -2.0);
    path.moveTo(-1.5, -2.0); path.lineTo(-1.5, 2.0);
    path.moveTo(-4.0, 0.0); path.lineTo(-1.5, 0.0);
    path.moveTo(0.8486, -0.8486); path.lineTo(0.0, 0.0);
    path.moveTo(0.0, 0.0); path.lineTo(0.8486, 0.8486);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("NMOS FET");
    item->setPath(path, QRectF(-4.0, -4.0, 8.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}
