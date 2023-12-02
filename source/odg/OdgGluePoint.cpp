// File: OdgGluePoint.cpp
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

#include "OdgGluePoint.h"
#include "OdgControlPoint.h"

OdgGluePoint::OdgGluePoint(const QPointF& position) :
    mPosition(position), mConnections()
{
    // Nothing more to do here.
}

OdgGluePoint::~OdgGluePoint()
{
    clearConnections();
}

//======================================================================================================================

void OdgGluePoint::setPosition(const QPointF& position)
{
    mPosition = position;
}

QPointF OdgGluePoint::position() const
{
    return mPosition;
}

//======================================================================================================================

void OdgGluePoint::addConnection(OdgControlPoint* point)
{
    if (point) mConnections.append(point);
}

void OdgGluePoint::removeConnection(OdgControlPoint* point)
{
    if (point) mConnections.removeAll(point);
}

void OdgGluePoint::clearConnections()
{
    OdgControlPoint* connection;
    while (!mConnections.isEmpty())
    {
        connection = mConnections.takeLast();
        connection->setGluePoint(nullptr);
    }
}

QList<OdgControlPoint*> OdgGluePoint::connections() const
{
    return mConnections;
}
