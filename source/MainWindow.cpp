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
#include "AboutDialog.h"
#include "DrawingBrowser.h"
#include "ElectricItems.h"
#include "ExportOptionsDialog.h"
#include "LogicItems.h"
#include "PreferencesDialog.h"
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
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStackedWidget>
#include <QStatusBar>
#include <QShowEvent>
#include <QToolBar>

MainWindow::MainWindow(const QString& filePath) : QMainWindow()
{	
	// Internal variables
	mApplicationName = "Jade";
	mFileFilter = "Jade Drawings (*.jdm);;All Files (*)";
	mFileSuffix = "jdm";
	mPromptOverwrite = true;
	mPromptCloseUnsaved = true;

	mNewCount = 0;
	mPropertiesDockVisibleOnClose = true;
#ifndef WIN32
	mWorkingDir = QDir::home();
#endif

	mExportSize = QSize();
	mExportMaintainAspectRatio = true;

	// Main drawing widget
	mDrawingWidget = new DrawingWidget();
	mDefaultDrawingProperties = mDrawingWidget->properties();

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
	mDrawingDock->hide();

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
	connect(mDrawingWidget, SIGNAL(modeActionChanged(QAction*)), this, SLOT(setModeFromPlaceAction(QAction*)));

	connect(mDrawingWidget, SIGNAL(propertiesTriggered()), mPropertiesDock, SLOT(show()));
	connect(mDrawingWidget, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
		mPropertiesBrowser, SLOT(setDrawingProperties(const QHash<QString,QVariant>&)));
	connect(mDrawingWidget, SIGNAL(itemsPropertiesChanged(const QList<DrawingItem*>&)),
		mPropertiesBrowser, SLOT(setItemsProperties(const QList<DrawingItem*>&)));
	connect(mDrawingWidget, SIGNAL(currentItemsChanged(const QList<DrawingItem*>&)),
		mPropertiesBrowser, SLOT(setItems(const QList<DrawingItem*>&)));

	connect(mPropertiesBrowser, SIGNAL(drawingPropertiesChanged(const QHash<QString,QVariant>&)),
		mDrawingWidget, SLOT(updateProperties(const QHash<QString,QVariant>&)));
	connect(mPropertiesBrowser, SIGNAL(defaultItemPropertiesChanged(const QHash<QString,QVariant>&)),
		&DrawingItem::factory, SLOT(setDefaultItemProperties(const QHash<QString,QVariant>&)));
	connect(mPropertiesBrowser, SIGNAL(itemsPropertiesChanged(const QHash< DrawingItem*, QHash<QString,QVariant> >&)),
		mDrawingWidget, SLOT(updateItemsProperties(const QHash< DrawingItem*, QHash<QString,QVariant> >&)));

	// Window setup
	addActions();
	createMenus();
	createToolBars();
	createStatusBar();

	setWindowIcon(QIcon(":/icons/jade/jade.png"));
	resize(1318, 800);

	loadSettings();
	connect(this, SIGNAL(drawingVisibilityChanged(bool)), this, SLOT(updateWindow(bool)));
	connect(this, SIGNAL(filePathChanged(const QString&)), this, SLOT(updateWindowTitle(const QString&)));

	connect(this, SIGNAL(drawingVisibilityChanged(bool)), this, SLOT(resetExportSize(bool)));
	connect(drawing(), SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)), this, SLOT(resetExportSize(const QHash<QString,QVariant>&)));

	if (!filePath.isEmpty()) openDrawing(filePath);
	else newDrawing();
}

MainWindow::~MainWindow()
{
	while (!mPathItems.isEmpty()) delete mPathItems.takeFirst();
}

//==================================================================================================

DrawingWidget* MainWindow::drawing() const
{
	return mDrawingWidget;
}

bool MainWindow::isDrawingVisible() const
{
	return (mDrawingWidget && mStackedWidget->currentIndex() == 1);
}

//==================================================================================================

void MainWindow::setApplicationName(const QString& name)
{
	mApplicationName = name;
	setWindowTitle(mFilePath);
}

void MainWindow::setFileDialogOptions(const QString& fileFilter, const QString& fileSuffix)
{
	mFileFilter = fileFilter;
	mFileSuffix = fileSuffix;
}

void MainWindow::setPromptOnOverwrite(bool prompt)
{
	mPromptOverwrite = prompt;
}

void MainWindow::setPromptOnClosingUnsaved(bool prompt)
{
	mPromptCloseUnsaved = prompt;
}

QString MainWindow::applicationName() const
{
	return mApplicationName;
}

QString MainWindow::fileFilter() const
{
	return mFileFilter;
}

QString MainWindow::fileSuffix() const
{
	return mFileSuffix;
}

bool MainWindow::shouldPromptOnOverwrite() const
{
	return mPromptOverwrite;
}

bool MainWindow::shouldPromptOnClosingUnsaved() const
{
	return mPromptCloseUnsaved;
}

QString MainWindow::filePath() const
{
	return mFilePath;
}

QDir MainWindow::workingDir() const
{
	return mWorkingDir;
}

//==================================================================================================

void MainWindow::setDefaultDrawingProperties(const QHash<QString,QVariant>& properties)
{
	mDefaultDrawingProperties = properties;
}

QHash<QString,QVariant> MainWindow::defaultDrawingProperties() const
{
	return mDefaultDrawingProperties;
}

//==================================================================================================

void MainWindow::saveSettings()
{
#ifdef RELEASE_BUILD
#ifdef WIN32
	QString configPath("config.ini");
#else
	QString configPath(QDir::home().absoluteFilePath(".jade/config.ini"));
#endif
#else
	QString configPath("config.ini");
#endif

	QSettings settings(configPath, QSettings::IniFormat);

	settings.beginGroup("Window");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.endGroup();

	settings.beginGroup("Prompts");
	settings.setValue("promptOnClosingUnsaved", shouldPromptOnClosingUnsaved());
	settings.setValue("promptOnOverwrite", shouldPromptOnOverwrite());
	settings.endGroup();

	QHash<QString,QVariant> properties = defaultDrawingProperties();
	settings.beginGroup("DrawingDefaults");
	settings.setValue("sceneRect", properties.value("sceneRect"));
	settings.setValue("backgroundBrush", properties.value("backgroundBrush"));
	settings.setValue("grid", properties.value("grid"));
	settings.setValue("gridStyle", properties.value("gridStyle"));
	settings.setValue("gridBrush", properties.value("gridBrush"));
	settings.setValue("gridSpacingMajor", properties.value("gridSpacingMajor"));
	settings.setValue("gridSpacingMinor", properties.value("gridSpacingMinor"));
	settings.setValue("dynamicGrid", properties.value("dynamicGrid"));
	settings.setValue("dynamicGridEnabled", properties.value("dynamicGridEnabled"));
	settings.endGroup();

	properties = DrawingItem::factory.defaultItemProperties();
	settings.beginGroup("ItemDefaults");
	settings.setValue("pen", properties.value("pen"));
	settings.setValue("brush", properties.value("brush"));
	settings.setValue("startArrow", properties.value("startArrow"));
	settings.setValue("endArrow", properties.value("endArrow"));
	settings.setValue("textBrush", properties.value("textBrush"));
	settings.setValue("font", properties.value("font"));
	settings.setValue("alignment", properties.value("alignment"));
	settings.endGroup();
}

void MainWindow::loadSettings()
{
#ifdef RELEASE_BUILD
#ifdef WIN32
	QString configPath("config.ini");
#else
	QString configPath(QDir::home().absoluteFilePath(".jade/config.ini"));
#endif
#else
	QString configPath("config.ini");
#endif

	QSettings settings(configPath, QSettings::IniFormat);

	settings.beginGroup("Window");
	restoreGeometry(settings.value("geometry", QVariant(QByteArray())).toByteArray());
	restoreState(settings.value("state", QVariant(QByteArray())).toByteArray());
	settings.endGroup();

	settings.beginGroup("Prompts");
	setPromptOnClosingUnsaved(settings.value("promptOnClosingUnsaved", QVariant(true)).toBool());
	setPromptOnOverwrite(settings.value("promptOnOverwrite", QVariant(true)).toBool());
	settings.endGroup();

	QHash<QString,QVariant> properties = defaultDrawingProperties();
	settings.beginGroup("DrawingDefaults");
	properties.insert("sceneRect", settings.value("sceneRect", QVariant(QRectF(-100, -100, 8200, 6200))));
	properties.insert("backgroundBrush", settings.value("backgroundBrush", QVariant(QBrush(QColor(255, 255, 255)))));
	properties.insert("grid", settings.value("grid", QVariant(50.0)));
	properties.insert("gridStyle", settings.value("gridStyle", QVariant((uint)Drawing::GridGraphPaper)));
	properties.insert("gridBrush", settings.value("gridBrush", QVariant(QBrush(QColor(0, 128, 128)))));
	properties.insert("gridSpacingMajor", settings.value("gridSpacingMajor", QVariant(8)));
	properties.insert("gridSpacingMinor", settings.value("gridSpacingMinor", QVariant(2)));
	properties.insert("dynamicGrid", settings.value("dynamicGrid", QVariant(1000.0)));
	properties.insert("dynamicGridEnabled", settings.value("dynamicGridEnabled", QVariant(false)));
	settings.endGroup();
	setDefaultDrawingProperties(properties);
	mPropertiesBrowser->setDrawingProperties(properties);

	QVariant startArrowVariant, endArrowVariant;
	startArrowVariant.setValue(DrawingArrow(Drawing::ArrowNone, 100.0));
	endArrowVariant.setValue(DrawingArrow(Drawing::ArrowNone, 100.0));

	properties = DrawingItem::factory.defaultItemProperties();
	settings.beginGroup("ItemDefaults");
	properties.insert("pen", settings.value("pen", QVariant(QPen(Qt::black, 12.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin))));
	properties.insert("brush", settings.value("brush", QVariant(QBrush(Qt::white))));
	properties.insert("startArrow", settings.value("startArrow", startArrowVariant));
	properties.insert("endArrow", settings.value("endArrow", endArrowVariant));
	properties.insert("textBrush", settings.value("textBrush", QVariant(QBrush(Qt::black))));
	properties.insert("font", settings.value("font", QVariant(QFont("Arial", 100))));
	properties.insert("alignment", settings.value("alignment", QVariant((uint)(Qt::AlignCenter))));
	settings.endGroup();
	DrawingItem::factory.setDefaultItemProperties(properties);
	mPropertiesBrowser->setDefaultItemProperties(properties);
}

//==================================================================================================

void MainWindow::newDrawing()
{
	// Close any open drawing first
	closeDrawing();

	// Create a new drawing only if there is no open drawing (i.e. close was successful or unneeded)
	if (mDrawingWidget && !isDrawingVisible())
	{
		clearDrawing();

		mNewCount++;
		mFilePath = "Untitled " + QString::number(mNewCount);
		mDrawingWidget->zoomFit();
		mStackedWidget->setCurrentIndex(1);
		mPropertiesDock->setVisible(mPropertiesDockVisibleOnClose);

		emit filePathChanged(mFilePath);
		emit drawingVisibilityChanged(true);
	}
}

void MainWindow::openDrawing(const QString& filePath)
{
	QString drawingPath = filePath;

	if (drawingPath.isEmpty())
	{
		// Prompt the user for the new file path to open
		QFileDialog::Options options = (mPromptOverwrite) ? QFileDialog::Options() : QFileDialog::DontConfirmOverwrite;
		drawingPath = mWorkingDir.path();
		drawingPath = QFileDialog::getOpenFileName(this, "Open File", drawingPath, mFileFilter, nullptr, options);
	}

	if (!drawingPath.isEmpty())
	{
		// Close any open drawing before proceeding
		closeDrawing();

		// Open an existing drawing only if there is no open drawing (i.e. close was successful or unneeded)
		if (mDrawingWidget && !isDrawingVisible())
		{
			clearDrawing();

			if (loadDrawingFromFile(drawingPath))
			{
				mFilePath = drawingPath;
				mDrawingWidget->zoomFit();
				mStackedWidget->setCurrentIndex(1);
				mPropertiesDock->setVisible(mPropertiesDockVisibleOnClose);

				emit filePathChanged(mFilePath);
				emit drawingVisibilityChanged(true);
			}
		}
	}
}

void MainWindow::saveDrawing(const QString& filePath)
{
	//emit filePathChanged(mFilePath);

	if (isDrawingVisible())
	{
		if (filePath.isEmpty() && mFilePath.startsWith("Untitled"))
		{
			// If no filePath is provided and mFilePath is invalid, then do a "save-as" instead
			saveDrawingAs();
		}
		else
		{
			// Use either the provided filePath or the cached mFilePath to save the drawing to file
			QString drawingPath = (!filePath.isEmpty()) ? filePath : mFilePath;

			if (saveDrawingToFile(drawingPath))
			{
				mFilePath = drawingPath;
				emit filePathChanged(mFilePath);
			}
		}
	}
}

void MainWindow::saveDrawingAs()
{
	if (isDrawingVisible())
	{
		QFileDialog::Options options = (mPromptOverwrite) ? QFileDialog::Options() : QFileDialog::DontConfirmOverwrite;
		QString filePath = (mFilePath.startsWith("Untitled")) ? mWorkingDir.path() : mFilePath;

		filePath = QFileDialog::getSaveFileName(this, "Save File", filePath, mFileFilter, nullptr, options);
		if (!filePath.isEmpty())
		{
			// Ensure that the selected filePath ends with the proper file suffix
			if (!filePath.endsWith("." + mFileSuffix, Qt::CaseInsensitive))
				filePath += "." + mFileSuffix;

			// Use the selected filePath to save the drawing to file
			if (saveDrawingToFile(filePath))
			{
				mFilePath = filePath;
				emit filePathChanged(mFilePath);
			}

			// Update the cached working directory
			QFileInfo fileInfo(filePath);
			mWorkingDir = fileInfo.dir();
		}
	}
}

void MainWindow::closeDrawing()
{
	if (isDrawingVisible())
	{
		bool proceedToClose = true;

		// If drawing has unsaved changes, prompt the user whether to save before closing
		if (mPromptCloseUnsaved && !mDrawingWidget->isClean())
		{
			QFileInfo fileInfo(mFilePath);

			QMessageBox::StandardButton  button = QMessageBox::question(this, "Save Changes",
				"Save changes to " + fileInfo.fileName() + " before closing?",
				QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);

			if (button == QMessageBox::Yes)
			{
				if (mFilePath.startsWith("Untitled")) saveDrawingAs();
				else saveDrawing();
			}

			proceedToClose = ((button == QMessageBox::Yes && mDrawingWidget->isClean()) ||
				button == QMessageBox::No);
		}

		if (proceedToClose)
		{
			// Hide the drawing widget and clear it to its default state
			mStackedWidget->setCurrentIndex(0);
			mFilePath = "";
			mPropertiesDockVisibleOnClose = mPropertiesDock->isVisible();
			mPropertiesDock->setVisible(false);

			clearDrawing();

			emit filePathChanged(mFilePath);
			emit drawingVisibilityChanged(false);
		}
	}
}

//==================================================================================================

void MainWindow::exportPng()
{
	if (isDrawingVisible())
	{
		QString filePath = MainWindow::filePath();
		QFileDialog::Options options =
			(shouldPromptOnOverwrite()) ? QFileDialog::Options() : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = workingDir().path();
		else filePath = filePath.left(filePath.length() - fileSuffix().length() - 1) + ".png";

		filePath = QFileDialog::getSaveFileName(this, "Export PNG", filePath,
			"Portable Network Graphics (*.png);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			if (!filePath.endsWith(".png", Qt::CaseInsensitive)) filePath += ".png";

			DrawingWidget* drawing = MainWindow::drawing();
			QRectF sceneRect = drawing->sceneRect();
			QSize exportSize = mExportSize;
			if (!exportSize.isValid()) exportSize = (sceneRect.size() / 4).toSize();

			ExportOptionsDialog exportDialog(drawing->sceneRect(), exportSize, mExportMaintainAspectRatio, this);
			if (exportDialog.exec() == ExportOptionsDialog::Accepted)
			{
				mExportSize = exportDialog.exportSize();
				mExportMaintainAspectRatio = exportDialog.maintainAspectRatio();

				QImage pngImage(mExportSize, QImage::Format_ARGB32);
				QPainter painter;

				drawing->clearSelection();

				painter.begin(&pngImage);
				painter.scale(pngImage.width() / sceneRect.width(), pngImage.height() / sceneRect.height());
				painter.translate(-sceneRect.left(), -sceneRect.top());
				painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
				drawing->renderExport(&painter);
				painter.end();

				pngImage.save(filePath, "PNG");
			}
		}
	}
}

void MainWindow::exportSvg()
{
	if (isDrawingVisible())
	{
		QString filePath = MainWindow::filePath();
		QFileDialog::Options options =
			(shouldPromptOnOverwrite()) ? QFileDialog::Options() : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = workingDir().path();
		else filePath = filePath.left(filePath.length() - fileSuffix().length() - 1) + ".svg";

		filePath = QFileDialog::getSaveFileName(this, "Export SVG", filePath,
			"Scalable Vector Graphics (*.svg);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			if (!filePath.endsWith(".svg", Qt::CaseInsensitive)) filePath += ".svg";

			DrawingWidget* drawing = MainWindow::drawing();
			QRectF sceneRect = drawing->sceneRect();
			QSize exportSize = mExportSize;
			if (!exportSize.isValid()) exportSize = (sceneRect.size() / 4).toSize();

			ExportOptionsDialog exportDialog(drawing->sceneRect(), exportSize, mExportMaintainAspectRatio, this);
			if (exportDialog.exec() == ExportOptionsDialog::Accepted)
			{
				mExportSize = exportDialog.exportSize();
				mExportMaintainAspectRatio = exportDialog.maintainAspectRatio();

				// Todo: SVG generation
			}
		}
	}
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

void MainWindow::updateWindow(bool drawingVisible)
{
	if (mDrawingWidget)
	{
		QList<QAction*> actionList = actions();
		if (actionList.size() >= NumberOfActions)
		{
			actionList[SaveAction]->setEnabled(drawingVisible);
			actionList[SaveAsAction]->setEnabled(drawingVisible);
			actionList[CloseAction]->setEnabled(drawingVisible);
			actionList[ExportPngAction]->setEnabled(drawingVisible);
			actionList[ExportSvgAction]->setEnabled(drawingVisible);
		}

		QList<QAction*> drawingActionList = mDrawingWidget->actions();
		for(auto actionIter = drawingActionList.begin(); actionIter != drawingActionList.end(); actionIter++)
			(*actionIter)->setEnabled(drawingVisible);

		QList<QAction*> modeActionList = mDrawingWidget->modeActions();
		for(auto actionIter = modeActionList.begin(); actionIter != modeActionList.end(); actionIter++)
			(*actionIter)->setEnabled(drawingVisible);

		if (mPropertiesBrowser)
			mPropertiesBrowser->setDrawingProperties(mDrawingWidget->properties());
	}
}

void MainWindow::updateWindowTitle(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	QString fileName = fileInfo.fileName();

	QMainWindow::setWindowTitle(fileName.isEmpty() ? mApplicationName : fileName + " - " + mApplicationName);
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

void MainWindow::setMouseInfoText(const QPointF& position)
{
	mMouseInfoLabel->setText(positionToString(position));
}

void MainWindow::setMouseInfoText(const QPointF& position1, const QPointF& position2)
{
	mMouseInfoLabel->setText(positionToString(position1) + " - " + positionToString(position2) + "  " +
		QString(QChar(0x394)) + " = " + positionToString(position2 - position1));
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

void MainWindow::resetExportSize(bool drawingVisible)
{
	mExportSize = QSize();
	Q_UNUSED(drawingVisible);
}

void MainWindow::resetExportSize(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("sceneRect")) mExportSize = QSize();
}

//==================================================================================================

bool MainWindow::saveDrawingToFile(const QString& filePath)
{
	QFile dataFile(filePath);

	bool fileError = !dataFile.open(QIODevice::WriteOnly);
	if (!fileError)
	{
		QXmlStreamWriter xml(&dataFile);
		xml.setAutoFormatting(true);
		xml.setAutoFormattingIndent(2);

		xml.writeStartDocument();
		xml.writeStartElement(mApplicationName.toLower() + "-drawing");
		mDrawingWidget->writeToXml(&xml);
		xml.writeEndElement();
		xml.writeEndDocument();

		dataFile.close();

		mDrawingWidget->setClean();
		mDrawingWidget->viewport()->update();
	}

	if (fileError)
	{
		QMessageBox::critical(this, "Error Saving File",
			"Unable to open file for saving.  File not saved: " + filePath);
	}

	return (!fileError);
}

bool MainWindow::loadDrawingFromFile(const QString& filePath)
{
	QFile dataFile(filePath);

	bool fileError = !dataFile.open(QIODevice::ReadOnly);
	if (!fileError)
	{
		clearDrawing();

		QXmlStreamReader xml(&dataFile);

		while (xml.readNextStartElement())
		{
			if (xml.name() == mApplicationName.toLower() + "-drawing")
				mDrawingWidget->readFromXml(&xml);
			else
				xml.skipCurrentElement();
		}

		dataFile.close();

		mDrawingWidget->setClean();
		mDrawingWidget->viewport()->update();

		mFilePath = filePath;
	}

	if (fileError)
	{
		QMessageBox::critical(this, "Error Reading File",
			"File could not be read. Please ensure that this file is valid: " + filePath);
	}

	return (!fileError);
}

void MainWindow::clearDrawing()
{
	if (mDrawingWidget)
	{
		mDrawingWidget->clearItems();
		mDrawingWidget->setProperties(mDefaultDrawingProperties);
		mDrawingWidget->setClean();
	}
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

void MainWindow::closeEvent(QCloseEvent* event)
{
	closeDrawing();

	if (!isDrawingVisible())
	{
		saveSettings();
		event->accept();
	}
	else event->ignore();
}

//==================================================================================================

QString MainWindow::positionToString(const QPointF& position) const
{
	return QString("(%1,%2)").arg(position.x(), 0, 'g', 5, QChar()).arg(position.y(), 0, 'g', 5, QChar());
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

	addAction("Reference Definitions...", mDrawingDock, SLOT(show()));

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
	mElectricItemsAction = addPathItems("Electric Items", ElectricItems::items(), ElectricItems::icons());
	mLogicItemsAction = addPathItems("Logic Items", LogicItems::items(), LogicItems::icons());
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

QAction* MainWindow::addPathItems(const QString& name, const QList<DrawingPathItem*>& items, const QStringList& icons)
{
	QAction* action = new QAction(QIcon(icons.first()), name, this);
	QMenu* menu = new QMenu(name);

	for(int i = 0; i < items.size() && i < icons.size(); i++)
		menu->addAction(addPlaceAction("Place " + items[i]->pathName(), icons[i], items[i]));

	action->setMenu(menu);
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
	menu->addSeparator();
	menu->addAction(mElectricItemsAction);
	menu->addAction(mLogicItemsAction);

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
	menu->addAction(actionList[DrawingBrowserAction]);
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
