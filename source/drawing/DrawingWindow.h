// DrawingWindow.h
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

#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <QMainWindow>

class DrawingWidget;

class DrawingWindow : public QMainWindow
{
	Q_OBJECT

private:
	DrawingWidget* mDrawingWidget;

public:
	DrawingWindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	virtual ~DrawingWindow();

	DrawingWidget* drawing() const;

protected:
	virtual void showEvent(QShowEvent* event) override;
};

#endif
