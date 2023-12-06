// File: SingleItemPropertiesWidget.h
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

#ifndef SINGLEITEMPROPERTIESWIDGET_H
#define SINGLEITEMPROPERTIESWIDGET_H

#include <QWidget>
#include "OdgGlobal.h"

class OdgControlPoint;
class OdgItem;

class SingleItemPropertiesWidget : public QWidget
{
    Q_OBJECT

private:


public:
    SingleItemPropertiesWidget();

public slots:
    void setItem(OdgItem* item);
    void setUnits(Odg::Units units);
    void setUnits(int units);

signals:
    void itemMoved(const QPointF& position);
    void itemResized(OdgControlPoint* point, const QPointF& position);
    void itemResized2(OdgControlPoint* point1, const QPointF& p1, OdgControlPoint* point2, const QPointF& p2);
    void itemPropertyChanged(const QString& name, const QVariant& value);
};

#endif
