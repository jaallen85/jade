/* MainWindow.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "MainWindow.h"
#include "DynamicPropertiesWidget.h"

MainWindow::MainWindow() : QMainWindow()
{
	// Settings
	fillDefaultPropertyValues();
	loadSettings();

	// Central widget
	mDiagramWidget = new DiagramWidget();
	mDiagramWidget->setFlags(DrawingWidget::UndoableSelectCommands);
	setCentralWidget(mDiagramWidget);

	// Properties widget and dock
	mPropertiesWidget = new DynamicPropertiesWidget(mDiagramWidget);
	mPropertiesDock = new QDockWidget("Properties");
	mPropertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mPropertiesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	mPropertiesDock->setWidget(mPropertiesWidget);
	addDockWidget(Qt::RightDockWidgetArea, mPropertiesDock);

	connect(mDiagramWidget, SIGNAL(selectionChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setSelectedItems(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(newItemChanged(DrawingItem*)), mPropertiesWidget, SLOT(setNewItem(DrawingItem*)));
	connect(mDiagramWidget, SIGNAL(itemsGeometryChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(updateGeometryFromSelectedItems(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(itemsStyleChanged()), mPropertiesWidget, SLOT(updateStylePropertiesFromSelectedItems()));
	connect(mDiagramWidget, SIGNAL(itemCornerRadiusChanged()), mPropertiesWidget, SLOT(updateCornerRadiusFromSelectedItem()));
	connect(mDiagramWidget, SIGNAL(itemCaptionChanged()), mPropertiesWidget, SLOT(updateCaptionFromSelectedItem()));
	connect(mDiagramWidget, SIGNAL(diagramPropertiesChanged()), mPropertiesWidget, SLOT(updatePropertiesFromDiagram()));

	connect(mPropertiesWidget, SIGNAL(selectedItemMoved(const QPointF&)), mDiagramWidget, SLOT(moveSelection(const QPointF&)));
	connect(mPropertiesWidget, SIGNAL(selectedItemResized(DrawingItemPoint*, const QPointF&)), mDiagramWidget, SLOT(resizeSelection(DrawingItemPoint*, const QPointF&)));
	connect(mPropertiesWidget, SIGNAL(selectedItemsStyleChanged(const QHash<DrawingItemStyle::Property,QVariant>&)),
		mDiagramWidget, SLOT(setSelectionStyleProperties(const QHash<DrawingItemStyle::Property,QVariant>&)));
	connect(mPropertiesWidget, SIGNAL(selectedItemCornerRadiusChanged(qreal,qreal)), mDiagramWidget, SLOT(setSelectionCornerRadius(qreal,qreal)));
	connect(mPropertiesWidget, SIGNAL(selectedItemCaptionChanged(const QString&)), mDiagramWidget, SLOT(setSelectionCaption(const QString&)));
	connect(mPropertiesWidget, SIGNAL(diagramPropertiesChanged(const QHash<DiagramWidget::Property,QVariant>&)),
		mDiagramWidget, SLOT(setProperties(const QHash<DiagramWidget::Property,QVariant>&)));

	connect(mDiagramWidget, SIGNAL(propertiesTriggered()), mPropertiesDock, SLOT(show()));

	// Status bar
	mModeLabel = new QLabel("Select Mode");
	mModifiedLabel = new QLabel("Modified");
	mNumberOfItemsLabel = new QLabel("0");
	mMouseInfoLabel = new QLabel("");
	mModeLabel->setMinimumWidth(QFontMetrics(mModeLabel->font()).width("Select Mode") + 64);
	mModifiedLabel->setMinimumWidth(QFontMetrics(mModifiedLabel->font()).width("Modified") + 64);
	mNumberOfItemsLabel->setMinimumWidth(QFontMetrics(mNumberOfItemsLabel->font()).width("888888") + 48);
	statusBar()->addWidget(mModeLabel);
	statusBar()->addWidget(mModifiedLabel);
	statusBar()->addWidget(mNumberOfItemsLabel);
	statusBar()->addWidget(mMouseInfoLabel, 100);

	connect(mDiagramWidget, SIGNAL(modeChanged(DrawingWidget::Mode)), this, SLOT(setModeText(DrawingWidget::Mode)));
	connect(mDiagramWidget, SIGNAL(cleanChanged(bool)), this, SLOT(setModifiedText(bool)));
	connect(mDiagramWidget, SIGNAL(numberOfItemsChanged(int)), this, SLOT(setNumberOfItemsText(int)));
	connect(mDiagramWidget, SIGNAL(mouseInfoChanged(const QString&)), mMouseInfoLabel, SLOT(setText(const QString&)));

	// Zoom combo
	mZoomCombo = new QComboBox();
	mZoomCombo->setMinimumWidth(QFontMetrics(mZoomCombo->font()).width("XXXXXX.XX%") + 48);
	mZoomCombo->addItems(QStringList() << "Fit to Page" << "25%" << "50%" << "100%" << "150%" << "200%" << "300%" << "400%");
	mZoomCombo->setEditable(true);
	mZoomCombo->setCurrentIndex(3);

	connect(mDiagramWidget, SIGNAL(scaleChanged(qreal)), this, SLOT(setZoomComboText(qreal)));
	connect(mZoomCombo, SIGNAL(activated(const QString&)), this, SLOT(setZoomLevel(const QString&)));

	// Final setup
	createActions();
	createMenus();
	createToolBars();

	setWindowTitle("Jade");
	setWindowIcon(QIcon(":/icons/jade/diagram.png"));
	resize(1290, 760);
}

MainWindow::~MainWindow() { }

//==================================================================================================

void MainWindow::fillDefaultPropertyValues()
{
	mDiagramDefaultProperties[DiagramWidget::SceneRect] = QRectF(-5000, -3750, 10000, 7500);
	mDiagramDefaultProperties[DiagramWidget::BackgroundColor] = QColor(255, 255, 255);
	mDiagramDefaultProperties[DiagramWidget::Grid] = 50;
	mDiagramDefaultProperties[DiagramWidget::GridStyle] = (quint32)(DiagramWidget::GridGraphPaper);
	mDiagramDefaultProperties[DiagramWidget::GridColor] = QColor(0, 128, 128);
	mDiagramDefaultProperties[DiagramWidget::GridSpacingMajor] = 8;
	mDiagramDefaultProperties[DiagramWidget::GridSpacingMinor] = 2;

	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenStyle, (uint)(Qt::SolidLine));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenColor, QColor(0, 0, 0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenOpacity, 1.0);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenWidth, 12.0);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenCapStyle, (uint)(Qt::RoundCap));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenJoinStyle, (uint)(Qt::RoundJoin));

	DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushStyle, (uint)(Qt::SolidPattern));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushColor, QColor(255, 255, 255));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushOpacity, 1.0);

	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontName, "Arial");
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontSize, 100.0);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontBold, false);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontItalic, false);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontUnderline, false);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontOverline, false);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontStrikeThrough, false);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextColor, QColor(0, 0, 0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextOpacity, 1.0);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextHorizontalAlignment, (uint)(Qt::AlignHCenter));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextVerticalAlignment, (uint)(Qt::AlignVCenter));

	DrawingItemStyle::setDefaultValue(DrawingItemStyle::StartArrowStyle, (uint)(DrawingItemStyle::ArrowNone));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::StartArrowSize, 100.0);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::EndArrowStyle, (uint)(DrawingItemStyle::ArrowNone));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::EndArrowSize, 100.0);
}

void MainWindow::loadSettings()
{

}

void MainWindow::saveSettings()
{

}

//==================================================================================================

void MainWindow::setModeFromAction(QAction* action)
{
	if (action->text() == "Scroll Mode") mDiagramWidget->setScrollMode();
	else if (action->text() == "Zoom Mode") mDiagramWidget->setZoomMode();
	else if (action->text() == "Place Arc") mDiagramWidget->setPlaceMode(new DrawingArcItem());
	else if (action->text() == "Place Curve") mDiagramWidget->setPlaceMode(new DrawingCurveItem());
	else if (action->text() == "Place Ellipse") mDiagramWidget->setPlaceMode(new DrawingEllipseItem());
	else if (action->text() == "Place Line") mDiagramWidget->setPlaceMode(new DrawingLineItem());
	else if (action->text() == "Place Polygon") mDiagramWidget->setPlaceMode(new DrawingPolygonItem());
	else if (action->text() == "Place Polyline") mDiagramWidget->setPlaceMode(new DrawingPolylineItem());
	else if (action->text() == "Place Rect") mDiagramWidget->setPlaceMode(new DrawingRectItem());
	else if (action->text() == "Place Text") mDiagramWidget->setPlaceMode(new DrawingTextItem());
	else if (action->text() == "Place Text Rect") mDiagramWidget->setPlaceMode(new DrawingTextRectItem());
	else if (action->text() == "Place Text Ellipse") mDiagramWidget->setPlaceMode(new DrawingTextEllipseItem());
	else if (action->text() == "Place Text Polygon") mDiagramWidget->setPlaceMode(new DrawingTextPolygonItem());
	else mDiagramWidget->setDefaultMode();
}

void MainWindow::setModeFromDiagram(DrawingWidget::Mode mode)
{
	switch (mode)
	{
	case DrawingWidget::ScrollMode:
		mModeActionGroup->actions()[ScrollModeAction]->setChecked(true);
		break;
	case DrawingWidget::ZoomMode:
		mModeActionGroup->actions()[ZoomModeAction]->setChecked(true);
		break;
	case DrawingWidget::DefaultMode:
		mModeActionGroup->actions()[DefaultModeAction]->setChecked(true);
		break;
	default:
		break;
	}
}

//==================================================================================================

void MainWindow::setModeText(DrawingWidget::Mode mode)
{
	QString modeText = "Select Mode";

	switch (mode)
	{
	case DrawingWidget::ScrollMode: modeText = "Scroll Mode"; break;
	case DrawingWidget::ZoomMode: modeText = "Zoom Mode"; break;
	case DrawingWidget::PlaceMode: modeText = "Place Mode"; break;
	default: break;
	}

	mModeLabel->setText(modeText);
}

void MainWindow::setModifiedText(bool clean)
{
	mModifiedLabel->setText((clean) ? "" : "Modified");
}

void MainWindow::setNumberOfItemsText(int itemCount)
{
	mNumberOfItemsLabel->setText(QString::number(itemCount));
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
		mDiagramWidget->zoomFit();
	}
	else
	{
		QString numText = text;
		if (numText.endsWith("%")) numText = numText.left(numText.size() - 1);

		bool ok = false;
		qreal newScale = numText.toFloat(&ok);
		if (ok)
		{
			newScale = newScale / 1000;

			mDiagramWidget->scaleBy(newScale / mDiagramWidget->scale());
			setZoomComboText(newScale);
		}
	}
}

//==================================================================================================

void MainWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);
	if (!event->spontaneous()) mDiagramWidget->zoomFit();
}

//==================================================================================================

void MainWindow::createActions()
{
	addAction("New...", this, SLOT(newDiagram()), ":/icons/oxygen/document-new.png", "Ctrl+N");
	addAction("Open...", this, SLOT(openDiagram()), ":/icons/oxygen/document-open.png", "Ctrl+O");
	addAction("Save", this, SLOT(saveDiagram()), ":/icons/oxygen/document-save.png", "Ctrl+S");
	addAction("Save As...", this, SLOT(saveDiagramAs()), ":/icons/oxygen/document-save-as.png", "Ctrl+Shift+S");
	addAction("Close", this, SLOT(closeDiagram()), ":/icons/oxygen/document-close.png", "Ctrl+W");
	addAction("Export PNG...", this, SLOT(exportPng()), ":/icons/oxygen/image-x-generic.png");
	addAction("Export SVG...", this, SLOT(exportSvg()), ":/icons/oxygen/image-svg+xml.png");
	addAction("Export ODG...", this, SLOT(exportOdg()), ":/icons/oxygen/application-vnd.oasis.opendocument.graphics.png");
	addAction("Print Preview...", this, SLOT(printPreview()), ":/icons/oxygen/document-preview.png");
	addAction("Print Setup...", this, SLOT(printSetup()), "");
	addAction("Print...", this, SLOT(printDiagram()), ":/icons/oxygen/document-print.png", "Ctrl+P");
	addAction("Print to PDF...", this, SLOT(printPdf()), ":/icons/oxygen/application-pdf.png");
	addAction("Preferences...", this, SLOT(preferences()), ":/icons/oxygen/configure.png");
	addAction("Exit", this, SLOT(close()), ":/icons/oxygen/application-exit.png");

	addAction("About...", this, SLOT(about()), ":/icons/oxygen/help-about.png");
	addAction("About Qt...", qApp, SLOT(aboutQt()));

	mModeActionGroup = new QActionGroup(this);
	connect(mModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setModeFromAction(QAction*)));
	connect(mDiagramWidget, SIGNAL(modeChanged(DrawingWidget::Mode)), this, SLOT(setModeFromDiagram(DrawingWidget::Mode)));

	addModeAction("Select Mode", ":/icons/oxygen/edit-select.png", "Escape");
	addModeAction("Scroll Mode", ":/icons/oxygen/transform-move.png", "");
	addModeAction("Zoom Mode", ":/icons/oxygen/page-zoom.png", "");
	addModeAction("Place Arc", ":/icons/oxygen/draw-arc.png", "");
	addModeAction("Place Curve", ":/icons/oxygen/draw-curve.png", "");
	addModeAction("Place Ellipse", ":/icons/oxygen/draw-ellipse.png", "");
	addModeAction("Place Line", ":/icons/oxygen/draw-line.png", "");
	addModeAction("Place Polygon", ":/icons/oxygen/draw-polygon.png", "");
	addModeAction("Place Polyline", ":/icons/oxygen/draw-polyline.png", "");
	addModeAction("Place Rect", ":/icons/oxygen/draw-rectangle.png", "");
	addModeAction("Place Text", ":/icons/oxygen/draw-text.png", "");
	addModeAction("Place Text Rect", ":/icons/items/textrect.png", "");
	addModeAction("Place Text Ellipse", ":/icons/items/textellipse.png", "");
	addModeAction("Place Text Polygon", ":/icons/items/textpolygon.png", "");

	mModeActionGroup->actions()[DefaultModeAction]->setChecked(true);
}

void MainWindow::createMenus()
{
	QList<QAction*> actions = MainWindow::actions();
	QList<QAction*> widgetActions = mDiagramWidget->actions();
	QList<QAction*> modeActions = mModeActionGroup->actions();
	QMenu* menu;

	menu = menuBar()->addMenu("File");
	menu->addAction(actions[NewAction]);
	menu->addAction(actions[OpenAction]);
	menu->addSeparator();
	menu->addAction(actions[SaveAction]);
	menu->addAction(actions[SaveAsAction]);
	menu->addSeparator();
	menu->addAction(actions[CloseAction]);
	menu->addSeparator();
	menu->addAction(actions[ExportPngAction]);
	menu->addAction(actions[ExportSvgAction]);
	menu->addAction(actions[ExportOdgAction]);
	menu->addSeparator();
	menu->addAction(actions[PrintPreviewAction]);
	menu->addAction(actions[PrintSetupAction]);
	menu->addAction(actions[PrintAction]);
	menu->addAction(actions[PrintPdfAction]);
	menu->addSeparator();
	menu->addAction(actions[PreferencesAction]);
	menu->addSeparator();
	menu->addAction(actions[ExitAction]);

	menu = menuBar()->addMenu("Edit");
	menu->addAction(widgetActions[DiagramWidget::UndoAction]);
	menu->addAction(widgetActions[DiagramWidget::RedoAction]);
	menu->addSeparator();
	menu->addAction(widgetActions[DiagramWidget::CutAction]);
	menu->addAction(widgetActions[DiagramWidget::CopyAction]);
	menu->addAction(widgetActions[DiagramWidget::PasteAction]);
	menu->addAction(widgetActions[DiagramWidget::DeleteAction]);
	menu->addSeparator();
	menu->addAction(widgetActions[DiagramWidget::SelectAllAction]);
	menu->addAction(widgetActions[DiagramWidget::SelectNoneAction]);

	menu = menuBar()->addMenu("Place");
	menu->addAction(modeActions[DefaultModeAction]);
	menu->addAction(modeActions[ScrollModeAction]);
	menu->addAction(modeActions[ZoomModeAction]);
	menu->addSeparator();
	menu->addAction(modeActions[PlaceArcAction]);
	menu->addAction(modeActions[PlaceCurveAction]);
	menu->addAction(modeActions[PlaceEllipseAction]);
	menu->addAction(modeActions[PlaceLineAction]);
	menu->addAction(modeActions[PlacePolylineAction]);
	menu->addAction(modeActions[PlacePolygonAction]);
	menu->addAction(modeActions[PlaceRectAction]);
	menu->addAction(modeActions[PlaceTextAction]);
	menu->addAction(modeActions[PlaceTextEllipseAction]);
	menu->addAction(modeActions[PlaceTextPolygonAction]);
	menu->addAction(modeActions[PlaceTextRectAction]);

	menu = menuBar()->addMenu("Object");
	menu->addAction(widgetActions[DiagramWidget::RotateAction]);
	menu->addAction(widgetActions[DiagramWidget::RotateBackAction]);
	menu->addAction(widgetActions[DiagramWidget::FlipAction]);
	menu->addSeparator();
	menu->addAction(widgetActions[DiagramWidget::InsertPointAction]);
	menu->addAction(widgetActions[DiagramWidget::RemovePointAction]);
	menu->addSeparator();
	menu->addAction(widgetActions[DiagramWidget::GroupAction]);
	menu->addAction(widgetActions[DiagramWidget::UngroupAction]);
	menu->addSeparator();
	menu->addAction(widgetActions[DiagramWidget::BringForwardAction]);
	menu->addAction(widgetActions[DiagramWidget::SendBackwardAction]);
	menu->addAction(widgetActions[DiagramWidget::BringToFrontAction]);
	menu->addAction(widgetActions[DiagramWidget::SendToBackAction]);

	menu = menuBar()->addMenu("View");
	menu->addAction(widgetActions[DiagramWidget::PropertiesAction]);
	menu->addSeparator();
	menu->addAction(widgetActions[DiagramWidget::ZoomInAction]);
	menu->addAction(widgetActions[DiagramWidget::ZoomOutAction]);
	menu->addAction(widgetActions[DiagramWidget::ZoomFitAction]);

	menu = menuBar()->addMenu("About");
	menu->addAction(actions[AboutAction]);
	menu->addAction(actions[AboutQtAction]);
}

void MainWindow::createToolBars()
{
	QWidget* zoomWidget = new QWidget();
	QHBoxLayout* zoomLayout = new QHBoxLayout();
	zoomLayout->addWidget(mZoomCombo);
	zoomLayout->setContentsMargins(2, 0, 2, 0);
	zoomWidget->setLayout(zoomLayout);

	QList<QAction*> actions = MainWindow::actions();
	QList<QAction*> widgetActions = mDiagramWidget->actions();
	QList<QAction*> modeActions = mModeActionGroup->actions();
	QToolBar* toolBar;
	//int size = QFontMetrics(font()).height() * 2;
	int size = mZoomCombo->sizeHint().height();

	toolBar = new QToolBar("File");
	toolBar->setObjectName("FileToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(actions[NewAction]);
	toolBar->addAction(actions[OpenAction]);
	toolBar->addAction(actions[SaveAction]);
	toolBar->addAction(actions[CloseAction]);
	toolBar->addSeparator();
	toolBar->addAction(actions[PrintAction]);
	toolBar->addAction(actions[PrintPreviewAction]);
	toolBar->addAction(actions[PrintPdfAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("Edit");
	toolBar->setObjectName("EditToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(widgetActions[DiagramWidget::UndoAction]);
	toolBar->addAction(widgetActions[DiagramWidget::RedoAction]);
	toolBar->addSeparator();
	toolBar->addAction(widgetActions[DiagramWidget::CutAction]);
	toolBar->addAction(widgetActions[DiagramWidget::CopyAction]);
	toolBar->addAction(widgetActions[DiagramWidget::PasteAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("Object");
	toolBar->setObjectName("ObjectToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(widgetActions[DiagramWidget::RotateAction]);
	toolBar->addAction(widgetActions[DiagramWidget::RotateBackAction]);
	toolBar->addAction(widgetActions[DiagramWidget::FlipAction]);
	toolBar->addSeparator();
	toolBar->addAction(widgetActions[DiagramWidget::BringForwardAction]);
	toolBar->addAction(widgetActions[DiagramWidget::SendBackwardAction]);
	toolBar->addAction(widgetActions[DiagramWidget::BringToFrontAction]);
	toolBar->addAction(widgetActions[DiagramWidget::SendToBackAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("View");
	toolBar->setObjectName("ViewToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(widgetActions[DiagramWidget::ZoomInAction]);
	toolBar->addWidget(zoomWidget);
	toolBar->addAction(widgetActions[DiagramWidget::ZoomOutAction]);
	addToolBar(toolBar);

	toolBar = new QToolBar("Place");
	toolBar->setObjectName("PlaceToolBar");
	toolBar->setIconSize(QSize(size + 2, size + 2));
	toolBar->addAction(modeActions[DefaultModeAction]);
	toolBar->addAction(modeActions[ScrollModeAction]);
	toolBar->addAction(modeActions[ZoomModeAction]);
	toolBar->addSeparator();
	toolBar->addAction(modeActions[PlaceLineAction]);
	toolBar->addAction(modeActions[PlaceArcAction]);
	toolBar->addAction(modeActions[PlacePolylineAction]);
	toolBar->addAction(modeActions[PlaceCurveAction]);
	toolBar->addAction(modeActions[PlaceRectAction]);
	toolBar->addAction(modeActions[PlaceEllipseAction]);
	toolBar->addAction(modeActions[PlacePolygonAction]);
	toolBar->addAction(modeActions[PlaceTextAction]);
	toolBar->addAction(modeActions[PlaceTextRectAction]);
	toolBar->addAction(modeActions[PlaceTextEllipseAction]);
	toolBar->addAction(modeActions[PlaceTextPolygonAction]);
	addToolBar(Qt::LeftToolBarArea, toolBar);
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

QAction* MainWindow::addModeAction(const QString& text, const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);

	return action;
}
