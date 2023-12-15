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
    items.append(createResistor1());
    items.append(createResistor2());
    items.append(createCapacitor1());
    items.append(createCapacitor2());
    items.append(createInductor());
    items.append(createDiode());
    items.append(createZenerDiode());
    items.append(createSchottkyDiode());
    items.append(createNpnBjt());
    items.append(createPnpBjt());
    items.append(createNmosFet());
    items.append(createPmosFet());
    items.append(createGround1());
    items.append(createGround2());
    items.append(createOpAmp());
    items.append(createLed());
    items.append(createVdc());
    items.append(createVac());
    items.append(createIdc());
    items.append(createIac());
    items.append(createTransmissionLine());
    return items;
}

QStringList ElectricItems::icons()
{
    QStringList icons;
    icons.append(":/icons/items/resistor1.png");
    icons.append(":/icons/items/resistor2.png");
    icons.append(":/icons/items/capacitor1.png");
    icons.append(":/icons/items/capacitor2.png");
    icons.append(":/icons/items/inductor1.png");
    icons.append(":/icons/items/diode.png");
    icons.append(":/icons/items/zener_diode.png");
    icons.append(":/icons/items/schottky_diode.png");
    icons.append(":/icons/items/npn_bjt.png");
    icons.append(":/icons/items/pnp_bjt.png");
    icons.append(":/icons/items/nmos_fet.png");
    icons.append(":/icons/items/pmos_fet.png");
    icons.append(":/icons/items/ground1.png");
    icons.append(":/icons/items/ground2.png");
    icons.append(":/icons/items/opamp.png");
    icons.append(":/icons/items/led.png");
    icons.append(":/icons/items/vdc.png");
    icons.append(":/icons/items/vac.png");
    icons.append(":/icons/items/idc.png");
    icons.append(":/icons/items/iac.png");
    icons.append("");
    return icons;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createResistor1()
{
    QPainterPath path;
    path.moveTo(-4.0, 0.0); path.lineTo(-3.0, 0.0);
    path.moveTo(-3.0, 0.0); path.lineTo(-2.5, -1.0);
    path.moveTo(-2.5, -1.0); path.lineTo(-1.5, 1.0);
    path.moveTo(-1.5, 1.0); path.lineTo(-0.5, -1.0);
    path.moveTo(-0.5, -1.0); path.lineTo(0.5, 1.0);
    path.moveTo(0.5, 1.0); path.lineTo(1.5, -1.0);
    path.moveTo(1.5, -1.0); path.lineTo(2.5, 1.0);
    path.moveTo(2.5, 1.0); path.lineTo(3.0, 0.0);
    path.moveTo(3.0, 0.0); path.lineTo(4.0, 0.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Resistor 1");
    item->setPath(path, QRectF(-4.0, -1.0, 8.0, 2.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createResistor2()
{
    QPainterPath path;
    path.addRect(QRectF(-3.0, -1.0, 6.0, 2.0));
    path.closeSubpath();
    path.moveTo(-4.0, 0.0); path.lineTo(-3.0, 0.0);
    path.moveTo(3.0, 0.0); path.lineTo(4.0, 0.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Resistor 2");
    item->setPath(path, QRectF(-4.0, -1.0, 8.0, 2.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createCapacitor1()
{
    QPainterPath path;
    path.moveTo(-4.0, 0.0); path.lineTo(-0.5, 0.0);
    path.moveTo(-0.5, -1.5); path.lineTo(-0.5, 1.5);
    path.moveTo(0.5, -1.5); path.lineTo(0.5, 1.5);
    path.moveTo(0.5, 0.0); path.lineTo(4.0, 0.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Capacitor 1");
    item->setPath(path, QRectF(-4.0, -1.5, 8.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createCapacitor2()
{
    QPainterPath path;
    path.moveTo(-4.0, 0.0); path.lineTo(-0.5, 0.0);
    path.moveTo(-0.5, -1.5); path.lineTo(-0.5, 1.5);
    path.moveTo(1.0, -1.5); path.cubicTo(0.2, -1.0, 0.2, 1.0, 1.0, 1.5);
    path.moveTo(0.4, 0.0); path.lineTo(4.0, 0.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Capacitor 2");
    item->setPath(path, QRectF(-4.0, -1.5, 8.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createInductor()
{
    QPainterPath path;
    path.moveTo(-4.0, 0.0); path.lineTo(-3.2, 0.0);
    path.moveTo(-3.2, 0.0); path.cubicTo(-3.2, -1.556, -1.0, -1.556, -1.0, 0.0);
    path.moveTo(-1.8, 0.0); path.cubicTo(-1.8, -1.556, 0.4, -1.556, 0.4, 0.0);
    path.moveTo(-0.4, 0.0); path.cubicTo(-0.4, -1.556, 1.8, -1.556, 1.8, 0.0);
    path.moveTo(1.0, 0.0); path.cubicTo(1.0, -1.556, 3.2, -1.556, 3.2, 0.0);
    path.moveTo(-1.0, 0.0); path.cubicTo(-1.0, 0.8, -1.8, 0.8, -1.8, 0.0);
    path.moveTo(0.4, 0.0); path.cubicTo(0.4, 0.8, -0.4, 0.8, -0.4, 0.0);
    path.moveTo(1.8, 0.0); path.cubicTo(1.8, 0.8, 1.0, 0.8, 1.0, 0.0);
    path.moveTo(3.2, 0.0); path.lineTo(4.0, 0.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Inductor");
    item->setPath(path, QRectF(-4.0, -1.5, 8.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createDiode()
{
    QPainterPath path;
    path.moveTo(1.0, 0.0); path.lineTo(-1.0, -1.5); path.lineTo(-1.0, 1.5); path.closeSubpath();

    path.moveTo(1.0, -1.5); path.lineTo(1.0, 1.5);

    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);
    path.moveTo(1.0, 0); path.lineTo(4.0, 0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Diode");
    item->setPath(path, QRectF(-4.0, -1.5, 8.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createZenerDiode()
{
    QPainterPath path;
    path.moveTo(1.0, 0.0); path.lineTo(-1.0, -1.5); path.lineTo(-1.0, 1.5); path.closeSubpath();

    path.moveTo(2.0, -2.5); path.lineTo(1.0, -1.5);
    path.moveTo(1.0, -1.5); path.lineTo(1.0, 1.5);
    path.moveTo(1.0, 1.5); path.lineTo(0.0, 2.5);

    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);
    path.moveTo(1.0, 0); path.lineTo(4.0, 0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Zener Diode");
    item->setPath(path, QRectF(-4.0, -2.5, 8.0, 5.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createSchottkyDiode()
{
    QPainterPath path;
    path.moveTo(1.0, 0.0); path.lineTo(-1.0, -1.5); path.lineTo(-1.0, 1.5); path.closeSubpath();

    path.moveTo(0.2, 1.2); path.lineTo(0.2, 2.0);
    path.moveTo(0.2, 2.0); path.lineTo(1.0, 2.0);
    path.moveTo(1.0, 2.0); path.lineTo(1.0, -2.0);
    path.moveTo(1.0, -2.0); path.lineTo(1.8, -2.0);
    path.moveTo(1.8, -2.0); path.lineTo(1.8, -1.2);

    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);
    path.moveTo(1.0, 0); path.lineTo(4.0, 0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Schottky Diode");
    item->setPath(path, QRectF(-4.0, -1.5, 8.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createNpnBjt()
{
    QPainterPath path;
    path.addEllipse(QRectF(-3.0, -3.0, 6.0, 6.0));
    path.closeSubpath();

    path.moveTo(2.0, -4.0); path.lineTo(2.0, -2.0);
    path.moveTo(2.0, -2.0); path.lineTo(-1.0, -1.0);
    path.moveTo(-1.0, -1.0); path.lineTo(-1.0, 1.0);
    path.moveTo(-1.0, 1.0); path.lineTo(2.0, 2.0);
    path.moveTo(2.0, 2.0); path.lineTo(2.0, 4.0);
    path.moveTo(-1.0, -2.0); path.lineTo(-1.0, 2.0);
    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);

    path.moveTo(1.4172, 0.951); path.lineTo(2.0, 2.0);
    path.moveTo(2.0, 2.0); path.lineTo(0.831, 2.5828);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("NPN BJT");
    item->setPath(path, QRectF(-4.0, -4.0, 8.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createPnpBjt()
{
    QPainterPath path;
    path.addEllipse(QRectF(-3.0, -3.0, 6.0, 6.0));
    path.closeSubpath();

    path.moveTo(2.0, -4.0); path.lineTo(2.0, -2.0);
    path.moveTo(2.0, -2.0); path.lineTo(-1.0, -1.0);
    path.moveTo(-1.0, -1.0); path.lineTo(-1.0, 1.0);
    path.moveTo(-1.0, 1.0); path.lineTo(2.0, 2.0);
    path.moveTo(2.0, 2.0); path.lineTo(2.0, 4.0);
    path.moveTo(-1.0, -2.0); path.lineTo(-1.0, 2.0);
    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);

    path.moveTo(0.049, 0.4172); path.lineTo(-1.0, 1.0);
    path.moveTo(-1.0, 1.0); path.lineTo(-0.4172, 2.049);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("PNP BJT");
    item->setPath(path, QRectF(-4.0, -4.0, 8.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createNmosFet()
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
    path.moveTo(-1.0, -2.0); path.lineTo(-1.0, 2.0);
    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);

    path.moveTo(0.7, -0.5); path.lineTo(0.0, 0.0);
    path.moveTo(0.0, 0.0); path.lineTo(0.7, 0.5);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("NMOS FET");
    item->setPath(path, QRectF(-4.0, -4.0, 8.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createPmosFet()
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
    path.moveTo(-1.0, -2.0); path.lineTo(-1.0, 2.0);
    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);

    path.moveTo(1.3, -0.5); path.lineTo(2.0, 0.0);
    path.moveTo(2.0, 0.0); path.lineTo(1.3, 0.5);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("PMOS FET");
    item->setPath(path, QRectF(-4.0, -4.0, 8.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createGround1()
{
    QPainterPath path;
    path.moveTo(0.0, 0.0); path.lineTo(0.0, 1.0);
    path.moveTo(-1.5, 1.0); path.lineTo(1.5, 1.0);
    path.moveTo(-1.0, 2.0); path.lineTo(1.0, 2.0);
    path.moveTo(-0.5, 3.0); path.lineTo(0.5, 3.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Ground 1");
    item->setPath(path, QRectF(-1.5, 0.0, 3.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createGround2()
{
    QPainterPath path;
    path.moveTo(0.0, 0.0); path.lineTo(0.0, 1.0);
    path.moveTo(-1.5, 1.0); path.lineTo(1.5, 1.0); path.lineTo(0.0, 3.0); path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Ground 2");
    item->setPath(path, QRectF(-1.5, 0.0, 3.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createOpAmp()
{
    QPainterPath path;
    path.moveTo(-4.0, -4.0); path.lineTo(-4.0, 4.0); path.lineTo(4.0, 0.0); path.closeSubpath();

    path.moveTo(-6.0, -2.0); path.lineTo(-4.0, -2.0);
    path.moveTo(-6.0, 2.0); path.lineTo(-4.0, 2.0);
    path.moveTo(6.0, 0.0); path.lineTo(4.0, 0.0);

    path.moveTo(-3.3, -2.0); path.lineTo(-1.9, -2.0);
    path.moveTo(-2.6, -2.7); path.lineTo(-2.6, -1.3);
    path.moveTo(-3.3, 2.0); path.lineTo(-1.9, 2.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Op Amp");
    item->setPath(path, QRectF(-6.0, -4.0, 12.0, 8.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createLed()
{
    QPainterPath path;
    path.moveTo(1.0, 0.0); path.lineTo(-1.0, -1.5); path.lineTo(-1.0, 1.5); path.closeSubpath();

    path.moveTo(1.0, -1.5); path.lineTo(1.0, 1.5);

    path.moveTo(-4.0, 0.0); path.lineTo(-1.0, 0.0);
    path.moveTo(1.0, 0); path.lineTo(4.0, 0);

    path.moveTo(1.4, -0.7); path.lineTo(2.2, -1.5);
    path.moveTo(1.6, -1.5); path.lineTo(2.2, -1.5);
    path.moveTo(2.2, -1.5); path.lineTo(2.2, -0.9);

    path.moveTo(2.6, -0.7); path.lineTo(3.4, -1.5);
    path.moveTo(2.8, -1.5); path.lineTo(3.4, -1.5);
    path.moveTo(3.4, -1.5); path.lineTo(3.4, -0.9);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("LED");
    item->setPath(path, QRectF(-4.0, -1.5, 8.0, 3.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createVdc()
{
    QPainterPath path;
    path.addEllipse(QRectF(-3.0, -4.0, 6.0, 8.0));
    path.closeSubpath();

    path.moveTo(0.0, -6.0); path.lineTo(0.0, -4.0);
    path.moveTo(0.0, 4.0); path.lineTo(0.0, 6.0);

    path.moveTo(-0.7, -2.0); path.lineTo(0.7, -2.0);
    path.moveTo(0.0, -2.7); path.lineTo(0.0, -1.3);
    path.moveTo(-0.7, 2.0); path.lineTo(0.7, 2.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("DC Voltage Source");
    item->setPath(path, QRectF(-3.0, -6.0, 6.0, 12.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createVac()
{
    QPainterPath path;
    path.addEllipse(QRectF(-3.0, -4.0, 6.0, 8.0));
    path.closeSubpath();

    path.moveTo(0.0, -6.0); path.lineTo(0.0, -4.0);
    path.moveTo(0.0, 4.0); path.lineTo(0.0, 6.0);

    path.moveTo(-1.0, 0.0); path.cubicTo(-0.2, -2.4, 0.2, 2.4, 1.0, 0.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("AC Voltage Source");
    item->setPath(path, QRectF(-3.0, -6.0, 6.0, 12.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createIdc()
{
    QPainterPath path;
    path.addEllipse(QRectF(-3.0, -4.0, 6.0, 8.0));
    path.closeSubpath();

    path.moveTo(0.0, -6.0); path.lineTo(0.0, -4.0);
    path.moveTo(0.0, 4.0); path.lineTo(0.0, 6.0);

    path.moveTo(0.0, 2.0); path.lineTo(0.0, -2.0);
    path.moveTo(0.5, -1.3); path.lineTo(0.0, -2.0);
    path.moveTo(0.0, -2.0); path.lineTo(-0.5, -1.3);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("DC Current Source");
    item->setPath(path, QRectF(-3.0, -6.0, 6.0, 12.0));
    item->setRect(item->pathRect());

    return item;
}

OdgPathItem* ElectricItems::createIac()
{
    QPainterPath path;
    path.addEllipse(QRectF(-3.0, -4.0, 6.0, 8.0));
    path.closeSubpath();

    path.moveTo(0.0, -6.0); path.lineTo(0.0, -4.0);
    path.moveTo(0.0, 4.0); path.lineTo(0.0, 6.0);

    path.moveTo(0.0, 2.0); path.lineTo(0.0, -2.0);
    path.moveTo(0.5, -1.3); path.lineTo(0.0, -2.0);
    path.moveTo(0.0, -2.0); path.lineTo(-0.5, -1.3);

    path.moveTo(-1.0, 0.0); path.cubicTo(-0.2, -2.4, 0.2, 2.4, 1.0, 0.0);

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("AC Current Source");
    item->setPath(path, QRectF(-3.0, -6.0, 6.0, 12.0));
    item->setRect(item->pathRect());

    return item;
}

//======================================================================================================================

OdgPathItem* ElectricItems::createTransmissionLine()
{
    QPainterPath path;

    path.moveTo(-4.0, -2.0);
    path.lineTo(4.0, -2.0);
    path.cubicTo(5.4, -2.0, 5.4, 2.0, 4.0, 2.0);
    path.lineTo(-4.0, 2.0);
    path.cubicTo(-2.6, 2.0, -2.6, -2.0, -4.0, -2.0);
    path.closeSubpath();

    path.cubicTo(-5.4, -2.0, -5.4, 2.0, -4.0, 2.0);
    path.cubicTo(-2.6, 2.0, -2.6, -2.0, -4.0, -2.0);
    path.closeSubpath();

    OdgPathItem* item = new OdgPathItem();
    item->setPathName("Transmission Line");
    item->setPath(path, QRectF(-5.0, -2.0, 10.0, 4.0));
    item->setRect(item->pathRect());

    return item;
}
