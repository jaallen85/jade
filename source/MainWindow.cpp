// MainWindow.cpp
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

#include "MainWindow.h"
#include "DrawingBrowser.h"
#include "PropertiesBrowser.h"
#include <DrawingCurveItem.h>
#include <DrawingEllipseItem.h>
#include <DrawingLineItem.h>
#include <DrawingPathItem.h>
#include <DrawingPolygonItem.h>
#include <DrawingPolylineItem.h>
#include <DrawingRectItem.h>
#include <DrawingTextItem.h>
#include <DrawingTextEllipseItem.h>
#include <DrawingTextRectItem.h>
#include <DrawingWidget.h>
#include <QApplication>
#include <QDockWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(const QString& filePath) : QMainWindow()
{	
	// Main drawing widget
	mDrawingWidget = new DrawingWidget();
	mDrawingDefaultProperties = mDrawingWidget->properties();

	mStackedWidget = new QStackedWidget();
	mStackedWidget->addWidget(new QWidget());
	mStackedWidget->addWidget(mDrawingWidget);
	mStackedWidget->setCurrentIndex(0);
	setCentralWidget(mStackedWidget);

	// Drawing browser
	mDrawingBrowser = new DrawingBrowser();

	mDrawingDock = new QDockWidget("Diagram");
	mDrawingDock->setWidget(mDrawingBrowser);
	mDrawingDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mDrawingDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	mDrawingDock->setObjectName("PropertiesDock");
	addDockWidget(Qt::LeftDockWidgetArea, mDrawingDock);

	// Properties browser
	mPropertiesBrowser = new PropertiesBrowser();
	mPropertiesBrowser->setDrawingProperties(mDrawingWidget->properties());
	mPropertiesBrowser->setDefaultItemProperties(DrawingItem::factory.defaultItemProperties());
	mPropertiesBrowser->setItems(QList<DrawingItem*>());

	mPropertiesDock = new QDockWidget("Properties");
	mPropertiesDock->setWidget(mPropertiesBrowser);
	mPropertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mPropertiesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	mPropertiesDock->setObjectName("PropertiesDock");
	addDockWidget(Qt::RightDockWidgetArea, mPropertiesDock);

	// Connections
	//connect(mDrawingWidget, SIGNAL(propertiesTriggered()), mPropertiesDock, SLOT(show()));
	//connect(mPropertiesBrowser, SIGNAL(defaultItemPropertiesChanged(const QHash<QString,QVariant>&)),
	//	&DrawingItem::factory, SLOT(setDefaultItemProperties(const QHash<QString,QVariant>&)));
	//connectDrawingAndPropertiesBrowser();

	addActions();
	createMenus();
	createToolBars();
	createStatusBar();

	setWindowIcon(QIcon(":/icons/jade/jade.png"));
	resize(1354, 800);


}

MainWindow::~MainWindow()
{
	while (!mPathItems.isEmpty()) delete mPathItems.takeFirst();
}

//==================================================================================================

void MainWindow::setModeFromPlaceAction(QAction* action)
{
	QList<QAction*> modeActionList = mDrawingWidget->modeActions();

	if (action && mPlaceActions.contains(action))
	{
		DrawingItem* item = DrawingItem::factory.createItem(action->property("name").toString());

		if (item == nullptr)
		{
			QString pathName = action->property("pathName").toString();
			for(auto itemIter = mPathItems.begin(); item == nullptr && itemIter != mPathItems.end(); itemIter++)
			{
				if ((*itemIter)->pathName() == pathName) item = (*itemIter)->copy();
			}
		}

		if (item)
		{
			QList<DrawingItem*> items;
			items.append(item);
			mDrawingWidget->setPlaceMode(items);
		}
		else mDrawingWidget->setDefaultMode();
	}
}

//==================================================================================================

void MainWindow::addActions()
{
	addAction("New...", this, SLOT(newDrawing()), ":/icons/oxygen/document-new.png", "Ctrl+N");
	addAction("Open...", this, SLOT(openDrawing()), ":/icons/oxygen/document-open.png", "Ctrl+O");
	addAction("Save", this, SLOT(saveDrawing()), ":/icons/oxygen/document-save.png", "Ctrl+S");
	addAction("Save As...", this, SLOT(saveDrawingAs()), ":/icons/oxygen/document-save-as.png", "Ctrl+Shift+S");
	addAction("Close", this, SLOT(closeDrawing()), ":/icons/oxygen/document-close.png", "Ctrl+W");
	addAction("Export PNG...", this, SLOT(exportPng()), ":/icons/oxygen/image-x-generic.png");
	addAction("Export SVG...", this, SLOT(exportSvg()), ":/icons/oxygen/image-svg+xml.png");
	addAction("Preferences...", this, SLOT(preferences()), ":/icons/oxygen/configure.png");
	addAction("Exit", this, SLOT(close()), ":/icons/oxygen/application-exit.png");

	addAction("About...", this, SLOT(about()), ":/icons/oxygen/help-about.png");
	addAction("About Qt...", qApp, SLOT(aboutQt()));

	QList<QAction*> drawingActionList = mDrawingWidget->actions();
	drawingActionList[DrawingWidget::ZoomInAction]->setIcon(QIcon(":/icons/oxygen/zoom-in.png"));
	drawingActionList[DrawingWidget::ZoomOutAction]->setIcon(QIcon(":/icons/oxygen/zoom-out.png"));
	drawingActionList[DrawingWidget::ZoomFitAction]->setIcon(QIcon(":/icons/oxygen/zoom-fit-best.png"));
	drawingActionList[DrawingWidget::UndoAction]->setIcon(QIcon(":/icons/oxygen/edit-undo.png"));
	drawingActionList[DrawingWidget::RedoAction]->setIcon(QIcon(":/icons/oxygen/edit-redo.png"));
	drawingActionList[DrawingWidget::CutAction]->setIcon(QIcon(":/icons/oxygen/edit-cut.png"));
	drawingActionList[DrawingWidget::CopyAction]->setIcon(QIcon(":/icons/oxygen/edit-copy.png"));
	drawingActionList[DrawingWidget::PasteAction]->setIcon(QIcon(":/icons/oxygen/edit-paste.png"));
	drawingActionList[DrawingWidget::DeleteAction]->setIcon(QIcon(":/icons/oxygen/edit-delete.png"));
	drawingActionList[DrawingWidget::SelectAllAction]->setIcon(QIcon(":/icons/oxygen/edit-select-all.png"));
	drawingActionList[DrawingWidget::RotateAction]->setIcon(QIcon(":/icons/oxygen/object-rotate-right.png"));
	drawingActionList[DrawingWidget::RotateBackAction]->setIcon(QIcon(":/icons/oxygen/object-rotate-left.png"));
	drawingActionList[DrawingWidget::FlipHorizontalAction]->setIcon(QIcon(":/icons/oxygen/object-flip-horizontal.png"));
	drawingActionList[DrawingWidget::FlipVerticalAction]->setIcon(QIcon(":/icons/oxygen/object-flip-vertical.png"));
	drawingActionList[DrawingWidget::BringForwardAction]->setIcon(QIcon(":/icons/oxygen/object-bring-forward.png"));
	drawingActionList[DrawingWidget::SendBackwardAction]->setIcon(QIcon(":/icons/oxygen/object-send-backward.png"));
	drawingActionList[DrawingWidget::BringToFrontAction]->setIcon(QIcon(":/icons/oxygen/object-bring-to-front.png"));
	drawingActionList[DrawingWidget::SendToBackAction]->setIcon(QIcon(":/icons/oxygen/object-send-to-back.png"));
	drawingActionList[DrawingWidget::GroupAction]->setIcon(QIcon(":/icons/oxygen/merge.png"));
	drawingActionList[DrawingWidget::UngroupAction]->setIcon(QIcon(":/icons/oxygen/split.png"));
	drawingActionList[DrawingWidget::PropertiesAction]->setIcon(QIcon(":/icons/oxygen/games-config-board.png"));

	QList<QAction*> drawingModeActionList = mDrawingWidget->modeActions();
	drawingModeActionList[DrawingWidget::DefaultModeAction]->setText("Select Mode");
	drawingModeActionList[DrawingWidget::DefaultModeAction]->setIcon(QIcon(":/icons/oxygen/edit-select.png"));
	drawingModeActionList[DrawingWidget::ScrollModeAction]->setIcon(QIcon(":/icons/oxygen/transform-move.png"));
	drawingModeActionList[DrawingWidget::ZoomModeAction]->setIcon(QIcon(":/icons/oxygen/page-zoom.png"));

	addPlaceAction("Place Line", ":/icons/oxygen/draw-line.png", new DrawingLineItem());
	addPlaceAction("Place Curve", ":/icons/oxygen/draw-curve.png", new DrawingCurveItem());
	addPlaceAction("Place Polyline", ":/icons/oxygen/draw-polyline.png", new DrawingPolylineItem());
	addPlaceAction("Place Rectangle", ":/icons/oxygen/draw-rectangle.png", new DrawingRectItem());
	addPlaceAction("Place Ellipse", ":/icons/oxygen/draw-ellipse.png", new DrawingEllipseItem());
	addPlaceAction("Place Polygon", ":/icons/oxygen/draw-polygon.png", new DrawingPolygonItem());
	addPlaceAction("Place Text", ":/icons/oxygen/draw-text.png", new DrawingTextItem());
	addPlaceAction("Place Text Rectangle", ":/icons/items/textrect.png", new DrawingTextRectItem());
	addPlaceAction("Place Text Ellipse", ":/icons/items/textellipse.png", new DrawingTextEllipseItem());

	DrawingItem::factory.registerItem(new DrawingPathItem());
}

QAction* MainWindow::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QMainWindow::addAction(action);

	return action;
}

QAction* MainWindow::addPlaceAction(const QString& text, const QString& iconPath, DrawingItem* item)
{
	QAction* action = mDrawingWidget->addModeAction(text, iconPath);
	mPlaceActions.append(action);

	if (item)
	{
		DrawingPathItem* pathItem = dynamic_cast<DrawingPathItem*>(item);
		if (pathItem)
		{
			mPathItems.append(pathItem);
			action->setProperty("pathName", pathItem->pathName());
		}
		else
		{
			DrawingItem::factory.registerItem(item);
			action->setProperty("name", item->name());
		}
	}

	return action;
}

//==================================================================================================

void MainWindow::createMenus()
{
	QList<QAction*> actionList = actions();
	QList<QAction*> drawingActionList = mDrawingWidget->actions();
	QList<QAction*> drawingModeActionList = mDrawingWidget->modeActions();
	QMenu* menu;

	menu = menuBar()->addMenu("File");
	menu->addAction(actionList[NewAction]);
	menu->addAction(actionList[OpenAction]);
	menu->addSeparator();
	menu->addAction(actionList[SaveAction]);
	menu->addAction(actionList[SaveAsAction]);
	menu->addSeparator();
	menu->addAction(actionList[CloseAction]);
	menu->addSeparator();
	menu->addAction(actionList[ExportPngAction]);
	menu->addAction(actionList[ExportSvgAction]);
	menu->addSeparator();
	menu->addAction(actionList[PreferencesAction]);
	menu->addSeparator();
	menu->addAction(actionList[ExitAction]);

	menu = menuBar()->addMenu("Edit");
	menu->addAction(drawingActionList[DrawingWidget::UndoAction]);
	menu->addAction(drawingActionList[DrawingWidget::RedoAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::CutAction]);
	menu->addAction(drawingActionList[DrawingWidget::CopyAction]);
	menu->addAction(drawingActionList[DrawingWidget::PasteAction]);
	menu->addAction(drawingActionList[DrawingWidget::DeleteAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::SelectAllAction]);
	menu->addAction(drawingActionList[DrawingWidget::SelectNoneAction]);

	menu = menuBar()->addMenu("Place");
	menu->addAction(drawingModeActionList[DrawingWidget::DefaultModeAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::ScrollModeAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::ZoomModeAction]);
	menu->addSeparator();
	menu->addAction(mPlaceActions[MainWindow::PlaceLineAction]);
	menu->addAction(mPlaceActions[MainWindow::PlaceCurveAction]);
	menu->addAction(mPlaceActions[MainWindow::PlacePolylineAction]);
	menu->addAction(mPlaceActions[MainWindow::PlaceRectAction]);
	menu->addAction(mPlaceActions[MainWindow::PlaceEllipseAction]);
	menu->addAction(mPlaceActions[MainWindow::PlacePolygonAction]);
	menu->addAction(mPlaceActions[MainWindow::PlaceTextAction]);
	menu->addAction(mPlaceActions[MainWindow::PlaceTextRectAction]);
	menu->addAction(mPlaceActions[MainWindow::PlaceTextEllipseAction]);

	menu = menuBar()->addMenu("Object");
	menu->addAction(drawingActionList[DrawingWidget::RotateAction]);
	menu->addAction(drawingActionList[DrawingWidget::RotateBackAction]);
	menu->addAction(drawingActionList[DrawingWidget::FlipHorizontalAction]);
	menu->addAction(drawingActionList[DrawingWidget::FlipVerticalAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::BringForwardAction]);
	menu->addAction(drawingActionList[DrawingWidget::SendBackwardAction]);
	menu->addAction(drawingActionList[DrawingWidget::BringToFrontAction]);
	menu->addAction(drawingActionList[DrawingWidget::SendToBackAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::GroupAction]);
	menu->addAction(drawingActionList[DrawingWidget::UngroupAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::InsertPointAction]);
	menu->addAction(drawingActionList[DrawingWidget::RemovePointAction]);

	menu = menuBar()->addMenu("View");
	menu->addAction(drawingActionList[DrawingWidget::PropertiesAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::ZoomInAction]);
	menu->addAction(drawingActionList[DrawingWidget::ZoomOutAction]);
	menu->addAction(drawingActionList[DrawingWidget::ZoomFitAction]);

	menu = menuBar()->addMenu("About");
	menu->addAction(actionList[AboutAction]);
	menu->addAction(actionList[AboutQtAction]);
}

void MainWindow::createToolBars()
{
	// Zoom combo
	mZoomCombo = new QComboBox();
	mZoomCombo->setMinimumWidth(QFontMetrics(mZoomCombo->font()).boundingRect("XXXXXX.XX%").width() + 48);
	mZoomCombo->addItems(QStringList() << "Fit to Page" << "25%" << "50%" << "100%" << "150%" << "200%" << "300%" << "400%");
	mZoomCombo->setEditable(true);
	mZoomCombo->setCurrentIndex(3);

	connect(mDrawingWidget, SIGNAL(scaleChanged(qreal)), this, SLOT(setZoomComboText(qreal)));
	connect(mZoomCombo, SIGNAL(activated(const QString&)), this, SLOT(setZoomLevel(const QString&)));

	QWidget* zoomWidget = new QWidget();
	QHBoxLayout* zoomLayout = new QHBoxLayout();
	zoomLayout->addWidget(mZoomCombo);
	zoomLayout->setContentsMargins(2, 0, 2, 0);
	zoomWidget->setLayout(zoomLayout);

	// Toolbars
	QList<QAction*> actionList = actions();
	QList<QAction*> drawingActionList = mDrawingWidget->actions();
	QList<QAction*> drawingModeActionList = mDrawingWidget->modeActions();
	QToolBar* toolBar;
	int size = mZoomCombo->sizeHint().height();

	toolBar = new QToolBar("File");
	toolBar->setObjectName("FileToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(actionList[NewAction]);
	toolBar->addAction(actionList[OpenAction]);
	toolBar->addAction(actionList[SaveAction]);
	toolBar->addAction(actionList[CloseAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("Place");
	toolBar->setObjectName("PlaceToolBar");
	toolBar->setIconSize(QSize(size + 2, size + 2));
	toolBar->addAction(drawingModeActionList[DrawingWidget::DefaultModeAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::ScrollModeAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::ZoomModeAction]);
	toolBar->addSeparator();
	toolBar->addAction(mPlaceActions[MainWindow::PlaceLineAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlaceCurveAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlacePolylineAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlaceRectAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlaceEllipseAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlacePolygonAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlaceTextAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlaceTextRectAction]);
	toolBar->addAction(mPlaceActions[MainWindow::PlaceTextEllipseAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("Object");
	toolBar->setObjectName("ObjectToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(drawingActionList[DrawingWidget::RotateAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::RotateBackAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::FlipHorizontalAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::FlipVerticalAction]);
	toolBar->addSeparator();
	toolBar->addAction(drawingActionList[DrawingWidget::BringForwardAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::SendBackwardAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::BringToFrontAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::SendToBackAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("View");
	toolBar->setObjectName("ViewToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(drawingActionList[DrawingWidget::ZoomInAction]);
	toolBar->addWidget(zoomWidget);
	toolBar->addAction(drawingActionList[DrawingWidget::ZoomOutAction]);
	addToolBar(toolBar);
}

void MainWindow::createStatusBar()
{
	mModeLabel = new QLabel("Select Mode");
	mModeLabel->setMinimumWidth(QFontMetrics(mModeLabel->font()).boundingRect("Select Mode").width() + 64);
	statusBar()->addWidget(mModeLabel);
	connect(mDrawingWidget, SIGNAL(modeChanged(Drawing::Mode)), this, SLOT(setModeText(Drawing::Mode)));

	mModifiedLabel = new QLabel("");
	mModifiedLabel->setMinimumWidth(QFontMetrics(mModifiedLabel->font()).boundingRect("Modified").width() + 64);
	statusBar()->addWidget(mModifiedLabel);
	connect(mDrawingWidget, SIGNAL(cleanChanged(bool)), this, SLOT(setModifiedText(bool)));

	mMouseInfoLabel = new QLabel("");
	mMouseInfoLabel->setMinimumWidth(QFontMetrics(mMouseInfoLabel->font()).boundingRect("(XXXX.XX,XXXX.XX)").width() + 64);
	statusBar()->addWidget(mMouseInfoLabel);
	connect(mDrawingWidget, SIGNAL(mouseInfoChanged(const QString&)), mMouseInfoLabel, SLOT(setText(const QString&)));
	connect(mDrawingWidget, SIGNAL(mouseInfoChanged(const QPointF&)), this, SLOT(setMouseInfoText(const QPointF&)));
	connect(mDrawingWidget, SIGNAL(mouseInfoChanged(const QPointF&, const QPointF&)), this, SLOT(setMouseInfoText(const QPointF&, const QPointF&)));
}
