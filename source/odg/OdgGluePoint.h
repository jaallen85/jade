// File: OdgGluePoint.h
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

#ifndef ODGGLUEPOINT_H
#define ODGGLUEPOINT_H

#include <QList>
#include <QPointF>
#include "OdgGlobal.h"

class OdgControlPoint;

class OdgGluePoint
{
private:
    QPointF mPosition;
    Odg::GluePointEscapeDirections mEscapeDirections;
    QList<OdgControlPoint*> mConnections;

public:
    OdgGluePoint(const QPointF& position = QPointF(), Odg::GluePointEscapeDirections escapeDirections = Odg::EscapeNone);
    ~OdgGluePoint();

    void setPosition(const QPointF& position);
    QPointF position() const;

    void setEscapeDirections(Odg::GluePointEscapeDirections directions);
    Odg::GluePointEscapeDirections escapeDirections() const;

    void addConnection(OdgControlPoint* point);
    void removeConnection(OdgControlPoint* point);
    void clearConnections();
    QList<OdgControlPoint*> connections() const;
};

#endif
