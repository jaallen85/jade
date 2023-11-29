// File: OdgItem.cpp
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

#include "OdgItem.h"

OdgItem::OdgItem() :
    mTransform(), mTransformInverse(), mControlPoints(), mGluePoints(), mSelected(false)
{
    // Nothing more to do here.
}

OdgItem::~OdgItem()
{
    clearGluePoints();
    clearControlPoints();
}

//======================================================================================================================

void OdgItem::setTransform(const QTransform& transform)
{
    if (transform.isInvertible())
    {
        mTransform = transform;
        mTransformInverse = transform.inverted();
    }
}

QTransform OdgItem::transform() const
{
    return mTransform;
}

QTransform OdgItem::transformInverse() const
{
    return mTransformInverse;
}

//======================================================================================================================

void OdgItem::addControlPoint(OdgControlPoint* point)
{
    if (point) mControlPoints.append(point);
}

void OdgItem::insertControlPoint(int index, OdgControlPoint* point)
{
    if (point) mControlPoints.insert(index, point);
}

void OdgItem::removeControlPoint(OdgControlPoint* point)
{
    if (point) mControlPoints.removeAll(point);
}

void OdgItem::clearControlPoints()
{
    qDeleteAll(mControlPoints);
    mControlPoints.clear();
}

QList<OdgControlPoint*> OdgItem::controlPoints() const
{
    return mControlPoints;
}

//======================================================================================================================

void OdgItem::addGluePoint(OdgGluePoint* point)
{
    if (point) mGluePoints.append(point);
}

void OdgItem::insertGluePoint(int index, OdgGluePoint* point)
{
    if (point) mGluePoints.insert(index, point);
}

void OdgItem::removeGluePoint(OdgGluePoint* point)
{
    if (point) mGluePoints.removeAll(point);
}

void OdgItem::clearGluePoints()
{
    qDeleteAll(mGluePoints);
    mGluePoints.clear();
}

QList<OdgGluePoint*> OdgItem::gluePoints() const
{
    return mGluePoints;
}

//======================================================================================================================

void OdgItem::setSelected(bool selected)
{
    mSelected = selected;
}

bool OdgItem::isSelected() const
{
    return mSelected;
}
