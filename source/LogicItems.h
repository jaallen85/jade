/* LogicItems.h
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

#ifndef LOGICITEMS_H
#define LOGICITEMS_H

#include <DrawingPathItem.h>

namespace LogicItems
{

QList<DrawingPathItem*> items();
QStringList icons();

DrawingPathItem* createAndGate();
DrawingPathItem* createOrGate();
DrawingPathItem* createXorGate();
DrawingPathItem* createXnorGate();
DrawingPathItem* createNandGate();
DrawingPathItem* createNorGate();
DrawingPathItem* createMultiplexer();
DrawingPathItem* createDemultiplexer();
DrawingPathItem* createBuffer();
DrawingPathItem* createNotGate();
DrawingPathItem* createTristateBuffer1();
DrawingPathItem* createTristateBuffer2();
DrawingPathItem* createFlipFlop1();
DrawingPathItem* createFlipFlop2();

}

#endif
