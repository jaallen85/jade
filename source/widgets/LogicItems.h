// File: LogicItems.h
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

#ifndef LOGICITEMS_H
#define LOGICITEMS_H

#include <QList>

class OdgPathItem;

class LogicItems
{

public:
    static QList<OdgPathItem*> items();
    static QStringList icons();

private:
    static OdgPathItem* createAndGate();
    static OdgPathItem* createOrGate();
    static OdgPathItem* createXorGate();
    static OdgPathItem* createXnorGate();
    static OdgPathItem* createNandGate();
    static OdgPathItem* createNorGate();

    static OdgPathItem* createBuffer();
    static OdgPathItem* createNotGate();

    static OdgPathItem* createMultiplexer();
    static OdgPathItem* createDemultiplexer();

    static OdgPathItem* createFlipFlop();
};

#endif
