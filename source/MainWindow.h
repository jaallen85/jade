/* DemoWindow.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DEMOWINDOW_H
#define DEMOWINDOW_H

#include <DrawingWidget.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	enum ActionIndex { ExitAction,
		UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
		SelectAllAction, SelectNoneAction, RotateAction, RotateBackAction, FlipAction,
		BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
		InsertPointAction, RemovePointAction, GroupAction, UngroupAction,
		ZoomInAction, ZoomOutAction, ZoomFitAction,
		DefaultModeAction, ScrollModeAction, ZoomModeAction,
		PlaceLineAction, PlaceArcAction, PlacePolylineAction, PlaceCurveAction,
		PlaceRectAction, PlaceEllipseAction, PlacePolygonAction, PlaceTextAction,
		PlacePathAction, NumberOfActions };

private:
	DrawingWidget* mDrawingWidget;

	QActionGroup* mModeActionGroup;
	QByteArray mWindowState;

public:
	MainWindow();
	~MainWindow();

private slots:
	void setModeFromAction(QAction* action);
	void updateMode(DrawingWidget::Mode mode);

private:
	void showEvent(QShowEvent* event);
	void hideEvent(QHideEvent* event);

	void createActions();
	void createMenus();
	void createToolBars();
	void addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	void addModeAction(const QString& text,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
