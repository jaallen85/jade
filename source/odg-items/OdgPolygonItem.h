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

	OdgItem* copy() const override;

    void setPolygon(const QPolygonF& polygon);
    QPolygonF polygon() const;

    void setBrush(const QBrush& brush);
    void setPen(const QPen& pen);
    QBrush brush() const;
    QPen pen() const;

	void setProperty(const QString &name, const QVariant &value) override;
	QVariant property(const QString &name) const override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool isValid() const override;

	void paint(QPainter& painter) override;

	void resize(OdgControlPoint *point, const QPointF &position, bool snapTo45Degrees) override;

	bool canInsertPoints() const override;
	bool canRemovePoints() const override;
	void insertPoint(const QPointF& position) override;
	void removePoint(const QPointF& position) override;

	void scaleBy(double scale) override;

	void placeCreateEvent(const QRectF& contentRect, double grid) override;
};

#endif
