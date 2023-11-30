// File: OdgControlPoint.h
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

#ifndef ODGCONTROLPOINT_H
#define ODGCONTROLPOINT_H

#include <QPointF>

class OdgGluePoint;

class OdgControlPoint
{
private:
    QPointF mPosition;
    bool mConnectable;
    OdgGluePoint* mGluePoint;

public:
    OdgControlPoint(const QPointF& position = QPointF(), bool connectable = false);
    ~OdgControlPoint();

    void setPosition(const QPointF& position);
    QPointF position() const;

    void setConnectable(bool connectable);
    bool isConnectable() const;

    void setGluePoint(OdgGluePoint* point);
    OdgGluePoint* gluePoint() const;
};

#endif
