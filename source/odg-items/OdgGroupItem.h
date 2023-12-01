// File: OdgGroupItem.h
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

#ifndef ODGGROUPITEM_H
#define ODGGROUPITEM_H

#include "OdgItem.h"

class OdgGroupItem : public OdgItem
{
private:
    enum ControlPointIndex { TopLeftControlPoint, TopMiddleControlPoint, TopRightControlPoint, MiddleRightControlPoint,
                             BottomRightControlPoint, BottomMiddleControlPoint, BottomLeftControlPoint,
                             MiddleLeftControlPoint, NumberOfControlPoints };

private:
    QList<OdgItem*> mItems;

public:
    OdgGroupItem();
    ~OdgGroupItem();

    void setItems(const QList<OdgItem*>& items);
    QList<OdgItem*> items() const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool isValid() const override;

    virtual void paint(QPainter& painter) override;

    virtual void scaleBy(double scale) override;
};

#endif
