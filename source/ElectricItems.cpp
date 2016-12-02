/* ElectricItems.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "ElectricItems.h"

QList<DrawingPathItem*> ElectricItems::items()
{
	QList<DrawingPathItem*> items;

	items << createResistor1() <<
		createResistor2() <<
		createCapacitor1() <<
		createCapacitor2() <<
		createInductor1() <<
		createDiode() <<
		createZenerDiode() <<
		createSchottkyDiode() <<
		createNpnBjt() <<
		createPnpBjt() <<
		createNmosFet() <<
		createPmosFet() <<
		createGround1() <<
		createGround2() <<
		createOpAmp() <<
		createLed() <<
		createVdc() <<
		createVac() <<
		createIdc() <<
		createIac();

	return items;
}

QStringList ElectricItems::icons()
{
	QStringList icons;

	icons << ":/icons/items/resistor1.png" <<
		":/icons/items/resistor2.png" <<
		":/icons/items/capacitor1.png" <<
		":/icons/items/capacitor2.png" <<
		":/icons/items/inductor1.png" <<
		":/icons/items/diode.png" <<
		":/icons/items/zener_diode.png" <<
		":/icons/items/schottky_diode.png" <<
		":/icons/items/npn_bjt.png" <<
		":/icons/items/pnp_bjt.png" <<
		":/icons/items/nmos_fet.png" <<
		":/icons/items/pmos_fet.png" <<
		":/icons/items/ground1.png" <<
		":/icons/items/ground2.png" <<
		":/icons/items/opamp.png" <<
		":/icons/items/led.png" <<
		":/icons/items/vdc.png" <<
		":/icons/items/vac.png" <<
		":/icons/items/idc.png" <<
		":/icons/items/iac.png";

	return icons;
}

//==================================================================================================

DrawingPathItem* ElectricItems::createResistor1()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -50, 400, 100);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-150, 0);
	path.moveTo(-150, 0); path.lineTo(-125, -50);
	path.moveTo(-125, -50); path.lineTo(-75, 50);
	path.moveTo(-75, 50); path.lineTo(-25, -50);
	path.moveTo(-25, -50); path.lineTo(25, 50);
	path.moveTo(25, 50); path.lineTo(75, -50);
	path.moveTo(75, -50); path.lineTo(125, 50);
	path.moveTo(125, 50); path.lineTo(150, 0);
	path.moveTo(150, 0); path.lineTo(200, 0);

	item->setName("Resistor 1");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* ElectricItems::createResistor2()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -50, 400, 100);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-150, 0);
	path.moveTo(-150, -50); path.lineTo(150, -50);
	path.moveTo(-150, 50); path.lineTo(150, 50);
	path.moveTo(-150, -50); path.lineTo(-150, 50);
	path.moveTo(150, -50); path.lineTo(150, 50);
	path.moveTo(150, 0); path.lineTo(200, 0);

	item->setName("Resistor 2");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}
DrawingPathItem* ElectricItems::createCapacitor1()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-25, 0);
	path.moveTo(-25, -75); path.lineTo(-25, 75);
	path.moveTo(25, -75); path.lineTo(25, 75);
	path.moveTo(25, 0); path.lineTo(200, 0);

	item->setName("Capacitor 1");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* ElectricItems::createCapacitor2()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-25, 0);
	path.moveTo(-25, -75); path.lineTo(-25, 75);
	path.moveTo(45, -75); path.cubicTo(5, -60, 5, 60, 45, 75);
	path.moveTo(25, 0); path.lineTo(200, 0);

	item->setName("Capacitor 2");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* ElectricItems::createInductor1()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-300, -75, 600, 75);
	QPainterPath path;
	path.moveTo(-300, 0); path.lineTo(-240, 0);
	path.moveTo(-240, 0); path.cubicTo(-240, -41.4214, -213.137, -75, -180, -75);
	path.moveTo(-180, -75); path.cubicTo(-146.863, -75, -120, -41.4214, -120, 0);
	path.moveTo(-120, 0); path.cubicTo(-120, -41.4214, -93.1371, -75, -60, -75);
	path.moveTo(-60, -75); path.cubicTo(-26.8629, -75, 0, -41.4214, 0, 0);
	path.moveTo(0, 0); path.cubicTo(0, -41.4214, 26.8629, -75, 60, -75);
	path.moveTo(60, -75); path.cubicTo(93.1371, -75, 120, -41.4214, 120, 0);
	path.moveTo(120, 0); path.cubicTo(120, -41.4214, 146.863, -75, 180, -75);
	path.moveTo(180, -75); path.cubicTo(213.137, -75, 240, -41.4214, 240, 0);
	path.moveTo(240, 0); path.lineTo(300, 0);

	item->setName("Inductor 1");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-300, 0));
	item->addConnectionPoint(QPointF(300, 0));

	return item;
}

DrawingPathItem* ElectricItems::createDiode()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(50, -75); path.lineTo(50, 75);
	path.moveTo(50, 0); path.lineTo(200, 0);

	item->setName("Diode");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* ElectricItems::createZenerDiode()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -125, 400, 250);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(100, -125); path.lineTo(50, -75);
	path.moveTo(50, -75); path.lineTo(50, 75);
	path.moveTo(50, 75); path.lineTo(0, 125);
	path.moveTo(50, 0); path.lineTo(200, 0);

	item->setName("Zener Diode");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* ElectricItems::createSchottkyDiode()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -100, 400, 200);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(10, 60); path.lineTo(10, 100);
	path.moveTo(10, 100); path.lineTo(50, 100);
	path.moveTo(50, 100); path.lineTo(50, -100);
	path.moveTo(50, -100); path.lineTo(90, -100);
	path.moveTo(90, -100); path.lineTo(90, -60);
	path.moveTo(50, 0); path.lineTo(200, 0);

	item->setName("Schottky Diode");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* ElectricItems::createNpnBjt()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;
	path.moveTo(175, 0);
	path.cubicTo(175, -88.3656, 103.366, -160, 15, -160);
	path.cubicTo(-73.3656, -160, -145, -88.3656, -145, 0);
	path.cubicTo(-145, 88.3656, -73.3656, 160, 15, 160);
	path.cubicTo(103.366, 160, 175, 88.3656, 175, 0);

	path.moveTo(100, -200); path.lineTo(100, -100);
	path.moveTo(100, -100); path.lineTo(-75, -50);
	path.moveTo(-75, -50); path.lineTo(-75, 50);
	path.moveTo(-75, 50); path.lineTo(100, 100);
	path.moveTo(100, 100); path.lineTo(100, 200);
	path.moveTo(-75, -100); path.lineTo(-75, 100);
	path.moveTo(-200, 0); path.lineTo(-75, 0);

	path.moveTo(70.86, 47.55); path.lineTo(100, 100);
	path.moveTo(100, 100); path.lineTo(41.55, 129.14);

	item->setName("NPN BJT");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(100, -200));
	item->addConnectionPoint(QPointF(100, 200));

	return item;
}

DrawingPathItem* ElectricItems::createPnpBjt()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;
	path.moveTo(175, 0);
	path.cubicTo(175, -88.3656, 103.366, -160, 15, -160);
	path.cubicTo(-73.3656, -160, -145, -88.3656, -145, 0);
	path.cubicTo(-145, 88.3656, -73.3656, 160, 15, 160);
	path.cubicTo(103.366, 160, 175, 88.3656, 175, 0);

	path.moveTo(100, -200); path.lineTo(100, -100);
	path.moveTo(100, -100); path.lineTo(-75, -50);
	path.moveTo(-75, -50); path.lineTo(-75, 50);
	path.moveTo(-75, 50); path.lineTo(100, 100);
	path.moveTo(100, 100); path.lineTo(100, 200);
	path.moveTo(-75, -100); path.lineTo(-75, 100);
	path.moveTo(-200, 0); path.lineTo(-75, 0);

	path.moveTo(-22.55, 20.86); path.lineTo(-75, 50);
	path.moveTo(-75, 50); path.lineTo(-45.86, 102.45);

	item->setName("PNP BJT");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(100, -200));
	item->addConnectionPoint(QPointF(100, 200));

	return item;
}

DrawingPathItem* ElectricItems::createNmosFet()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;
	path.moveTo(175, 0);
	path.cubicTo(175, -88.3656, 103.366, -160, 15, -160);
	path.cubicTo(-73.3656, -160, -145, -88.3656, -145, 0);
	path.cubicTo(-145, 88.3656, -73.3656, 160, 15, 160);
	path.cubicTo(103.366, 160, 175, 88.3656, 175, 0);

	path.moveTo(100, -200); path.lineTo(100, -75);
	path.moveTo(100, -75); path.lineTo(0, -75);
	path.moveTo(0, 0); path.lineTo(100, 0);
	path.moveTo(100, 0); path.lineTo(100, 200);
	path.moveTo(0, 75); path.lineTo(100, 75);
	path.moveTo(0, 100); path.lineTo(0, 50);
	path.moveTo(0, 25); path.lineTo(0, -25);
	path.moveTo(0, -50); path.lineTo(0, -100);
	path.moveTo(-75, -100); path.lineTo(-75, 100);
	path.moveTo(-200, 0); path.lineTo(-75, 0);
	path.moveTo(42.43, -42.43); path.lineTo(0, 0);
	path.moveTo(0, 0); path.lineTo(42.43, 42.43);

	item->setName("NMOS FET");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(100, -200));
	item->addConnectionPoint(QPointF(100, 200));

	return item;
}

DrawingPathItem* ElectricItems::createPmosFet()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;

	path.moveTo(175, 0);
	path.cubicTo(175, -88.3656, 103.366, -160, 15, -160);
	path.cubicTo(-73.3656, -160, -145, -88.3656, -145, 0);
	path.cubicTo(-145, 88.3656, -73.3656, 160, 15, 160);
	path.cubicTo(103.366, 160, 175, 88.3656, 175, 0);

	path.moveTo(100, 200); path.lineTo(100, 75);
	path.moveTo(100, 75); path.lineTo(0, 75);
	path.moveTo(0, 0); path.lineTo(100, 0);
	path.moveTo(100, 0); path.lineTo(100, -200);
	path.moveTo(0, -75); path.lineTo(100, -75);
	path.moveTo(0, 100); path.lineTo(0, 50);
	path.moveTo(0, 25); path.lineTo(0, -25);
	path.moveTo(0, -50); path.lineTo(0, -100);
	path.moveTo(-75, -100); path.lineTo(-75, 100);
	path.moveTo(-200, 0); path.lineTo(-75, 0);
	path.moveTo(57.57, -42.43); path.lineTo(100, 0);
	path.moveTo(100, 0); path.lineTo(57.57, 42.43);

	item->setName("PMOS FET");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(100, -200));
	item->addConnectionPoint(QPointF(100, 200));

	return item;
}

DrawingPathItem* ElectricItems::createGround1()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-100, 0, 200, 150);
	QPainterPath path;

	path.moveTo(0, 0); path.lineTo(0, 50);
	path.moveTo(-100, 50); path.lineTo(100, 50);
	path.moveTo(-75, 100); path.lineTo(75, 100);
	path.moveTo(-50, 150); path.lineTo(50, 150);

	item->setName("Ground 1");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(0, 0));

	return item;
}

DrawingPathItem* ElectricItems::createGround2()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-100, 0, 200, 150);
	QPainterPath path;

	path.moveTo(0, 0); path.lineTo(0, 50);
	path.moveTo(-100, 50); path.lineTo(100, 50);
	path.moveTo(100, 50); path.lineTo(0, 150);
	path.moveTo(0, 150); path.lineTo(-100, 50);

	item->setName("Ground 2");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(0, 0));

	return item;
}

DrawingPathItem* ElectricItems::createOpAmp()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-300, -200, 600, 400);
	QPainterPath path;

	path.moveTo(-200, -200); path.lineTo(-200, 200); path.lineTo(200, 0); path.lineTo(-200, -200);
	path.moveTo(-300, -100); path.lineTo(-200, -100);
	path.moveTo(-300, 100); path.lineTo(-200, 100);
	path.moveTo(300, 0); path.lineTo(200, 0);
	path.moveTo(-170, -100); path.lineTo(-90, -100);
	path.moveTo(-130, -140); path.lineTo(-130, -60);
	path.moveTo(-170, 100); path.lineTo(-90, 100);

	item->setName("Op Amp");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-300, -100));
	item->addConnectionPoint(QPointF(-300, 100));
	item->addConnectionPoint(QPointF(300, 0));
	item->addConnectionPoint(QPointF(0, -100));
	item->addConnectionPoint(QPointF(0, 100));

	return item;
}

DrawingPathItem* ElectricItems::createLed()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(50, -75); path.lineTo(50, 75);
	path.moveTo(50, 0); path.lineTo(200, 0);

	path.moveTo(70, -35); path.lineTo(110, -75);
	path.moveTo(80, -75); path.lineTo(110, -75);
	path.moveTo(110, -75); path.lineTo(110, -45);

	path.moveTo(130, -35); path.lineTo(170, -75);
	path.moveTo(140, -75); path.lineTo(170, -75);
	path.moveTo(170, -75); path.lineTo(170, -45);

	item->setName("LED");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* ElectricItems::createVdc()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-150, -300, 300, 600);
	QPainterPath path;

	path.moveTo(150, 0);
	path.cubicTo(150, -110.457, 82.8427, -200, 0, -200);
	path.cubicTo(-82.8427, -200, -150, -110.457, -150, 0);
	path.cubicTo(-150, 110.457, -82.8427, 200, 0, 200);
	path.cubicTo(82.8427, 200, 150, 110.457, 150, 0);

	path.moveTo(0, -300); path.lineTo(0, -200);
	path.moveTo(0, 200); path.lineTo(0, 300);

	path.moveTo(-40, -65); path.lineTo(40, -65);
	path.moveTo(0, -105); path.lineTo(0, -25);
	path.moveTo(-40, 90); path.lineTo(40, 90);

	item->setName("DC Voltage");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(0, -300));
	item->addConnectionPoint(QPointF(0, 300));

	return item;
}

DrawingPathItem* ElectricItems::createVac()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-150, -300, 300, 600);
	QPainterPath path;

	path.moveTo(150, 0);
	path.cubicTo(150, -110.457, 82.8427, -200, 0, -200);
	path.cubicTo(-82.8427, -200, -150, -110.457, -150, 0);
	path.cubicTo(-150, 110.457, -82.8427, 200, 0, 200);
	path.cubicTo(82.8427, 200, 150, 110.457, 150, 0);

	path.moveTo(0, -300); path.lineTo(0, -200);
	path.moveTo(0, 200); path.lineTo(0, 300);

	path.moveTo(-50, 0); path.cubicTo(-10, -120, 10, 120, 50, 0);

	item->setName("AC Voltage");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(0, -300));
	item->addConnectionPoint(QPointF(0, 300));

	return item;
}

DrawingPathItem* ElectricItems::createIdc()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-150, -300, 300, 600);
	QPainterPath path;

	path.moveTo(150, 0);
	path.cubicTo(150, -110.457, 82.8427, -200, 0, -200);
	path.cubicTo(-82.8427, -200, -150, -110.457, -150, 0);
	path.cubicTo(-150, 110.457, -82.8427, 200, 0, 200);
	path.cubicTo(82.8427, 200, 150, 110.457, 150, 0);

	path.moveTo(0, -300); path.lineTo(0, -200);
	path.moveTo(0, 200); path.lineTo(0, 300);

	path.moveTo(0, 100); path.lineTo(0, -100);
	path.moveTo(-42.43, -57.57); path.lineTo(0, -100);
	path.moveTo(0, -100); path.lineTo(42.43, -57.57);

	item->setName("DC Current");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(0, -300));
	item->addConnectionPoint(QPointF(0, 300));

	return item;
}

DrawingPathItem* ElectricItems::createIac()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-150, -300, 300, 600);
	QPainterPath path;

	path.moveTo(150, 0);
	path.cubicTo(150, -110.457, 82.8427, -200, 0, -200);
	path.cubicTo(-82.8427, -200, -150, -110.457, -150, 0);
	path.cubicTo(-150, 110.457, -82.8427, 200, 0, 200);
	path.cubicTo(82.8427, 200, 150, 110.457, 150, 0);

	path.moveTo(0, -300); path.lineTo(0, -200);
	path.moveTo(0, 200); path.lineTo(0, 300);

	path.moveTo(0, 100); path.lineTo(0, -100);
	path.moveTo(-42.43, -57.57); path.lineTo(0, -100);
	path.moveTo(0, -100); path.lineTo(42.43, -57.57);
	path.moveTo(-50, 0); path.cubicTo(-10, -120, 10, 120, 50, 0);

	item->setName("AC Current");
	item->setRect(pathRect);
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(0, -300));
	item->addConnectionPoint(QPointF(0, 300));

	return item;
}
