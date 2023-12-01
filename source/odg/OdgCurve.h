// File: OdgCurve.h
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

#ifndef ODGCURVE_H
#define ODGCURVE_H

#include <QPointF>

class OdgCurve
{
private:
    QPointF mP1;
    QPointF mCP1;
    QPointF mCP2;
    QPointF mP2;

public:
    OdgCurve(const QPointF& p1 = QPointF(), const QPointF& cp1 = QPointF(),
             const QPointF& cp2 = QPointF(), const QPointF& p2 = QPointF());
    OdgCurve(const OdgCurve& other);

    void setP1(const QPointF& p1);
    void setCP1(const QPointF& cp1);
    void setCP2(const QPointF& cp2);
    void setP2(const QPointF& p2);
    QPointF p1() const;
    QPointF cp1() const;
    QPointF cp2() const;
    QPointF p2() const;

    void translate(const QPointF& delta);
    void scale(double scale);
    double length() const;
    double startAngle() const;
    double endAngle() const;
    QPointF center() const;

private:
    QPointF pointFromRatio(double ratio) const;

public:
    OdgCurve& operator=(const OdgCurve &other);
};

bool operator==(const OdgCurve& curve1, const OdgCurve& curve2);
bool operator!=(const OdgCurve& curve1, const OdgCurve& curve2);

#endif
