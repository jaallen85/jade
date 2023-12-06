// File: MultipleItemPropertiesWidget.h
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

#ifndef MULTIPLEITEMPROPERTIESWIDGET_H
#define MULTIPLEITEMPROPERTIESWIDGET_H

#include <QWidget>
#include "OdgGlobal.h"

class OdgItem;

class MultipleItemPropertiesWidget : public QWidget
{
    Q_OBJECT

private:


public:
    MultipleItemPropertiesWidget();


public slots:
    void setItems(const QList<OdgItem*>& items);
    void setUnits(Odg::Units units);
    void setUnits(int units);

signals:
    void itemsMovedDelta(const QPointF& delta);
    void itemsPropertyChanged(const QString& name, const QVariant& value);
};

#endif
