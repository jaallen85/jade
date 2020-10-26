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
#include "PreferencesDialog.h"
#include "AboutDialog.h"
#include <DrawingWidget.h>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(const QString& filePath) : DrawingWindow()
{
	setApplicationName("Jade");
	setFileDialogOptions("Jade Drawings (*.jdm);;All Files (*)", "jdm");

	addActions();
	createMenus();
	createToolBars();
	createStatusBar();

	setWindowIcon(QIcon(":/icons/jade/jade.png"));
	resize(1334, 800);

	loadSettings();
	connect(this, SIGNAL(drawingVisibilityChanged(bool)), this, SLOT(updateWindow(bool)));

	if (!filePath.isEmpty()) openDrawing(filePath);
	else newDrawing();
}

MainWindow::~MainWindow()
{

}

//==================================================================================================

void MainWindow::saveSettings()
{

}

void MainWindow::loadSettings()
{

}

//==================================================================================================

void MainWindow::exportPng()
{

}

void MainWindow::exportSvg()
{

}

void MainWindow::exportVsdx()
{

}

void MainWindow::exportOdg()
{

}

//==================================================================================================

void MainWindow::preferences()
{
	PreferencesDialog dialog(this);
	dialog.setPrompts(shouldPromptOnClosingUnsaved(), shouldPromptOnOverwrite());
	dialog.setDefaultDrawingProperties(defaultDrawingProperties());

	if (dialog.exec() == QDialog::Accepted)
	{
		setPromptOnClosingUnsaved(dialog.shouldPromptOnClosingUnsaved());
		setPromptOnOverwrite(dialog.shouldPromptOnOverwrite());
		setDefaultDrawingProperties(dialog.defaultDrawingProperties());
	}
}

void MainWindow::about()
{
	AboutDialog dialog(this);
	dialog.exec();
}

//==================================================================================================

void MainWindow::closeEvent(QCloseEvent* event)
{
	closeDrawing();

	if (!isDrawingWidgetVisible())
	{
		saveSettings();
		event->accept();
	}
	else event->ignore();
}

//==================================================================================================

void MainWindow::setZoomComboText(qreal scale)
{
	mZoomCombo->setCurrentText(QString::number(1000 * scale, 'g', 4) + "%");
}

void MainWindow::setZoomLevel(const QString& text)
{
	if (text == "Fit to Page")
	{
		drawing()->zoomFit();
	}
	else
	{
		QString numText = text;
		if (numText.endsWith("%")) numText = numText.left(numText.size() - 1);

		bool ok = false;
		qreal newScale = numText.toFloat(&ok);
		if (ok) drawing()->setScale(newScale / 1000);
	}
}

//==================================================================================================

void MainWindow::setModeText(Drawing::Mode mode)
{
	QString modeText = "Select Mode";

	switch (mode)
	{
	case Drawing::ScrollMode: modeText = "Scroll Mode"; break;
	case Drawing::ZoomMode: modeText = "Zoom Mode"; break;
	default: break;
	}

	mModeLabel->setText(modeText);
}

void MainWindow::setModifiedText(bool clean)
{
	mModifiedLabel->setText((clean) ? "" : "Modified");
}

//==================================================================================================

void MainWindow::updateWindow(bool drawingVisible)
{
	QList<QAction*> actionList = actions();
	actionList[ExportPngAction]->setEnabled(drawingVisible);
	actionList[ExportSvgAction]->setEnabled(drawingVisible);
	actionList[ExportOdgAction]->setEnabled(drawingVisible);
	actionList[ExportVsdxAction]->setEnabled(drawingVisible);
}

//==================================================================================================

void MainWindow::addActions()
{
	addAction("Export PNG...", this, SLOT(exportPng()), ":/icons/oxygen/image-x-generic.png");
	addAction("Export SVG...", this, SLOT(exportSvg()), ":/icons/oxygen/image-svg+xml.png");
	addAction("Export ODG...", this, SLOT(exportOdg()), ":/icons/oxygen/application-vnd.oasis.opendocument.graphics.png");
	addAction("Export VSDX...", this, SLOT(exportVsdx()), ":/icons/oxygen/application-msword.png");
	addAction("Preferences...", this, SLOT(preferences()), ":/icons/oxygen/configure.png");

	addAction("About...", this, SLOT(about()), ":/icons/oxygen/help-about.png");
	addAction("About Qt...", qApp, SLOT(aboutQt()));

	QList<QAction*> actionList = actions();
	actionList[NewAction]->setIcon(QIcon(":/icons/oxygen/document-new.png"));
	actionList[OpenAction]->setIcon(QIcon(":/icons/oxygen/document-open.png"));
	actionList[SaveAction]->setIcon(QIcon(":/icons/oxygen/document-save.png"));
	actionList[SaveAsAction]->setIcon(QIcon(":/icons/oxygen/document-save-as.png"));
	actionList[CloseAction]->setIcon(QIcon(":/icons/oxygen/document-close.png"));
	actionList[ExitAction]->setIcon(QIcon(":/icons/oxygen/application-exit.png"));

	QList<QAction*> drawingActionList = drawing()->actions();
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

	QList<QAction*> drawingModeActionList = drawing()->modeActions();
	drawingModeActionList[DrawingWidget::DefaultModeAction]->setText("Select Mode");
	drawingModeActionList[DrawingWidget::DefaultModeAction]->setIcon(QIcon(":/icons/oxygen/edit-select.png"));
	drawingModeActionList[DrawingWidget::ScrollModeAction]->setIcon(QIcon(":/icons/oxygen/transform-move.png"));
	drawingModeActionList[DrawingWidget::ZoomModeAction]->setIcon(QIcon(":/icons/oxygen/page-zoom.png"));
	drawingModeActionList[DrawingWidget::PlaceCurveAction]->setIcon(QIcon(":/icons/oxygen/draw-curve.png"));
	drawingModeActionList[DrawingWidget::PlaceEllipseAction]->setIcon(QIcon(":/icons/oxygen/draw-ellipse.png"));
	drawingModeActionList[DrawingWidget::PlaceLineAction]->setIcon(QIcon(":/icons/oxygen/draw-line.png"));
	drawingModeActionList[DrawingWidget::PlacePolygonAction]->setIcon(QIcon(":/icons/oxygen/draw-polygon.png"));
	drawingModeActionList[DrawingWidget::PlacePolylineAction]->setIcon(QIcon(":/icons/oxygen/draw-polyline.png"));
	drawingModeActionList[DrawingWidget::PlaceRectAction]->setIcon(QIcon(":/icons/oxygen/draw-rectangle.png"));
	drawingModeActionList[DrawingWidget::PlaceTextAction]->setIcon(QIcon(":/icons/oxygen/draw-text.png"));
	drawingModeActionList[DrawingWidget::PlaceTextEllipseAction]->setIcon(QIcon(":/icons/items/textellipse.png"));
	drawingModeActionList[DrawingWidget::PlaceTextRectAction]->setIcon(QIcon(":/icons/items/textrect.png"));
}

void MainWindow::createMenus()
{
	QList<QAction*> actionList = actions();
	QList<QAction*> drawingActionList = drawing()->actions();
	QList<QAction*> drawingModeActionList = drawing()->modeActions();
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
	menu->addAction(actionList[ExportVsdxAction]);
	menu->addAction(actionList[ExportOdgAction]);
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
	menu->addAction(drawingModeActionList[DrawingWidget::PlaceLineAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlaceCurveAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlacePolylineAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlaceRectAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlaceEllipseAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlacePolygonAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlaceTextAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlaceTextRectAction]);
	menu->addAction(drawingModeActionList[DrawingWidget::PlaceTextEllipseAction]);

	menu = menuBar()->addMenu("Object");
	menu->addAction(drawingActionList[DrawingWidget::RotateAction]);
	menu->addAction(drawingActionList[DrawingWidget::RotateBackAction]);
	menu->addAction(drawingActionList[DrawingWidget::FlipHorizontalAction]);
	menu->addAction(drawingActionList[DrawingWidget::FlipVerticalAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::InsertPointAction]);
	menu->addAction(drawingActionList[DrawingWidget::RemovePointAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::GroupAction]);
	menu->addAction(drawingActionList[DrawingWidget::UngroupAction]);
	menu->addSeparator();
	menu->addAction(drawingActionList[DrawingWidget::BringForwardAction]);
	menu->addAction(drawingActionList[DrawingWidget::SendBackwardAction]);
	menu->addAction(drawingActionList[DrawingWidget::BringToFrontAction]);
	menu->addAction(drawingActionList[DrawingWidget::SendToBackAction]);

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
	mZoomCombo = new QComboBox();
	mZoomCombo->setMinimumWidth(QFontMetrics(mZoomCombo->font()).boundingRect("XXXXXX.XX%").width() + 48);
	mZoomCombo->addItems(QStringList() << "Fit to Page" << "25%" << "50%" << "100%" << "150%" << "200%" << "300%" << "400%");
	mZoomCombo->setEditable(true);
	mZoomCombo->setCurrentIndex(3);

	connect(drawing(), SIGNAL(scaleChanged(qreal)), this, SLOT(setZoomComboText(qreal)));
	connect(mZoomCombo, SIGNAL(activated(const QString&)), this, SLOT(setZoomLevel(const QString&)));

	QWidget* zoomWidget = new QWidget();
	QHBoxLayout* zoomLayout = new QHBoxLayout();
	zoomLayout->addWidget(mZoomCombo);
	zoomLayout->setContentsMargins(2, 0, 2, 0);
	zoomWidget->setLayout(zoomLayout);

	QList<QAction*> actionList = actions();
	QList<QAction*> drawingActionList = drawing()->actions();
	QList<QAction*> drawingModeActionList = drawing()->modeActions();
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

	toolBar = new QToolBar("Edit");
	toolBar->setObjectName("EditToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(drawingActionList[DrawingWidget::UndoAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::RedoAction]);
	toolBar->addSeparator();
	toolBar->addAction(drawingActionList[DrawingWidget::CutAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::CopyAction]);
	toolBar->addAction(drawingActionList[DrawingWidget::PasteAction]);
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

	toolBar = new QToolBar("Place");
	toolBar->setObjectName("PlaceToolBar");
	toolBar->setIconSize(QSize(size + 2, size + 2));
	toolBar->addAction(drawingModeActionList[DrawingWidget::DefaultModeAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::ScrollModeAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::ZoomModeAction]);
	toolBar->addSeparator();
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlaceLineAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlaceCurveAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlacePolylineAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlaceRectAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlaceEllipseAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlacePolygonAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlaceTextAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlaceTextRectAction]);
	toolBar->addAction(drawingModeActionList[DrawingWidget::PlaceTextEllipseAction]);
	addToolBar(Qt::LeftToolBarArea, toolBar);
}

void MainWindow::createStatusBar()
{
	mModeLabel = new QLabel("Select Mode");
	mModeLabel->setMinimumWidth(QFontMetrics(mModeLabel->font()).boundingRect("Select Mode").width() + 64);
	statusBar()->addWidget(mModeLabel);
	connect(drawing(), SIGNAL(modeChanged(Drawing::Mode)), this, SLOT(setModeText(Drawing::Mode)));

	mModifiedLabel = new QLabel("");
	mModifiedLabel->setMinimumWidth(QFontMetrics(mModifiedLabel->font()).boundingRect("Modified").width() + 64);
	statusBar()->addWidget(mModifiedLabel);
	connect(drawing(), SIGNAL(cleanChanged(bool)), this, SLOT(setModifiedText(bool)));
}
