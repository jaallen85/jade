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
#include "MainToolBox.h"

MainWindow::MainWindow() : QMainWindow()
{
	mDrawingWidget = new DrawingWidget();
	setCentralWidget(mDrawingWidget);

	createMainToolBox();
	createPropertiesWidget();
	createStatusBar();


	createActions();
	createMenus();
	createToolBars();

	setWindowTitle("Jade");
	setWindowIcon(QIcon(":/icons/jade/diagram.png"));
	resize(1019, 790);
}

MainWindow::~MainWindow() { }

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

void MainWindow::createMainToolBox()
{
	mMainToolBox = new MainToolBox();
	mMainToolBoxDock = new QDockWidget("Tools");
	mMainToolBoxDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mMainToolBoxDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	mMainToolBoxDock->setWidget(mMainToolBox);
	addDockWidget(Qt::LeftDockWidgetArea, mMainToolBoxDock, Qt::Vertical);

	connect(mMainToolBox, SIGNAL(defaultModeTriggered()), mDrawingWidget, SLOT(setDefaultMode()));
	connect(mMainToolBox, SIGNAL(scrollModeTriggered()), mDrawingWidget, SLOT(setScrollMode()));
	connect(mMainToolBox, SIGNAL(zoomModeTriggered()), mDrawingWidget, SLOT(setZoomMode()));
	connect(mMainToolBox, SIGNAL(placeModeTriggered(DrawingItem*)), mDrawingWidget, SLOT(setPlaceMode(DrawingItem*)));
	connect(mMainToolBox, SIGNAL(propertiesTriggered()), mDrawingWidget, SLOT(properties()));
	connect(mDrawingWidget, SIGNAL(modeChanged(DrawingWidget::Mode)), mMainToolBox, SLOT(updateMode(DrawingWidget::Mode)));
}

void MainWindow::createPropertiesWidget()
{

}

void MainWindow::createStatusBar()
{

}

//==================================================================================================

void MainWindow::createActions()
{
	addAction("Exit", this, SLOT(close()), ":/icons/oxygen/application-exit.png");
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
