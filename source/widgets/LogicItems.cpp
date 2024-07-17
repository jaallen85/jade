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
    items.append(createOrGate());
    items.append(createXorGate());
    items.append(createXnorGate());
    items.append(createNandGate());
    items.append(createNorGate());
    items.append(createBuffer());
    items.append(createNotGate());
    items.append(createMultiplexer());
    items.append(createDemultiplexer());
    items.append(createFlipFlop());
    return items;
}

QStringList LogicItems::icons()
{
    QStringList icons;
    icons.append(":/icons/items/and_gate.png");
    icons.append(":/icons/items/or_gate.png");
    icons.append(":/icons/items/xor_gate.png");
    icons.append(":/icons/items/xnor_gate.png");
    icons.append(":/icons/items/nand_gate.png");
    icons.append(":/icons/items/nor_gate.png");
    icons.append(":/icons/items/buffer.png");
    icons.append(":/icons/items/not_gate.png");
    icons.append(":/icons/items/multiplexer.png");
    icons.append(":/icons/items/demultiplexer.png");
    icons.append(":/icons/items/flip_flop2.png");
    return icons;
}

//======================================================================================================================

OdgPathItem* LogicItems::createAndGate()
{
    QPainterPath path;
    path.moveTo(-2.0, -2.0);
    path.cubicTo(3.3, -2.0, 3.3, 2.0, -2.0, 2.0);
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("AND Gate");
    item->setPath(path, QRectF(-2.0, -2.0, 4.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* LogicItems::createOrGate()
{
    QPainterPath path;
    path.moveTo(-2.0, -2.0);
    path.cubicTo(3.3, -2.0, 3.3, 2.0, -2.0, 2.0);
    path.cubicTo(-0.4, 2.0, -0.4, -2.0, -2.0, -2.0);
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("OR Gate");
    item->setPath(path, QRectF(-2.0, -2.0, 4.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* LogicItems::createXorGate()
{
    QPainterPath path;
    path.moveTo(-2.0, -2.0);
    path.cubicTo(3.3, -2.0, 3.3, 2.0, -2.0, 2.0);
    path.cubicTo(-0.4, 2.0, -0.4, -2.0, -2.0, -2.0);
    path.closeSubpath();

    path.addEllipse(QRectF(2.0, -0.5, 1.0, 1.0));
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("XOR Gate");
    item->setPath(path, QRectF(-3.0, -2.0, 6.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* LogicItems::createXnorGate()
{
    QPainterPath path;
    path.moveTo(-2.0, -2.0);
    path.cubicTo(3.3, -2.0, 3.3, 2.0, -2.0, 2.0);
    path.cubicTo(-0.4, 2.0, -0.4, -2.0, -2.0, -2.0);
    path.closeSubpath();

    path.addEllipse(QRectF(2.0, -0.5, 1.0, 1.0));
    path.closeSubpath();

    path.moveTo(-3.0, -2.0);
    path.cubicTo(-1.4, -2.0, -1.4, 2.0, -3.0, 2.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("XNOR Gate");
    item->setPath(path, QRectF(-3.0, -2.0, 6.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* LogicItems::createNandGate()
{
    QPainterPath path;
    path.moveTo(-2.0, -2.0);
    path.cubicTo(3.3, -2.0, 3.3, 2.0, -2.0, 2.0);
    path.closeSubpath();

    path.addEllipse(QRectF(2.0, -0.5, 1.0, 1.0));
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("NAND Gate");
    item->setPath(path, QRectF(-3.0, -2.0, 6.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* LogicItems::createNorGate()
{
    QPainterPath path;
    path.moveTo(-2.0, -2.0);
    path.cubicTo(3.3, -2.0, 3.3, 2.0, -2.0, 2.0);
    path.cubicTo(-0.4, 2.0, -0.4, -2.0, -2.0, -2.0);
    path.closeSubpath();

    path.addEllipse(QRectF(2.0, -0.5, 1.0, 1.0));
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("NOR Gate");
    item->setPath(path, QRectF(-3.0, -2.0, 6.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* LogicItems::createBuffer()
{
    QPainterPath path;
    path.moveTo(2.0, 0.0); path.lineTo(-2.0, -2.0); path.lineTo(-2.0, 2.0); path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Buffer");
    item->setPath(path, QRectF(-2.0, -2.0, 4.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* LogicItems::createNotGate()
{
    QPainterPath path;
    path.moveTo(2.0, 0.0); path.lineTo(-2.0, -2.0); path.lineTo(-2.0, 2.0); path.closeSubpath();

    path.addEllipse(QRectF(2.0, -0.5, 1.0, 1.0));
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("NOT Gate");
    item->setPath(path, QRectF(-3.0, -2.0, 6.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* LogicItems::createMultiplexer()
{
    QPainterPath path;
    path.moveTo(-2.0, -4.0);
    path.lineTo(-2.0, 4.0);
    path.lineTo(2.0, 2.0);
    path.lineTo(2.0, -2.0);
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Multiplexer");
    item->setPath(path, QRectF(-2.0, -4.0, 4.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* LogicItems::createDemultiplexer()
{
    QPainterPath path;
    path.moveTo(-2.0, -2.0);
    path.lineTo(-2.0, 2.0);
    path.lineTo(2.0, 4.0);
    path.lineTo(2.0, -4.0);
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Demultiplexer");
    item->setPath(path, QRectF(-2.0, -4.0, 4.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* LogicItems::createFlipFlop()
{
    QPainterPath path;
    path.addRect(QRectF(-3.0, -4.0, 6.0, 8.0));
    path.closeSubpath();

    path.moveTo(-3.0, 1.0); path.lineTo(-2.0, 2.0);
    path.moveTo(-2.0, 2.0); path.lineTo(-3.0, 3.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Flip-Flop");
    item->setPath(path, QRectF(-3.0, -4.0, 6.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}
