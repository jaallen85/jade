// File: OdgCurve.cpp
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

#include "OdgCurve.h"
#include <QLineF>
#include <QtMath>

OdgCurve::OdgCurve(const QPointF& p1, const QPointF& cp1, const QPointF& cp2, const QPointF& p2) :
    mP1(p1), mCP1(cp1), mCP2(cp2), mP2(p2)
{
    // Nothing more to do here.
}

OdgCurve::OdgCurve(const OdgCurve& other) :
    mP1(other.mP1), mCP1(other.mCP1), mCP2(other.mCP2), mP2(other.mP2)
{
    // Nothing more to do here.
}

//======================================================================================================================

void OdgCurve::setP1(const QPointF& p1)
{
    mP1 = p1;
}

void OdgCurve::setCP1(const QPointF& cp1)
{
    mCP1 = cp1;
}

void OdgCurve::setCP2(const QPointF& cp2)
{
    mCP2 = cp2;
}

void OdgCurve::setP2(const QPointF& p2)
{
    mP2 = p2;
}

QPointF OdgCurve::p1() const
{
    return mP1;
}

QPointF OdgCurve::cp1() const
{
    return mCP1;
}

QPointF OdgCurve::cp2() const
{
    return mCP2;
}

QPointF OdgCurve::p2() const
{
    return mP2;
}

//======================================================================================================================

void OdgCurve::translate(const QPointF& delta)
{
    mP1.setX(mP1.x() + delta.x());   mP1.setY(mP1.y() + delta.y());
    mCP1.setX(mCP1.x() + delta.x()); mCP1.setY(mCP1.y() + delta.y());
    mCP2.setX(mCP2.x() + delta.x()); mCP2.setY(mCP2.y() + delta.y());
    mP2.setX(mP2.x() + delta.x());   mP2.setY(mP2.y() + delta.y());
}

void OdgCurve::scale(double scale)
{
    mP1.setX(mP1.x() * scale);   mP1.setY(mP1.y() * scale);
    mCP1.setX(mCP1.x() * scale); mCP1.setY(mCP1.y() * scale);
    mCP2.setX(mCP2.x() * scale); mCP2.setY(mCP2.y() * scale);
    mP2.setX(mP2.x() * scale);   mP2.setY(mP2.y() * scale);
}

double OdgCurve::length() const
{
    return QLineF(mP1, mP2).length();
}

double OdgCurve::startAngle() const
{
    const QPointF p2 = pointFromRatio(0.05);
    return qRadiansToDegrees(qAtan2(mP1.y() - p2.y(), mP1.x() - p2.x()));
}

double OdgCurve::endAngle() const
{
    const QPointF p2 = pointFromRatio(0.95);
    return qRadiansToDegrees(qAtan2(mP2.y() - p2.y(), mP2.x() - p2.x()));
}

QPointF OdgCurve::center() const
{
    return QLineF(mP1, mP2).center();
}

//======================================================================================================================

QPointF OdgCurve::pointFromRatio(double ratio) const
{
    double x = ((1 - ratio) * (1 - ratio) * (1 - ratio) * mP1.x() +
                3 * ratio * (1 - ratio) * (1 - ratio) * mCP1.x() +
                3 * ratio * ratio * (1 - ratio) * mCP2.x() +
                ratio * ratio * ratio * mP2.x());
    double y = ((1 - ratio) * (1 - ratio) * (1 - ratio) * mP1.y() +
                3 * ratio * (1 - ratio) * (1 - ratio) * mCP1.y() +
                3 * ratio * ratio * (1 - ratio) * mCP2.y() +
                ratio * ratio * ratio * mP2.y());

    return QPointF(x, y);
}

//======================================================================================================================

OdgCurve& OdgCurve::operator=(const OdgCurve &other)
{
    mP1 = other.mP1;
    mCP1 = other.mCP1;
    mCP2 = other.mCP2;
    mP2 = other.mP2;
    return *this;
}

//======================================================================================================================

bool operator==(const OdgCurve& fontStyle1, const OdgCurve& fontStyle2)
{
    return (fontStyle1.p1() == fontStyle2.p1() &&
            fontStyle1.cp1() == fontStyle2.cp1() &&
            fontStyle1.cp2() == fontStyle2.cp2() &&
            fontStyle1.p2() == fontStyle2.p2());
}

bool operator!=(const OdgCurve& fontStyle1, const OdgCurve& fontStyle2)
{
    return (fontStyle1.p1() != fontStyle2.p1() ||
            fontStyle1.cp1() != fontStyle2.cp1() ||
            fontStyle1.cp2() != fontStyle2.cp2() ||
            fontStyle1.p2() != fontStyle2.p2());
}
