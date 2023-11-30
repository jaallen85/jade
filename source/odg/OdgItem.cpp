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
#include "OdgControlPoint.h"
#include "OdgGluePoint.h"

OdgItem::OdgItem() :
    mPosition(), mFlipped(false), mRotation(0), mTransform(), mTransformInverse(),
    mControlPoints(), mGluePoints(), mSelected(false)
{
    // Nothing more to do here.
}

OdgItem::~OdgItem()
{
    clearGluePoints();
    clearControlPoints();
}

//======================================================================================================================

void OdgItem::setPosition(const QPointF& position)
{
    mPosition = position;
    updateTransform();
}

void OdgItem::setFlipped(bool flipped)
{
    mFlipped = flipped;
    updateTransform();
}

void OdgItem::setRotation(int rotation)
{
    mRotation = rotation;
    updateTransform();
}

QPointF OdgItem::position() const
{
    return mPosition;
}

bool OdgItem::isFlipped() const
{
    return mFlipped;
}

int OdgItem::rotation() const
{
    return mRotation;
}

QTransform OdgItem::transform() const
{
    return mTransform;
}

QTransform OdgItem::transformInverse() const
{
    return mTransformInverse;
}

QPointF OdgItem::mapToScene(const QPointF& position) const
{
    return mTransform.map(position);
}

QRectF OdgItem::mapToScene(const QRectF& rect) const
{
    return QRectF(mapToScene(rect.topLeft()), mapToScene(rect.bottomRight()));
}

QPolygonF OdgItem::mapToScene(const QPolygonF& polygon) const
{
    return mTransform.map(polygon);
}

QPainterPath OdgItem::mapToScene(const QPainterPath& path) const
{
    return mTransform.map(path);
}

QPointF OdgItem::mapFromScene(const QPointF& position) const
{
    return mTransformInverse.map(position);
}

QRectF OdgItem::mapFromScene(const QRectF& rect) const
{
    return QRectF(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight()));
}

QPolygonF OdgItem::mapFromScene(const QPolygonF& polygon) const
{
    return mTransformInverse.map(polygon);
}

QPainterPath OdgItem::mapFromScene(const QPainterPath& path) const
{
    return mTransformInverse.map(path);
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

//======================================================================================================================

bool OdgItem::isValid() const
{
    return true;
}

//======================================================================================================================

void OdgItem::scaleBy(double scale)
{
    setPosition(QPointF(mPosition.x() * scale, mPosition.y() * scale));
    for(auto& controlPoint : qAsConst(mControlPoints))
        controlPoint->setPosition(QPointF(controlPoint->position().x() * scale, controlPoint->position().y() * scale));
}

//======================================================================================================================

void OdgItem::updateTransform()
{
    mTransform.reset();
    mTransform.translate(mPosition.x(), mPosition.y());
    if (mFlipped) mTransform.scale(-1, 1);
    mTransform.rotate(90 * mRotation);

    mTransformInverse = mTransform.inverted();
}
