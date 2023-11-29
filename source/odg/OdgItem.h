// File: OdgItem.h
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

#ifndef ODGITEM_H
#define ODGITEM_H

#include <QTransform>

class QPainter;
class OdgControlPoint;
class OdgGluePoint;

class OdgItem
{
private:
    QTransform mTransform, mTransformInverse;

    QList<OdgControlPoint*> mControlPoints;
    QList<OdgGluePoint*> mGluePoints;

    bool mSelected;

public:
    OdgItem();
    virtual ~OdgItem();

    void setTransform(const QTransform& transform);
    QTransform transform() const;
    QTransform transformInverse() const;

    void addControlPoint(OdgControlPoint* point);
    void insertControlPoint(int index, OdgControlPoint* point);
    void removeControlPoint(OdgControlPoint* point);
    void clearControlPoints();
    QList<OdgControlPoint*> controlPoints() const;

    void addGluePoint(OdgGluePoint* point);
    void insertGluePoint(int index, OdgGluePoint* point);
    void removeGluePoint(OdgGluePoint* point);
    void clearGluePoints();
    QList<OdgGluePoint*> gluePoints() const;

    void setSelected(bool selected);
    bool isSelected() const;

    virtual void paint(QPainter* painter) = 0;

    virtual void scaleBy(double scale) = 0;
};

#endif
