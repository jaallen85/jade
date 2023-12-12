// File: LogicItems.cpp
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

#include "LogicItems.h"
#include <OdgPathItem.h>

QList<OdgPathItem*> LogicItems::items()
{
    QList<OdgPathItem*> items;
    items.append(createAndGate());
    return items;
}

QStringList LogicItems::icons()
{
    QStringList icons;
    icons.append(":/icons/items/and_gate.png");
    return icons;
}

//======================================================================================================================

OdgPathItem* LogicItems::createAndGate()
{
    OdgPathItem* item = new OdgPathItem();

    QPainterPath path;
    path.moveTo(-3.0, -2.0);
    path.cubicTo(5.0, -2.0, 5.0, 2.0, -3.0, 2.0);
    path.closeSubpath();

    //path.moveTo(-200, -50); path.lineTo(-150, -50);
    //path.moveTo(-200, 50); path.lineTo(-150, 50);
    //path.moveTo(150, 0); path.lineTo(200, 0);

    item->setPathName("AND Gate");
    item->setPath(path, path.boundingRect());
    item->setRect(item->pathRect());

    return item;
}
