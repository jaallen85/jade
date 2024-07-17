// File: OdgLineItem.h
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

#ifndef ODGLINEITEM_H
#define ODGLINEITEM_H

#include "OdgItem.h"
#include "OdgMarker.h"
#include <QPen>

class OdgLineItem : public OdgItem
{
private:
    enum ControlPointIndex { StartControlPoint, EndControlPoint, NumberOfControlPoints };

private:
    QLineF mLine;

    QPen mPen;
    OdgMarker mStartMarker;
    OdgMarker mEndMarker;

public:
    OdgLineItem();

	OdgItem* copy() const override;

    void setLine(const QLineF& line);
    QLineF line() const;

    void setPen(const QPen& pen);
    void setStartMarker(const OdgMarker& marker);
    void setEndMarker(const OdgMarker& marker);
    QPen pen() const;
    OdgMarker startMarker() const;
    OdgMarker endMarker() const;

	void setProperty(const QString &name, const QVariant &value) override;
	QVariant property(const QString &name) const override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool isValid() const override;

    void paint(QPainter& painter) override;

	void resize(OdgControlPoint *point, const QPointF &position, bool snapTo45Degrees) override;

    void scaleBy(double scale) override;

	void placeCreateEvent(const QRectF& contentRect, double grid) override;
	OdgControlPoint* placeResizeStartPoint() const override;
	OdgControlPoint* placeResizeEndPoint() const override;

private:
    bool shouldShowMarker(double size) const;
    double startMarkerAngle() const;
    double endMarkerAngle() const;
};

#endif
