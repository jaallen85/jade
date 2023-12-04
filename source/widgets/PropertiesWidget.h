// File: PropertiesWidget.h
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

#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QStackedWidget>

class QScrollArea;
class DrawingWidget;
class DrawingPropertiesWidget;

class PropertiesWidget : public QStackedWidget
{
    Q_OBJECT

private:
    DrawingWidget* mDrawing;

    DrawingPropertiesWidget* mDrawingPropertiesWidget;
    QScrollArea* mDrawingPropertiesScrollArea;

public:
    PropertiesWidget(DrawingWidget* drawing);

public slots:
    void setAllDrawingProperties();
    void setDrawingProperty(const QString& name, const QVariant& value);
};

#endif
