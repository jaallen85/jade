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
    setItems(QList<OdgItem*>());
}

//======================================================================================================================

void OdgGroupItem::setItems(const QList<OdgItem*>& items)
{
    qDeleteAll(mItems);
    mItems = items;

    // Set control point positions to match items' bounding rect
    const QList<OdgControlPoint*> controlPoints = this->controlPoints();
    if (controlPoints.size() >= NumberOfControlPoints)
    {
        QRectF rect = boundingRect();
        QPointF center = rect.center();
        controlPoints.at(TopLeftControlPoint)->setPosition(QPointF(rect.left(), rect.top()));
        controlPoints.at(TopMiddleControlPoint)->setPosition(QPointF(center.x(), rect.top()));
        controlPoints.at(TopRightControlPoint)->setPosition(QPointF(rect.right(), rect.top()));
        controlPoints.at(MiddleRightControlPoint)->setPosition(QPointF(rect.right(), center.y()));
        controlPoints.at(BottomRightControlPoint)->setPosition(QPointF(rect.right(), rect.bottom()));
        controlPoints.at(BottomMiddleControlPoint)->setPosition(QPointF(center.x(), rect.bottom()));
        controlPoints.at(BottomLeftControlPoint)->setPosition(QPointF(rect.left(), rect.bottom()));
        controlPoints.at(MiddleLeftControlPoint)->setPosition(QPointF(rect.left(), center.y()));
    }
}

QList<OdgItem*> OdgGroupItem::items() const
{
    return mItems;
}

//======================================================================================================================

QRectF OdgGroupItem::boundingRect() const
{
    QRectF rect;
    for(auto& item : mItems)
        rect = rect.united(item->mapToScene(item->boundingRect()));
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
