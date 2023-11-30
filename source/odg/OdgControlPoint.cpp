// File: OdgControlPoint.cpp
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

#include "OdgControlPoint.h"
#include "OdgGluePoint.h"

OdgControlPoint::OdgControlPoint(const QPointF& position, bool connectable) :
    mPosition(position), mConnectable(connectable), mGluePoint(nullptr)
{
    // Nothing more to do here.
}

OdgControlPoint::~OdgControlPoint()
{
    setGluePoint(nullptr);
}

//======================================================================================================================

void OdgControlPoint::setPosition(const QPointF& position)
{
    mPosition = position;
}

QPointF OdgControlPoint::position() const
{
    return mPosition;
}

//======================================================================================================================

void OdgControlPoint::setConnectable(bool connectable)
{
    mConnectable = connectable;
}

bool OdgControlPoint::isConnectable() const
{
    return mConnectable;
}

//======================================================================================================================

void OdgControlPoint::setGluePoint(OdgGluePoint* point)
{
    if (mGluePoint) mGluePoint->removeConnection(this);
    mGluePoint = point;
    if (mGluePoint) mGluePoint->addConnection(this);
}

OdgGluePoint* OdgControlPoint::gluePoint() const
{
    return mGluePoint;
}
