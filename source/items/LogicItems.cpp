// LogicItems.cpp
// Copyright (C) 2020  Jason Allen
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

QList<DrawingPathItem*> LogicItems::items()
{
	QList<DrawingPathItem*> items;

	items << createAndGate() <<
		createOrGate() <<
		createXorGate() <<
		createXnorGate() <<
		createNandGate() <<
		createNorGate() <<
		createMultiplexer() <<
		createDemultiplexer() <<
		createBuffer() <<
		createNotGate() <<
		createTristateBuffer1() <<
		createTristateBuffer2() <<
		createFlipFlop1() <<
		createFlipFlop2();

	return items;
}

QStringList LogicItems::icons()
{
	QStringList icons;

	icons << ":/icons/items/and_gate.png" <<
		":/icons/items/or_gate.png" <<
		":/icons/items/xor_gate.png" <<
		":/icons/items/xnor_gate.png" <<
		":/icons/items/nand_gate.png" <<
		":/icons/items/nor_gate.png" <<
		":/icons/items/multiplexer.png" <<
		":/icons/items/demultiplexer.png" <<
		":/icons/items/buffer.png" <<
		":/icons/items/not_gate.png" <<
		":/icons/items/tristate_buffer.png" <<
		":/icons/items/tristate_buffer2.png" <<
		":/icons/items/flip_flop.png" <<
		":/icons/items/flip_flop2.png";

	return icons;
}

//==================================================================================================

DrawingPathItem* LogicItems::createAndGate()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -100, 400, 200);
	QPainterPath path;
	path.moveTo(-150, -100);
	path.cubicTo(250, -100, 250, 100, -150, 100);
	path.lineTo(-150, -100);

	path.moveTo(-200, -50); path.lineTo(-150, -50);
	path.moveTo(-200, 50); path.lineTo(-150, 50);
	path.moveTo(150, 0); path.lineTo(200, 0);

	item->setPathName("AND Gate");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -50));
	item->addConnectionPoint(QPointF(-200, 50));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createOrGate()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -100, 400, 200);
	QPainterPath path;
	path.moveTo(-150, -100);
	path.cubicTo(250, -100, 250, 100, -150, 100);
	path.cubicTo(-70, 100, -70, -100, -150, -100);

	path.moveTo(-200, -50); path.lineTo(-100, -50);
	path.moveTo(-200, 50); path.lineTo(-100, 50);
	path.moveTo(150, 0); path.lineTo(200, 0);

	item->setPathName("OR Gate");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -50));
	item->addConnectionPoint(QPointF(-200, 50));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createXorGate()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -100, 400, 200);
	QPainterPath path;
	path.moveTo(-150, -100);
	path.cubicTo(250, -100, 250, 100, -150, 100);
	path.cubicTo(-70, 100, -70, -100, -150, -100);

	path.moveTo(-200, -100);
	path.cubicTo(-120, -100, -120, 100, -200, 100);

	path.moveTo(-200, -50); path.lineTo(-150, -50);
	path.moveTo(-200, 50); path.lineTo(-150, 50);
	path.moveTo(150, 0); path.lineTo(200, 0);

	item->setPathName("XOR Gate");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -50));
	item->addConnectionPoint(QPointF(-200, 50));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createXnorGate()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -100, 400, 200);
	QPainterPath path;
	path.moveTo(-150, -100);
	path.cubicTo(250, -100, 250, 100, -150, 100);
	path.cubicTo(-70, 100, -70, -100, -150, -100);

	path.moveTo(-200, -100);
	path.cubicTo(-120, -100, -120, 100, -200, 100);

	path.moveTo(-200, -50); path.lineTo(-150, -50);
	path.moveTo(-200, 50); path.lineTo(-150, 50);
	path.moveTo(190, 0); path.lineTo(200, 0);

	path.moveTo(190, 0);
	path.cubicTo(190, -11.0457, 181.046, -20, 170, -20);
	path.cubicTo(158.954, -20, 150, -11.0457, 150, 0);
	path.cubicTo(150, 11.0457, 158.954, 20, 170, 20);
	path.cubicTo(181.046, 20, 190, 11.0457, 190, 0);

	item->setPathName("XNOR Gate");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -50));
	item->addConnectionPoint(QPointF(-200, 50));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createNandGate()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -100, 400, 200);
	QPainterPath path;
	path.moveTo(-150, -100);
	path.cubicTo(250, -100, 250, 100, -150, 100);
	path.lineTo(-150, -100);

	path.moveTo(-200, -50); path.lineTo(-150, -50);
	path.moveTo(-200, 50); path.lineTo(-150, 50);
	path.moveTo(190, 0); path.lineTo(200, 0);

	path.moveTo(190, 0);
	path.cubicTo(190, -11.0457, 181.046, -20, 170, -20);
	path.cubicTo(158.954, -20, 150, -11.0457, 150, 0);
	path.cubicTo(150, 11.0457, 158.954, 20, 170, 20);
	path.cubicTo(181.046, 20, 190, 11.0457, 190, 0);

	item->setPathName("NAND Gate");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -50));
	item->addConnectionPoint(QPointF(-200, 50));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createNorGate()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -100, 400, 200);
	QPainterPath path;
	path.moveTo(-150, -100);
	path.cubicTo(250, -100, 250, 100, -150, 100);
	path.cubicTo(-70, 100, -70, -100, -150, -100);

	path.moveTo(-200, -50); path.lineTo(-100, -50);
	path.moveTo(-200, 50); path.lineTo(-100, 50);
	path.moveTo(190, 0); path.lineTo(200, 0);

	path.moveTo(190, 0);
	path.cubicTo(190, -11.0457, 181.046, -20, 170, -20);
	path.cubicTo(158.954, -20, 150, -11.0457, 150, 0);
	path.cubicTo(150, 11.0457, 158.954, 20, 170, 20);
	path.cubicTo(181.046, 20, 190, 11.0457, 190, 0);

	item->setPathName("NOR Gate");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -50));
	item->addConnectionPoint(QPointF(-200, 50));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createMultiplexer()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;
	path.moveTo(-100, -200);
	path.lineTo(-100, 200);
	path.lineTo(100, 100);
	path.lineTo(100, -100);
	path.lineTo(-100, -200);

	path.moveTo(-200, -100); path.lineTo(-100, -100);
	path.moveTo(-200, 100); path.lineTo(-100, 100);
	path.moveTo(100, 0); path.lineTo(200, 0);

	item->setPathName("Multiplexer");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -100));
	item->addConnectionPoint(QPointF(-200, 100));
	item->addConnectionPoint(QPointF(200, 0));
	item->addConnectionPoint(QPointF(0, -150));
	item->addConnectionPoint(QPointF(0, 150));

	return item;
}

DrawingPathItem* LogicItems::createDemultiplexer()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;
	path.moveTo(100, -200);
	path.lineTo(100, 200);
	path.lineTo(-100, 100);
	path.lineTo(-100, -100);
	path.lineTo(100, -200);

	path.moveTo(200, -100); path.lineTo(100, -100);
	path.moveTo(200, 100); path.lineTo(100, 100);
	path.moveTo(-100, 0); path.lineTo(-200, 0);

	item->setPathName("Demultiplexer");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(200, -100));
	item->addConnectionPoint(QPointF(200, 100));
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(0, -150));
	item->addConnectionPoint(QPointF(0, 150));

	return item;
}

DrawingPathItem* LogicItems::createBuffer()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(50, 0); path.lineTo(200, 0);

	item->setPathName("Buffer");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createNotGate()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(90, 0); path.lineTo(200, 0);

	path.moveTo(90, 0);
	path.cubicTo(90, -11.0457, 81.0457, -20, 70, -20);
	path.cubicTo(58.9543, -20, 50, -11.0457, 50, 0);
	path.cubicTo(50, 11.0457, 58.9543, 20, 70, 20);
	path.cubicTo(81.0457, 20, 90, 11.0457, 90, 0);

	item->setPathName("NOT Gate");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));

	return item;
}

DrawingPathItem* LogicItems::createTristateBuffer1()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(50, 0); path.lineTo(200, 0);

	path.moveTo(0, -37.5); path.lineTo(0, -75);

	item->setPathName("Tristate Buffer 1");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));
	item->addConnectionPoint(QPointF(0, -75));

	return item;
}

DrawingPathItem* LogicItems::createTristateBuffer2()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -75, 400, 150);
	QPainterPath path;
	path.moveTo(-200, 0); path.lineTo(-50, 0);
	path.moveTo(50, 0); path.lineTo(-50, -75); path.lineTo(-50, 75); path.lineTo(50, 0);
	path.moveTo(90, 0); path.lineTo(200, 0);

	path.moveTo(90, 0);
	path.cubicTo(90, -11.0457, 81.0457, -20, 70, -20);
	path.cubicTo(58.9543, -20, 50, -11.0457, 50, 0);
	path.cubicTo(50, 11.0457, 58.9543, 20, 70, 20);
	path.cubicTo(81.0457, 20, 90, 11.0457, 90, 0);

	path.moveTo(0, -37.5); path.lineTo(0, -75);

	item->setPathName("Tristate Buffer 2");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, 0));
	item->addConnectionPoint(QPointF(200, 0));
	item->addConnectionPoint(QPointF(0, -75));

	return item;
}

DrawingPathItem* LogicItems::createFlipFlop1()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;
	path.moveTo(-150, -200); path.lineTo(-150, 200); path.lineTo(150, 200); path.lineTo(150, -200); path.lineTo(-150, -200);

	path.moveTo(-200, -100); path.lineTo(-150, -100);
	path.moveTo(-200, 100); path.lineTo(-150, 100);
	path.moveTo(200, -100); path.lineTo(150, -100);
	path.moveTo(200, 100); path.lineTo(150, 100);

	item->setPathName("Flip-Flop 1");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -100));
	item->addConnectionPoint(QPointF(-200, 100));
	item->addConnectionPoint(QPointF(0, -200));
	item->addConnectionPoint(QPointF(0, 200));
	item->addConnectionPoint(QPointF(200, -100));
	item->addConnectionPoint(QPointF(200, 100));

	return item;
}

DrawingPathItem* LogicItems::createFlipFlop2()
{
	DrawingPathItem* item = new DrawingPathItem();

	QRectF pathRect(-200, -200, 400, 400);
	QPainterPath path;
	path.moveTo(-150, -200); path.lineTo(-150, 200); path.lineTo(150, 200); path.lineTo(150, -200); path.lineTo(-150, -200);

	path.moveTo(-200, -100); path.lineTo(-150, -100);
	path.moveTo(-200, 100); path.lineTo(-150, 100);
	path.moveTo(200, -100); path.lineTo(150, -100);
	path.moveTo(200, 100); path.lineTo(150, 100);

	path.moveTo(-150, 50); path.lineTo(-100, 100);
	path.moveTo(-100, 100); path.lineTo(-150, 150);

	item->setPathName("Flip-Flop 2");
	item->setRect(QTransform().scale(1.25, 1.25).mapRect(pathRect));
	item->setPath(path, pathRect);
	item->addConnectionPoint(QPointF(-200, -100));
	item->addConnectionPoint(QPointF(-200, 100));
	item->addConnectionPoint(QPointF(0, -200));
	item->addConnectionPoint(QPointF(0, 200));
	item->addConnectionPoint(QPointF(200, -100));
	item->addConnectionPoint(QPointF(200, 100));

	return item;
}
