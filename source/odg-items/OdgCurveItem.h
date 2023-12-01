// File: OdgCurveItem.h
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

#ifndef ODGCURVEITEM_H
#define ODGCURVEITEM_H

#include "OdgCurve.h"
#include "OdgItem.h"
#include "OdgMarker.h"
#include <QPen>

class OdgCurveItem : public OdgItem
{
private:
    enum ControlPointIndex { StartControlPoint, StartControlBezierPoint, EndControlBezierPoint, EndControlPoint,
                             NumberOfControlPoints };

private:
    OdgCurve mCurve;

    QPen mPen;
    OdgMarker mStartMarker;
    OdgMarker mEndMarker;

    QPainterPath mCurvePath;

public:
    OdgCurveItem();

    void setCurve(const OdgCurve& curve);
    OdgCurve curve() const;

    void setPen(const QPen& pen);
    void setStartMarker(const OdgMarker& marker);
    void setEndMarker(const OdgMarker& marker);
    QPen pen() const;
    OdgMarker startMarker() const;
    OdgMarker endMarker() const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool isValid() const override;

    void paint(QPainter& painter) override;

    void scaleBy(double scale) override;

private:
    bool shouldShowMarker(double size) const;
    double startMarkerAngle() const;
    double endMarkerAngle() const;
};

#endif
