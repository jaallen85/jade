/* MainWindow.cpp
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

#include "MainWindow.h"

MainWindow::MainWindow() : QMainWindow()
{
	mDrawingWidget = new DrawingWidget();
	setCentralWidget(mDrawingWidget);

	createActions();
	createMenus();
	createToolBars();

	setWindowTitle("Jade");
	setWindowIcon(QIcon(":/icons/jade/diagram.png"));
	resize(1019, 790);
}

MainWindow::~MainWindow() { }

//==================================================================================================

/*void MainWindow::setModeFromAction(QAction* action)
{
	QList<QAction*> actions = MainWindow::actions();

	if (action == actions[DefaultModeAction]) mDrawingWidget->setDefaultMode();
	else if (action == actions[ScrollModeAction]) mDrawingWidget->setScrollMode();
	else if (action == actions[ZoomModeAction]) mDrawingWidget->setZoomMode();
	else if (action == actions[PlaceLineAction]) mDrawingWidget->setPlaceMode(new DrawingLineItem());
	else if (action == actions[PlaceArcAction]) mDrawingWidget->setPlaceMode(new DrawingArcItem());
	else if (action == actions[PlacePolylineAction]) mDrawingWidget->setPlaceMode(new DrawingPolylineItem());
	else if (action == actions[PlaceCurveAction]) mDrawingWidget->setPlaceMode(new DrawingCurveItem());
	else if (action == actions[PlaceRectAction]) mDrawingWidget->setPlaceMode(new DrawingRectItem());
	else if (action == actions[PlaceEllipseAction]) mDrawingWidget->setPlaceMode(new DrawingEllipseItem());
	else if (action == actions[PlacePolygonAction]) mDrawingWidget->setPlaceMode(new DrawingPolygonItem());
	else if (action == actions[PlaceTextAction]) mDrawingWidget->setPlaceMode(new DrawingTextItem());
	else if (action == actions[PlacePathAction])
	{
		QPainterPath path;
		path.moveTo(-200, 0);
		path.lineTo(-150, 0);
		path.moveTo(-150, 0);
		path.lineTo(-125, -50);
		path.moveTo(-125, -50);
		path.lineTo(-75, 50);
		path.moveTo(-75, 50);
		path.lineTo(-25, -50);
		path.moveTo(-25, -50);
		path.lineTo(25, 50);
		path.moveTo(25, 50);
		path.lineTo(75, -50);
		path.moveTo(75, -50);
		path.lineTo(125, 50);
		path.moveTo(125, 50);
		path.lineTo(150, 0);
		path.moveTo(150, 0);
		path.lineTo(200, 0);

		DrawingPathItem* pathItem = new DrawingPathItem();
		pathItem->setPath(path, QRectF(-200, -50, 400, 100));
		pathItem->setRect(-200, -50, 400, 100);
		pathItem->addConnectionPoint(QPointF(-200, 0));
		pathItem->addConnectionPoint(QPointF(200, 0));
		pathItem->addConnectionPoint(QPointF(75, -50));
		mDrawingWidget->setPlaceMode(pathItem);
	}
}

void MainWindow::updateMode(DrawingWidget::Mode mode)
{
	if (mode == DrawingWidget::DefaultMode && !actions()[DefaultModeAction]->isChecked())
		actions()[DefaultModeAction]->setChecked(true);
}*/

//==================================================================================================

void MainWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);

	if (!event->spontaneous())
	{
		if (mDrawingWidget) mDrawingWidget->zoomFit();
	}
	else if (!mWindowState.isEmpty()) restoreState(mWindowState);
}

void MainWindow::hideEvent(QHideEvent* event)
{
	QMainWindow::hideEvent(event);

	if (event->spontaneous()) mWindowState = saveState();
}

//==================================================================================================

void MainWindow::createActions()
{
	addAction("Exit", this, SLOT(close()), ":/icons/oxygen/application-exit.png");

	/*mModeActionGroup = new QActionGroup(this);
	connect(mModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setModeFromAction(QAction*)));
	connect(mDrawingWidget, SIGNAL(modeChanged(DrawingWidget::Mode)), this, SLOT(updateMode(DrawingWidget::Mode)));

	addModeAction("Select Mode", ":/icons/oxygen/edit-select.png", "Escape");
	addModeAction("Scroll Mode", ":/icons/oxygen/transform-move.png", "");
	addModeAction("Zoom Mode", ":/icons/oxygen/page-zoom.png", "");
	addModeAction("Place Line", ":/icons/oxygen/draw-line.png", "");
	addModeAction("Place Arc", ":/icons/oxygen/draw-arc.png", "");
	addModeAction("Place Polyline", ":/icons/oxygen/draw-polyline.png", "");
	addModeAction("Place Curve", ":/icons/oxygen/draw-curve.png", "");
	addModeAction("Place Rect", ":/icons/oxygen/draw-rectangle.png", "");
	addModeAction("Place Ellipse", ":/icons/oxygen/draw-ellipse.png", "");
	addModeAction("Place Polygon", ":/icons/oxygen/draw-polygon.png", "");
	addModeAction("Place Text", ":/icons/oxygen/draw-text.png", "");
	addModeAction("Place Path", ":/icons/oxygen/resistor1.png", "");

	actions()[DefaultModeAction]->setChecked(true);*/
}

void MainWindow::createMenus()
{
	QList<QAction*> actions = MainWindow::actions();
	QList<QAction*> drawingActions = mDrawingWidget->actions();
	QMenu* menu;

	menu = menuBar()->addMenu("File");
	menu->addAction(actions[ExitAction]);

	menu = menuBar()->addMenu("Edit");
	menu->addAction(drawingActions[DrawingWidget::UndoAction]);
	menu->addAction(drawingActions[DrawingWidget::RedoAction]);
	menu->addSeparator();
	menu->addAction(drawingActions[DrawingWidget::CutAction]);
	menu->addAction(drawingActions[DrawingWidget::CopyAction]);
	menu->addAction(drawingActions[DrawingWidget::PasteAction]);
	menu->addAction(drawingActions[DrawingWidget::DeleteAction]);
	menu->addSeparator();
	menu->addAction(drawingActions[DrawingWidget::SelectAllAction]);
	menu->addAction(drawingActions[DrawingWidget::SelectNoneAction]);

	menu = menuBar()->addMenu("Object");
	menu->addAction(drawingActions[DrawingWidget::RotateAction]);
	menu->addAction(drawingActions[DrawingWidget::RotateBackAction]);
	menu->addAction(drawingActions[DrawingWidget::FlipAction]);
	menu->addSeparator();
	menu->addAction(drawingActions[DrawingWidget::InsertPointAction]);
	menu->addAction(drawingActions[DrawingWidget::RemovePointAction]);
	menu->addSeparator();
	menu->addAction(drawingActions[DrawingWidget::GroupAction]);
	menu->addAction(drawingActions[DrawingWidget::UngroupAction]);
	menu->addSeparator();
	menu->addAction(drawingActions[DrawingWidget::BringForwardAction]);
	menu->addAction(drawingActions[DrawingWidget::SendBackwardAction]);
	menu->addAction(drawingActions[DrawingWidget::BringToFrontAction]);
	menu->addAction(drawingActions[DrawingWidget::SendToBackAction]);

	menu = menuBar()->addMenu("View");
	menu->addAction(drawingActions[DrawingWidget::ZoomInAction]);
	menu->addAction(drawingActions[DrawingWidget::ZoomOutAction]);
	menu->addAction(drawingActions[DrawingWidget::ZoomFitAction]);
}

void MainWindow::createToolBars()
{
	QList<QAction*> actions = MainWindow::actions();
	QList<QAction*> drawingActions = mDrawingWidget->actions();
	QToolBar* toolBar;
	int size = QFontMetrics(font()).height() * 1.5;

	toolBar = new QToolBar("Diagram");
	toolBar->setObjectName("DiagramToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(drawingActions[DrawingWidget::RotateAction]);
	toolBar->addAction(drawingActions[DrawingWidget::RotateBackAction]);
	toolBar->addAction(drawingActions[DrawingWidget::FlipAction]);
	toolBar->addSeparator();
	toolBar->addAction(drawingActions[DrawingWidget::BringForwardAction]);
	toolBar->addAction(drawingActions[DrawingWidget::SendBackwardAction]);
	toolBar->addAction(drawingActions[DrawingWidget::BringToFrontAction]);
	toolBar->addAction(drawingActions[DrawingWidget::SendToBackAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("View");
	toolBar->setObjectName("ViewToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(drawingActions[DrawingWidget::ZoomInAction]);
	toolBar->addAction(drawingActions[DrawingWidget::ZoomOutAction]);
	toolBar->addAction(drawingActions[DrawingWidget::ZoomFitAction]);
	addToolBar(toolBar);
}

void MainWindow::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QMainWindow::addAction(action);
}

/*void MainWindow::addModeAction(const QString& text, const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);

	QMainWindow::addAction(action);
}*/
