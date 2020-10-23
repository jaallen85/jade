// DrawingWidget.h
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

#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <DrawingCanvas.h>
#include <QHash>
#include <QVariant>

class QActionGroup;

class DrawingWidget : public DrawingCanvas
{
	Q_OBJECT

private:


public:
	DrawingWidget(QWidget* parent = nullptr);
	virtual ~DrawingWidget();

	void setProperties(const QHash<QString,QVariant>& properties);
	QHash<QString,QVariant> properties() const;

signals:
	void propertiesTriggered();
	void propertiesChanged(const QHash<QString,QVariant>& properties);

public slots:
	void updateProperties(const QHash<QString,QVariant>& properties);
};

#endif
