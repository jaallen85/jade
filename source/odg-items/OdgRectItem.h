// File: OdgRectItem.h
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

#ifndef ODGRECTITEM_H
#define ODGRECTITEM_H

#include "OdgItem.h"
#include <QPen>

class OdgRectItem : public OdgItem
{
protected:
    enum ControlPointIndex { TopLeftControlPoint, TopMiddleControlPoint, TopRightControlPoint, MiddleRightControlPoint,
                             BottomRightControlPoint, BottomMiddleControlPoint, BottomLeftControlPoint,
                             MiddleLeftControlPoint, NumberOfControlPoints };
    enum GluePointIndex { TopGluePoint, RightGluePoint, BottomGluePoint, LeftGluePoint, NumberOfGluePoints };

protected:
    QRectF mRect;

    QBrush mBrush;
    QPen mPen;

public:
    OdgRectItem();

	virtual OdgItem* copy() const override;

	virtual void setRect(const QRectF& rect);
    QRectF rect() const;

    void setBrush(const QBrush& brush);
    void setPen(const QPen& pen);
    QBrush brush() const;
    QPen pen() const;

	virtual void setProperty(const QString &name, const QVariant &value) override;
	virtual QVariant property(const QString &name) const override;

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual bool isValid() const override;

    virtual void paint(QPainter& painter) override;

	virtual void resize(OdgControlPoint *point, const QPointF &position, bool snapTo45Degrees) override;

    virtual void scaleBy(double scale) override;

	virtual void placeCreateEvent(const QRectF& contentRect, double grid) override;
	virtual OdgControlPoint* placeResizeStartPoint() const override;
	virtual OdgControlPoint* placeResizeEndPoint() const override;
};

#endif
