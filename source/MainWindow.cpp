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
#include <DrawingPathItem.h>
#include <DrawingShapeItems.h>
#include <DrawingTextItem.h>
#include <DrawingTwoPointItems.h>

MainWindow::MainWindow() : QMainWindow()
{
	mDrawingWidget = new DrawingWidget();
	setCentralWidget(mDrawingWidget);

	DrawingLineItem* lineItem = new DrawingLineItem();
	lineItem->setLine(0, 0, 400, 800);
	mDrawingWidget->addItem(lineItem);

	DrawingRectItem* rectItem = new DrawingRectItem();
	rectItem->setRect(800, 0, 400, 800);
	mDrawingWidget->addItem(rectItem);

	DrawingTextItem* textItem = new DrawingTextItem();
	textItem->setPos(1600, 0);
	mDrawingWidget->addItem(textItem);

	DrawingEllipseItem* ellipseItem = new DrawingEllipseItem();
	ellipseItem->setRect(-800, 0, 400, 800);
	mDrawingWidget->addItem(ellipseItem);

	createActions();
	createMenus();
	createToolBars();

	setWindowTitle("libdrawing Demo");
	resize(1019, 790);
}

MainWindow::~MainWindow() { }

//==================================================================================================

void MainWindow::setModeFromAction(QAction* action)
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
}

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

	addAction("Undo", mDrawingWidget, SLOT(undo()), ":/icons/oxygen/edit-undo.png", "Ctrl+Z");
	addAction("Redo", mDrawingWidget, SLOT(redo()), ":/icons/oxygen/edit-redo.png", "Ctrl+Shift+Z");
	addAction("Cut", mDrawingWidget, SLOT(cut()), ":/icons/oxygen/edit-cut.png", "Ctrl+X");
	addAction("Copy", mDrawingWidget, SLOT(copy()), ":/icons/oxygen/edit-copy.png", "Ctrl+C");
	addAction("Paste", mDrawingWidget, SLOT(paste()), ":/icons/oxygen/edit-paste.png", "Ctrl+V");
	addAction("Delete", mDrawingWidget, SLOT(deleteSelection()), ":/icons/oxygen/edit-delete.png", "Delete");
	addAction("Select All", mDrawingWidget, SLOT(selectAll()), ":/icons/oxygen/edit-select-all.png", "Ctrl+A");
	addAction("Select None", mDrawingWidget, SLOT(selectNone()), "", "Ctrl+Shift+A");

	addAction("Rotate", mDrawingWidget, SLOT(rotateSelection()), ":/icons/oxygen/object-rotate-right.png", "R");
	addAction("Rotate Back", mDrawingWidget, SLOT(rotateBackSelection()), ":/icons/oxygen/object-rotate-left.png", "Shift+R");
	addAction("Flip", mDrawingWidget, SLOT(flipSelection()), ":/icons/oxygen/object-flip-horizontal.png", "F");

	addAction("Bring Forward", mDrawingWidget, SLOT(bringForward()), ":/icons/oxygen/object-bring-forward.png");
	addAction("Send Backward", mDrawingWidget, SLOT(sendBackward()), ":/icons/oxygen/object-send-backward.png");
	addAction("Bring to Front", mDrawingWidget, SLOT(bringToFront()), ":/icons/oxygen/object-bring-to-front.png");
	addAction("Send to Back", mDrawingWidget, SLOT(sendToBack()), ":/icons/oxygen/object-send-to-back.png");

	addAction("Insert Point", mDrawingWidget, SLOT(insertItemPoint()), "");
	addAction("Remove Point", mDrawingWidget, SLOT(removeItemPoint()), "");

	addAction("Group", mDrawingWidget, SLOT(group()), ":/icons/oxygen/merge.png", "Ctrl+G");
	addAction("Ungroup", mDrawingWidget, SLOT(ungroup()), ":/icons/oxygen/split.png", "Ctrl+Shift+G");

	addAction("Zoom In", mDrawingWidget, SLOT(zoomIn()), ":/icons/oxygen/zoom-in.png", ".");
	addAction("Zoom Out", mDrawingWidget, SLOT(zoomOut()), ":/icons/oxygen/zoom-out.png", ",");
	addAction("Zoom Fit", mDrawingWidget, SLOT(zoomFit()), ":/icons/oxygen/zoom-fit-best.png", "/");

	mModeActionGroup = new QActionGroup(this);
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

	actions()[DefaultModeAction]->setChecked(true);
}

void MainWindow::createMenus()
{
	QList<QAction*> actions = MainWindow::actions();
	QMenu* menu;

	menu = menuBar()->addMenu("File");
	menu->addAction(actions[ExitAction]);

	menu = menuBar()->addMenu("Edit");
	menu->addAction(actions[UndoAction]);
	menu->addAction(actions[RedoAction]);
	menu->addSeparator();
	menu->addAction(actions[CutAction]);
	menu->addAction(actions[CopyAction]);
	menu->addAction(actions[PasteAction]);
	menu->addAction(actions[DeleteAction]);
	menu->addSeparator();
	menu->addAction(actions[SelectAllAction]);
	menu->addAction(actions[SelectNoneAction]);

	menu = menuBar()->addMenu("Place");
	menu->addAction(actions[DefaultModeAction]);
	menu->addAction(actions[ScrollModeAction]);
	menu->addAction(actions[ZoomModeAction]);
	menu->addSeparator();
	menu->addAction(actions[PlaceLineAction]);
	menu->addAction(actions[PlaceArcAction]);
	menu->addAction(actions[PlacePolylineAction]);
	menu->addAction(actions[PlaceCurveAction]);
	menu->addAction(actions[PlaceRectAction]);
	menu->addAction(actions[PlaceEllipseAction]);
	menu->addAction(actions[PlacePolygonAction]);
	menu->addAction(actions[PlaceTextAction]);
	menu->addAction(actions[PlacePathAction]);

	menu = menuBar()->addMenu("Object");
	menu->addAction(actions[RotateAction]);
	menu->addAction(actions[RotateBackAction]);
	menu->addAction(actions[FlipAction]);
	menu->addSeparator();
	menu->addAction(actions[InsertPointAction]);
	menu->addAction(actions[RemovePointAction]);
	menu->addSeparator();
	menu->addAction(actions[GroupAction]);
	menu->addAction(actions[UngroupAction]);
	menu->addSeparator();
	menu->addAction(actions[BringForwardAction]);
	menu->addAction(actions[SendBackwardAction]);
	menu->addAction(actions[BringToFrontAction]);
	menu->addAction(actions[SendToBackAction]);

	menu = menuBar()->addMenu("View");
	menu->addAction(actions[ZoomInAction]);
	menu->addAction(actions[ZoomOutAction]);
	menu->addAction(actions[ZoomFitAction]);
}

void MainWindow::createToolBars()
{
	QList<QAction*> actions = MainWindow::actions();
	QToolBar* toolBar;
	int size = QFontMetrics(font()).height() * 1.5;

	toolBar = new QToolBar("Place");
	toolBar->setObjectName("PlaceToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(actions[DefaultModeAction]);
	toolBar->addAction(actions[ScrollModeAction]);
	toolBar->addAction(actions[ZoomModeAction]);
	toolBar->addSeparator();
	toolBar->addAction(actions[PlaceLineAction]);
	toolBar->addAction(actions[PlaceArcAction]);
	toolBar->addAction(actions[PlacePolylineAction]);
	toolBar->addAction(actions[PlaceCurveAction]);
	toolBar->addAction(actions[PlaceRectAction]);
	toolBar->addAction(actions[PlaceEllipseAction]);
	toolBar->addAction(actions[PlacePolygonAction]);
	toolBar->addAction(actions[PlaceTextAction]);
	toolBar->addAction(actions[PlacePathAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("Diagram");
	toolBar->setObjectName("DiagramToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(actions[RotateAction]);
	toolBar->addAction(actions[RotateBackAction]);
	toolBar->addAction(actions[FlipAction]);
	toolBar->addSeparator();
	toolBar->addAction(actions[BringForwardAction]);
	toolBar->addAction(actions[SendBackwardAction]);
	toolBar->addAction(actions[BringToFrontAction]);
	toolBar->addAction(actions[SendToBackAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("View");
	toolBar->setObjectName("ViewToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(actions[ZoomInAction]);
	toolBar->addAction(actions[ZoomOutAction]);
	toolBar->addAction(actions[ZoomFitAction]);
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

void MainWindow::addModeAction(const QString& text, const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);

	QMainWindow::addAction(action);
}
