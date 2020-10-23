// DrawingPropertiesBrowser.h
// Copyright (C) 2020  Jason Allen
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

#ifndef DRAWINGPROPERTIESBROWSER_H
#define DRAWINGPROPERTIESBROWSER_H

#include <QHash>
#include <QVariant>
#include <QWidget>

class DrawingPropertiesBrowser : public QWidget
{
	Q_OBJECT

private:


public:
	DrawingPropertiesBrowser(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	virtual ~DrawingPropertiesBrowser();

public slots:
	void setDrawingProperties(const QHash<QString,QVariant>& properties);

signals:
	void drawingPropertiesChanged(const QHash<QString,QVariant>& properties);
};

#endif
