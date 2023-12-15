// File: ElectricItems.h
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

#ifndef ELECTRICITEMS_H
#define ELECTRICITEMS_H

#include <QList>

class OdgPathItem;

class ElectricItems
{

public:
    static QList<OdgPathItem*> items();
    static QStringList icons();

private:
    static OdgPathItem* createResistor1();
    static OdgPathItem* createResistor2();
    static OdgPathItem* createCapacitor1();
    static OdgPathItem* createCapacitor2();
    static OdgPathItem* createInductor();

    static OdgPathItem* createDiode();
    static OdgPathItem* createZenerDiode();
    static OdgPathItem* createSchottkyDiode();

    static OdgPathItem* createNpnBjt();
    static OdgPathItem* createPnpBjt();
    static OdgPathItem* createNmosFet();
    static OdgPathItem* createPmosFet();

    static OdgPathItem* createGround1();
    static OdgPathItem* createGround2();

    static OdgPathItem* createOpAmp();

    static OdgPathItem* createLed();

    static OdgPathItem* createVdc();
    static OdgPathItem* createVac();
    static OdgPathItem* createIdc();
    static OdgPathItem* createIac();

    static OdgPathItem* createTransmissionLine();
};

#endif
