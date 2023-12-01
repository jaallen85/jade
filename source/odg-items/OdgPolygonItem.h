// File: OdgPolygonItem.h
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

#ifndef ODGPOLYGONITEM_H
#define ODGPOLYGONITEM_H

#include "OdgItem.h"
#include <QPen>

class OdgPolygonItem : public OdgItem
{
private:
    QPolygonF mPolygon;

    QBrush mBrush;
    QPen mPen;

public:
    OdgPolygonItem();

    void setPolygon(const QPolygonF& polygon);
    QPolygonF polygon() const;

    void setBrush(const QBrush& brush);
    void setPen(const QPen& pen);
    QBrush brush() const;
    QPen pen() const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool isValid() const override;

    virtual void paint(QPainter& painter) override;

    virtual void scaleBy(double scale) override;
};

#endif
