// File: OdgGroupItem.cpp
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

#include "OdgGroupItem.h"
#include "OdgControlPoint.h"
#include <QPainter>

OdgGroupItem::OdgGroupItem() : mItems()
{
    for(int i = 0; i < NumberOfControlPoints; i++) addControlPoint(new OdgControlPoint(QPointF(0, 0)));
}

OdgGroupItem::~OdgGroupItem()
{
    qDeleteAll(mItems);
    mItems.clear();
}

//======================================================================================================================

OdgItem* OdgGroupItem::copy() const
{
	OdgGroupItem* groupItem = new OdgGroupItem();
	groupItem->setPosition(mPosition);
	groupItem->setRotation(mRotation);
	groupItem->setFlipped(mFlipped);
	groupItem->setItems(copyItems(mItems));
	return groupItem;
}

//======================================================================================================================

void OdgGroupItem::setItems(const QList<OdgItem*>& items)
{
    qDeleteAll(mItems);
    mItems = items;

    // Set control point positions to match items' bounding rect
    if (mControlPoints.size() >= NumberOfControlPoints)
    {
        QRectF rect = boundingRect();
        QPointF center = rect.center();
        mControlPoints.at(TopLeftControlPoint)->setPosition(QPointF(rect.left(), rect.top()));
        mControlPoints.at(TopMiddleControlPoint)->setPosition(QPointF(center.x(), rect.top()));
        mControlPoints.at(TopRightControlPoint)->setPosition(QPointF(rect.right(), rect.top()));
        mControlPoints.at(MiddleRightControlPoint)->setPosition(QPointF(rect.right(), center.y()));
        mControlPoints.at(BottomRightControlPoint)->setPosition(QPointF(rect.right(), rect.bottom()));
        mControlPoints.at(BottomMiddleControlPoint)->setPosition(QPointF(center.x(), rect.bottom()));
        mControlPoints.at(BottomLeftControlPoint)->setPosition(QPointF(rect.left(), rect.bottom()));
        mControlPoints.at(MiddleLeftControlPoint)->setPosition(QPointF(rect.left(), center.y()));
    }
}

QList<OdgItem*> OdgGroupItem::items() const
{
    return mItems;
}

//======================================================================================================================

void OdgGroupItem::setProperty(const QString &name, const QVariant &value)
{
	// Nothing to do here.
}

QVariant OdgGroupItem::property(const QString &name) const
{
	if (name == "position") return mPosition;
	return QVariant();
}

//======================================================================================================================

QRectF OdgGroupItem::boundingRect() const
{
    QRectF rect;
    for(auto& item : mItems)
        rect = rect.united(item->mapToScene(item->boundingRect()).normalized());
    return rect;
}

QPainterPath OdgGroupItem::shape() const
{
    QPainterPath shape;
    shape.addRect(boundingRect());
    return shape;
}

bool OdgGroupItem::isValid() const
{
    return (!mItems.isEmpty());
}

//======================================================================================================================

void OdgGroupItem::paint(QPainter& painter)
{
    for(auto& item : qAsConst(mItems))
    {
        painter.setTransform(item->transform(), true);
        item->paint(painter);
        painter.setTransform(item->transformInverse(), true);
    }
}

//======================================================================================================================

void OdgGroupItem::scaleBy(double scale)
{
    OdgItem::scaleBy(scale);

    for(auto& item : qAsConst(mItems))
        item->scaleBy(scale);
}
