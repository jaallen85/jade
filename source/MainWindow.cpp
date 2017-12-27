/* MainWindow.cpp
 *
 * Copyright (C) 2013-2017 Jason Allen
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
#include "DiagramWriter.h"
#include "DiagramReader.h"
#include "PreferencesDialog.h"
#include "AboutDialog.h"
#include "ExportOptionsDialog.h"
#include "ElectricItems.h"
#include "LogicItems.h"
#include "OdgWriter.h"

//#define RELEASE_BUILD
#undef RELEASE_BUILD

MainWindow::MainWindow(const QString& filePath) : QMainWindow()
{
	mPromptCloseUnsaved = true;
	mPromptOverwrite = true;
	mFileFilter = "Jade Drawings (*.jdm);;All Files (*)";
	mFileSuffix = "jdm";
	mNewDiagramCount = 0;
#ifndef WIN32
	mWorkingDir = QDir::home();
#endif

	mPrevMaintainAspectRatio = true;

	QMainWindow::setWindowTitle("Jade");
	setWindowIcon(QIcon(":/icons/jade/diagram.png"));
	resize(1290, 760);

	loadSettings();

	mDiagramWidget = new DiagramWidget();

	mStackedWidget = new QStackedWidget();
	mStackedWidget->addWidget(new QWidget());
	mStackedWidget->addWidget(mDiagramWidget);
	mStackedWidget->setCurrentIndex(0);
	setCentralWidget(mStackedWidget);

	createPropertiesDock();
	createStatusBar();

	createActions();
	createMenus();
	createToolBars();

	if (!filePath.isEmpty() && loadDiagramFromFile(filePath)) showDiagram();
	else newDiagram();
}

MainWindow::~MainWindow()
{
	while (!mPathItems.isEmpty()) delete mPathItems.takeFirst();
}

//==================================================================================================

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
	restoreGeometry(settings.value("geometry", QVariant()).toByteArray());
	restoreState(settings.value("state", QVariant()).toByteArray());
	settings.endGroup();

	settings.beginGroup("Prompts");
	mPromptCloseUnsaved = settings.value("promptOnClosingUnsaved", QVariant(true)).toBool();
	mPromptOverwrite = settings.value("promptOnOverwrite", QVariant(true)).toBool();
	settings.endGroup();

	settings.beginGroup("DrawingDefaults");
	mDiagramDefaultProperties[DiagramWidget::SceneRect] = settings.value("sceneRect", QVariant(QRectF(-5000, -3750, 10000, 7500))).toRectF();
	mDiagramDefaultProperties[DiagramWidget::BackgroundColor] = settings.value("backgroundColor", QVariant(QColor(255, 255, 255))).value<QColor>();
	mDiagramDefaultProperties[DiagramWidget::Grid] = settings.value("grid", QVariant(50.0)).toDouble();
	mDiagramDefaultProperties[DiagramWidget::GridStyle] = settings.value("gridStyle", QVariant((uint)DiagramWidget::GridGraphPaper)).toUInt();
	mDiagramDefaultProperties[DiagramWidget::GridSpacingMajor] = settings.value("gridSpacingMajor", QVariant(8)).toInt();
	mDiagramDefaultProperties[DiagramWidget::GridSpacingMinor] = settings.value("gridSpacingMinor", QVariant(2)).toInt();
	mDiagramDefaultProperties[DiagramWidget::GridColor] = settings.value("gridColor", QVariant(QColor(0, 128, 128))).value<QColor>();
	settings.endGroup();

	settings.beginGroup("ItemDefaults");
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenStyle, settings.value("penStyle", (uint)(Qt::SolidLine)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenWidth, settings.value("penWidth", 12.0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenColor, settings.value("penColor", QColor(0, 0, 0)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenOpacity, settings.value("penOpacity", 1.0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenCapStyle, (uint)(Qt::RoundCap));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenJoinStyle, (uint)(Qt::RoundJoin));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushStyle, (uint)(Qt::SolidPattern));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushColor, settings.value("brushColor", QColor(255, 255, 255)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushOpacity, settings.value("brushOpacity", 1.0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontName, settings.value("fontName", "Arial"));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontSize, settings.value("fontSize", 100.0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontBold, settings.value("fontBold", false));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontItalic, settings.value("fontItalic", false));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontUnderline, settings.value("fontUnderline", false));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontOverline, false);
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontStrikeThrough, settings.value("fontStrikeThrough", false));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextHorizontalAlignment, settings.value("textAlignHorizontal", (uint)(Qt::AlignHCenter)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextVerticalAlignment, settings.value("textAlignVertical", (uint)(Qt::AlignVCenter)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextColor, settings.value("textColor", QColor(0, 0, 0)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextOpacity, settings.value("textOpacity", 1.0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::StartArrowStyle, settings.value("startArrowStyle", (uint)(DrawingItemStyle::ArrowNone)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::StartArrowSize, settings.value("startArrowSize", 100.0));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::EndArrowStyle, settings.value("endArrowStyle", (uint)(DrawingItemStyle::ArrowNone)));
	DrawingItemStyle::setDefaultValue(DrawingItemStyle::EndArrowSize, settings.value("endArrowSize", 100.0));
	settings.endGroup();

	settings.beginGroup("Recent");
	if (settings.contains("workingDir"))
	{
		QDir newDir(settings.value("workingDir").toString());
		if (newDir.exists()) mWorkingDir = newDir;
	}
	settings.endGroup();

	mPrinter.setPageOrientation(QPageLayout::Landscape);
	mPrinter.setPageSize(QPageSize(QPageSize::Letter));
	mPrinter.setPageMargins(QMarginsF(0.5, 0.5, 0.5, 0.5), QPageLayout::Inch);
	mPrinter.setResolution(600);

	/*settings.beginGroup("Printer");
	mPrinter.setPageSize((QPrinter::PageSize)settings.value("pageSize", (int)QPrinter::Letter).toInt());
	mPrinter.setPageMargins(QMarginsF(
		settings.value("pageMarginLeft", 0.5).toReal(), settings.value("pageMarginTop", 0.5).toReal(),
		settings.value("pageMarginRight", 0.5).toReal(), settings.value("pageMarginBottom", 0.5).toReal()),
		(QPageLayout::Unit)settings.value("pageMarginUnits", (int)QPageLayout::Inch).toInt());
	mPrinter.setPageOrientation(
		(QPageLayout::Orientation)settings.value("pageOrientation", (int)QPageLayout::Landscape).toInt());
	mPrinter.setResolution(settings.value("resolution", 600).toInt());
	settings.endGroup();*/
}

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
	settings.setValue("promptOnClosingUnsaved", mPromptCloseUnsaved);
	settings.setValue("promptOnOverwrite", mPromptOverwrite);
	settings.endGroup();

	settings.beginGroup("DrawingDefaults");
	settings.setValue("sceneRect", mDiagramDefaultProperties[DiagramWidget::SceneRect].toRectF());
	settings.setValue("backgroundColor", mDiagramDefaultProperties[DiagramWidget::BackgroundColor]);
	settings.setValue("grid", mDiagramDefaultProperties[DiagramWidget::Grid]);
	settings.setValue("gridStyle",mDiagramDefaultProperties[DiagramWidget::GridStyle]);
	settings.setValue("gridSpacingMajor", mDiagramDefaultProperties[DiagramWidget::GridSpacingMajor]);
	settings.setValue("gridSpacingMinor", mDiagramDefaultProperties[DiagramWidget::GridSpacingMinor]);
	settings.setValue("gridColor", mDiagramDefaultProperties[DiagramWidget::GridColor]);
	settings.endGroup();

	settings.beginGroup("ItemDefaults");
	settings.setValue("penStyle", DrawingItemStyle::defaultValue(DrawingItemStyle::PenStyle));
	settings.setValue("penWidth", DrawingItemStyle::defaultValue(DrawingItemStyle::PenWidth));
	settings.setValue("penColor", DrawingItemStyle::defaultValue(DrawingItemStyle::PenColor));
	settings.setValue("penOpacity", DrawingItemStyle::defaultValue(DrawingItemStyle::PenOpacity));
	settings.setValue("brushColor", DrawingItemStyle::defaultValue(DrawingItemStyle::BrushColor));
	settings.setValue("brushOpacity", DrawingItemStyle::defaultValue(DrawingItemStyle::BrushOpacity));
	settings.setValue("fontName", DrawingItemStyle::defaultValue(DrawingItemStyle::FontName));
	settings.setValue("fontSize", DrawingItemStyle::defaultValue(DrawingItemStyle::FontSize));
	settings.setValue("fontBold", DrawingItemStyle::defaultValue(DrawingItemStyle::FontBold));
	settings.setValue("fontItalic", DrawingItemStyle::defaultValue(DrawingItemStyle::FontItalic));
	settings.setValue("fontUnderline", DrawingItemStyle::defaultValue(DrawingItemStyle::FontUnderline));
	settings.setValue("fontStrikeThrough", DrawingItemStyle::defaultValue(DrawingItemStyle::FontStrikeThrough));
	settings.setValue("textAlignHorizontal", DrawingItemStyle::defaultValue(DrawingItemStyle::TextHorizontalAlignment));
	settings.setValue("textAlignVertical", DrawingItemStyle::defaultValue(DrawingItemStyle::TextVerticalAlignment));
	settings.setValue("textColor", DrawingItemStyle::defaultValue(DrawingItemStyle::TextColor));
	settings.setValue("textOpacity", DrawingItemStyle::defaultValue(DrawingItemStyle::TextOpacity));
	settings.setValue("startArrowStyle", DrawingItemStyle::defaultValue(DrawingItemStyle::StartArrowStyle));
	settings.setValue("startArrowSize", DrawingItemStyle::defaultValue(DrawingItemStyle::StartArrowSize));
	settings.setValue("endArrowStyle", DrawingItemStyle::defaultValue(DrawingItemStyle::EndArrowStyle));
	settings.setValue("endArrowSize", DrawingItemStyle::defaultValue(DrawingItemStyle::EndArrowSize));
	settings.endGroup();

	settings.beginGroup("Recent");
	settings.setValue("workingDir", mWorkingDir.absolutePath());
	settings.endGroup();

	settings.beginGroup("Printer");
	settings.setValue("pageSize", (int)mPrinter.pageSize());
	settings.setValue("pageMarginLeft", mPrinter.pageLayout().margins().left());
	settings.setValue("pageMarginTop", mPrinter.pageLayout().margins().top());
	settings.setValue("pageMarginRight", mPrinter.pageLayout().margins().right());
	settings.setValue("pageMarginBottom", mPrinter.pageLayout().margins().bottom());
	settings.setValue("pageMarginUnits", (int)mPrinter.pageLayout().units());
	settings.setValue("pageOrientation", (int)mPrinter.pageLayout().orientation());
	settings.setValue("resolution", mPrinter.resolution());
	settings.endGroup();
}

//==================================================================================================

bool MainWindow::newDiagram()
{
	bool diagramCreated = false;

	if (closeDiagram())
	{
		diagramCreated = true;
		mNewDiagramCount++;

		mFilePath = "Untitled " + QString::number(mNewDiagramCount);
		mDiagramWidget->setProperties(mDiagramDefaultProperties);

		showDiagram();
	}

	return diagramCreated;
}

bool MainWindow::openDiagram()
{
	bool drawingOpened = false;

	QString filePath = mWorkingDir.path();
	QFileDialog::Options options = (mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite;

	filePath = QFileDialog::getOpenFileName(this, "Open File", filePath, mFileFilter, nullptr, options);
	if (!filePath.isEmpty())
	{
		QFileInfo fileInfo(filePath);
		mWorkingDir = fileInfo.dir();

		if (closeDiagram())
		{
			drawingOpened = loadDiagramFromFile(filePath);

			if (!drawingOpened)
			{
				QMessageBox::critical(this, "Error Reading File",
					"File could not be read. Please ensure that this file is a valid Jade drawing: " + filePath);

				hideDiagram();
			}
			else showDiagram();
		}
	}

	return drawingOpened;
}

bool MainWindow::saveDiagram()
{
	bool diagramSaved = false;

	if (isDiagramVisible())
	{
		if (!mFilePath.startsWith("Untitled"))
		{
			diagramSaved = saveDiagramToFile(mFilePath);
			if (!diagramSaved)
			{
				QMessageBox::critical(this, "Error Saving File",
					"Unable to open file for saving.  File not saved: " + mFilePath);
			}
			else setWindowTitle(mFilePath);
		}

		else diagramSaved = saveDiagramAs();
	}

	return diagramSaved;
}

bool MainWindow::saveDiagramAs()
{
	bool diagramSaved = false;

	if (isDiagramVisible())
	{
		QString filePath = (mFilePath.startsWith("Untitled")) ? mWorkingDir.path() : mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite;

		filePath = QFileDialog::getSaveFileName(this, "Save File", filePath, mFileFilter, nullptr, options);
		if (!filePath.isEmpty())
		{
			QFileInfo fileInfo(filePath);
			mWorkingDir = fileInfo.dir();

			if (!filePath.endsWith("." + mFileSuffix, Qt::CaseInsensitive))
				filePath += "." + mFileSuffix;

			diagramSaved = saveDiagramToFile(filePath);
			if (!diagramSaved)
			{
				QMessageBox::critical(this, "Error Saving File",
					"Unable to open file for saving.  File not saved: " + mFilePath);
			}
			else setWindowTitle(mFilePath);
		}
	}

	return diagramSaved;
}

bool MainWindow::closeDiagram()
{
	bool diagramClosed = true;

	if (isDiagramVisible())
	{
		QMessageBox::StandardButton button = QMessageBox::Yes;

		if (mPromptCloseUnsaved && !mDiagramWidget->isClean())
		{
			QFileInfo fileInfo(mFilePath);

			button = QMessageBox::question(this, "Save Changes",
				"Save changes to " + fileInfo.fileName() + " before closing?",
				QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);

			if (button == QMessageBox::Yes)
			{
				if (mFilePath.startsWith("Untitled"))
				{
					if (!saveDiagramAs()) button = QMessageBox::Cancel;
				}
				else saveDiagram();
			}
		}

		diagramClosed = (button != QMessageBox::Cancel);
		if (diagramClosed) hideDiagram();
	}

	return diagramClosed;
}

//==================================================================================================

void MainWindow::exportPng()
{
	if (isDiagramVisible())
	{
		QString filePath = mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = mWorkingDir.path();
		else filePath = filePath.left(filePath.length() - mFileSuffix.length() - 1) + ".png";

		filePath = QFileDialog::getSaveFileName(this, "Export PNG", filePath, "Portable Network Graphics (*.png);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			ExportOptionsDialog exportDialog(mDiagramWidget->scene()->sceneRect(), QSize(), mPrevMaintainAspectRatio, 0.2, this);

			if (exportDialog.exec() == QDialog::Accepted)
			{
				if (!filePath.endsWith(".png", Qt::CaseInsensitive)) filePath += ".png";

				QImage pngImage(exportDialog.exportSize(), QImage::Format_ARGB32);
				QPainter painter;
				QRectF visibleRect = mDiagramWidget->scene()->sceneRect();

				mDiagramWidget->clearSelection();

				painter.begin(&pngImage);
				painter.scale(pngImage.width() / visibleRect.width(), pngImage.height() / visibleRect.height());
				painter.translate(-visibleRect.left(), -visibleRect.top());
				painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
				mDiagramWidget->renderExport(&painter);
				painter.end();

				pngImage.save(filePath, "PNG");

				mPrevExportSize = exportDialog.exportSize();
				mPrevMaintainAspectRatio = exportDialog.maintainAspectRatio();
			}
		}
	}
}

void MainWindow::exportSvg()
{
	if (isDiagramVisible())
	{
		QString filePath = mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = mWorkingDir.path();
		else filePath = filePath.left(filePath.length() - mFileSuffix.length() - 1) + ".svg";

		filePath = QFileDialog::getSaveFileName(this, "Export SVG", filePath, "Scalable Vector Graphics (*.svg);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			ExportOptionsDialog exportDialog(mDiagramWidget->scene()->sceneRect(), QSize(), mPrevMaintainAspectRatio, 0.1, this);

			if (exportDialog.exec() == QDialog::Accepted)
			{
				if (!filePath.endsWith(".svg", Qt::CaseInsensitive)) filePath += ".svg";

				QSvgGenerator svgImage;
				QPainter painter;
				QRectF visibleRect = mDiagramWidget->scene()->sceneRect();

				mDiagramWidget->selectNone();

				svgImage.setFileName(filePath);
				svgImage.setSize(exportDialog.exportSize());
				svgImage.setViewBox(QRect(QPoint(0, 0), exportDialog.exportSize()));

				painter.begin(&svgImage);
				painter.scale(svgImage.size().width() / visibleRect.width(), svgImage.size().height() / visibleRect.height());
				painter.translate(-visibleRect.left(), -visibleRect.top());
				painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
				mDiagramWidget->renderExport(&painter);
				painter.end();

				mPrevExportSize = exportDialog.exportSize();
				mPrevMaintainAspectRatio = exportDialog.maintainAspectRatio();
			}
		}
	}
}

void MainWindow::exportOdg()
{
	if (isDiagramVisible())
	{
		QString filePath = mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = mWorkingDir.path();
		else filePath = filePath.left(filePath.length() - mFileSuffix.length() - 1) + ".odg";

		filePath = QFileDialog::getSaveFileName(this, "Export to ODG", filePath, "Open Document Graphics (*.odg);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			if (!filePath.endsWith(".odg", Qt::CaseInsensitive)) filePath += ".odg";

			mDiagramWidget->selectNone();

			OdgWriter writer;
			if (!writer.write(mDiagramWidget, &mPrinter, filePath))
				QMessageBox::critical(this, "ODG Export Error", writer.errorMessage());
		}
	}
}

//==================================================================================================

void MainWindow::printPreview()
{
	if (isDiagramVisible())
	{
		QPrintPreviewDialog printPreviewDialog(&mPrinter, this);
		connect(&printPreviewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPages(QPrinter*)));

		mDiagramWidget->clearSelection();

		printPreviewDialog.exec();
	}
}

void MainWindow::printSetup()
{
	if (isDiagramVisible())
	{
		QPageSetupDialog printSetupDialog(&mPrinter, this);
		printSetupDialog.exec();
	}
}

void MainWindow::printDiagram()
{
	if (isDiagramVisible())
	{
		QPrintDialog printDialog(&mPrinter, this);
		printDialog.setEnabledOptions(QAbstractPrintDialog::PrintShowPageSize);

		mDiagramWidget->clearSelection();

		if (printDialog.exec() == QDialog::Accepted)
		{
			for(int i = 0; i < mPrinter.numCopies(); i++) printPages(&mPrinter);
		}
	}
}

void MainWindow::printPdf()
{
	if (isDiagramVisible())
	{
		QString filePath = mFilePath;
		QFileDialog::Options options = (mPromptOverwrite) ? (QFileDialog::Options)0 : QFileDialog::DontConfirmOverwrite;

		if (filePath.startsWith("Untitled")) filePath = mWorkingDir.path();
		else filePath = filePath.left(filePath.length() - mFileSuffix.length() - 1) + ".pdf";

		filePath = QFileDialog::getSaveFileName(this, "Print to PDF", filePath, "Portable Document Format (*.pdf);;All Files (*)", nullptr, options);
		if (!filePath.isEmpty())
		{
			if (!filePath.endsWith(".pdf", Qt::CaseInsensitive)) filePath += ".pdf";

			mDiagramWidget->clearSelection();

			mPrinter.setOutputFileName(filePath);
			printPages(&mPrinter);
			mPrinter.setOutputFileName("");
		}
	}
}

//==================================================================================================

void MainWindow::preferences()
{
	PreferencesDialog dialog(this);
	dialog.setPrompts(mPromptCloseUnsaved, mPromptOverwrite);
	dialog.setDiagramProperties(mDiagramDefaultProperties);

	if (dialog.exec() == QDialog::Accepted)
	{
		mPromptCloseUnsaved = dialog.shouldPromptOnClosingUnsaved();
		mPromptOverwrite = dialog.shouldPromptOnOverwrite();
		mDiagramDefaultProperties = dialog.diagramProperties();
	}
}

void MainWindow::about()
{
	AboutDialog dialog(this);
	dialog.exec();
}

//==================================================================================================

void MainWindow::showDiagram()
{
	setDiagramVisible(true);

	mDiagramWidget->setDefaultMode();
	mPropertiesWidget->setDiagramProperties(mDiagramWidget->properties());

	mDiagramWidget->zoomFit();

	setModifiedText(mDiagramWidget->isClean());
	setNumberOfItemsText(mDiagramWidget->scene()->items().size());
	mMouseInfoLabel->setText("");

	mPrevExportSize = QSize();
}

void MainWindow::hideDiagram()
{
	setDiagramVisible(false);

	mFilePath = "";
	clearDiagram();

	mModifiedLabel->setText("");
	mNumberOfItemsLabel->setText("");
	mMouseInfoLabel->setText("");
}

//==================================================================================================

void MainWindow::setDiagramVisible(bool visible)
{
	QList<QAction*> actions = MainWindow::actions();
	QList<QAction*> diagramActions = mDiagramWidget->actions();
	QList<QAction*> modeActions = mModeActionGroup->actions();

	// Show/hide diagram
	mStackedWidget->setCurrentIndex(visible ? 1 : 0);

	// Enable/disable actions
	while (!diagramActions.isEmpty())
		diagramActions.takeFirst()->setEnabled(visible);
	while (!modeActions.isEmpty())
		modeActions.takeFirst()->setEnabled(visible);

	actions[ExportPngAction]->setEnabled(visible);
	actions[ExportSvgAction]->setEnabled(visible);
	actions[ExportOdgAction]->setEnabled(visible);
	actions[PrintPreviewAction]->setEnabled(visible);
	actions[PrintSetupAction]->setEnabled(visible);
	actions[PrintAction]->setEnabled(visible);
	actions[PrintPdfAction]->setEnabled(visible);

	// Update window title
	setWindowTitle(visible ? mFilePath : "");
}

void MainWindow::setWindowTitle(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	QString fileName = fileInfo.fileName();

	QMainWindow::setWindowTitle(fileName.isEmpty() ? "Jade" : fileName + " - " + "Jade");
}

//==================================================================================================

void MainWindow::setModeFromAction(QAction* action)
{
	if (action->text() == "Scroll Mode") mDiagramWidget->setScrollMode();
	else if (action->text() == "Zoom Mode") mDiagramWidget->setZoomMode();
	else
	{
		QList<DrawingItem*> newItems;

		if (action->text() == "Place Arc") newItems.append(new DrawingArcItem());
		else if (action->text() == "Place Curve") newItems.append(new DrawingCurveItem());
		else if (action->text() == "Place Ellipse") newItems.append(new DrawingEllipseItem());
		else if (action->text() == "Place Line") newItems.append(new DrawingLineItem());
		else if (action->text() == "Place Polygon") newItems.append(new DrawingPolygonItem());
		else if (action->text() == "Place Polyline") newItems.append(new DrawingPolylineItem());
		else if (action->text() == "Place Rect") newItems.append(new DrawingRectItem());
		else if (action->text() == "Place Text") newItems.append(new DrawingTextItem());
		else if (action->text() == "Place Text Rect") newItems.append(new DrawingTextRectItem());
		else if (action->text() == "Place Text Ellipse") newItems.append(new DrawingTextEllipseItem());
		else if (action->text() == "Place Text Polygon") newItems.append(new DrawingTextPolygonItem());
		else
		{
			DrawingPathItem* matchItem = nullptr;
			QString text;

			for(auto itemIter = mPathItems.begin(); matchItem == nullptr && itemIter != mPathItems.end(); itemIter++)
			{
				text = action->text();
				if (text.right(text.size() - 6) == (*itemIter)->name()) matchItem = *itemIter;
			}

			if (matchItem)
			{
				DrawingPathItem* newItem = new DrawingPathItem();
				newItem->setName(matchItem->name());
				newItem->setPath(matchItem->path(), matchItem->pathRect());
				newItem->setRect(matchItem->rect());
				newItem->addConnectionPoints(matchItem->connectionPoints());
				newItems.append(newItem);
			}
		}

		if (!newItems.isEmpty()) mDiagramWidget->setPlaceMode(newItems);
		else mDiagramWidget->setDefaultMode();
	}
}

void MainWindow::setModeFromDiagram(DrawingView::Mode mode)
{
	switch (mode)
	{
	case DrawingView::ScrollMode:
		mModeActionGroup->actions()[ScrollModeAction]->setChecked(true);
		break;
	case DrawingView::ZoomMode:
		mModeActionGroup->actions()[ZoomModeAction]->setChecked(true);
		break;
	case DrawingView::DefaultMode:
		mModeActionGroup->actions()[DefaultModeAction]->setChecked(true);
		break;
	default:
		break;
	}
}

//==================================================================================================

void MainWindow::setModeText(DrawingView::Mode mode)
{
	QString modeText = "Select Mode";

	switch (mode)
	{
	case DrawingView::ScrollMode: modeText = "Scroll Mode"; break;
	case DrawingView::ZoomMode: modeText = "Zoom Mode"; break;
	case DrawingView::PlaceMode: modeText = "Place Mode"; break;
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

void MainWindow::printPages(QPrinter* printer)
{
	QPainter painter;
	QRectF visibleRect = mDiagramWidget->scene()->sceneRect();
	qreal pageAspect, scale;

	pageAspect = printer->pageRect().width() / (qreal)printer->pageRect().height();
	scale = qMin(printer->pageRect().width() / visibleRect.width(),
		printer->pageRect().height() / visibleRect.height());

	if (visibleRect.height() * pageAspect > visibleRect.width())
	{
		visibleRect.adjust(-(visibleRect.height() * pageAspect - visibleRect.width()) / 2, 0,
			(visibleRect.height() * pageAspect - visibleRect.width()) / 2, 0);
	}
	else if (visibleRect.width() / pageAspect > visibleRect.height())
	{
		visibleRect.adjust(0, -(visibleRect.width() / pageAspect - visibleRect.height()) / 2,
			0, (visibleRect.width() / pageAspect - visibleRect.height()) / 2);
	}

	painter.begin(printer);
	painter.scale(scale, scale);
	painter.translate(-visibleRect.left(), -visibleRect.top());
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
	mDiagramWidget->renderExport(&painter);
	painter.end();
}

//==================================================================================================

void MainWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);

	if (!event->spontaneous())
	{
		if (mDiagramWidget) mDiagramWidget->zoomFit();
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
	if (closeDiagram())
	{
		saveSettings();
		event->accept();
	}
	else event->ignore();
}

//==================================================================================================

bool MainWindow::isDiagramVisible() const
{
	return (mStackedWidget->currentIndex() == 1);
}

bool MainWindow::saveDiagramToFile(const QString& filePath)
{
	QFile dataFile(filePath);

	bool fileError = !dataFile.open(QIODevice::WriteOnly);
	if (!fileError)
	{
		DiagramWriter writer(&dataFile);
		writer.write(mDiagramWidget);
		dataFile.close();

		mDiagramWidget->setClean();
		mDiagramWidget->viewport()->update();

		mFilePath = filePath;
	}

	return (!fileError);
}

bool MainWindow::loadDiagramFromFile(const QString& filePath)
{
	QFile dataFile(filePath);

	bool fileError = !dataFile.open(QIODevice::ReadOnly);
	if (!fileError)
	{
		clearDiagram();

		DiagramReader reader(&dataFile);
		reader.read(mDiagramWidget);
		dataFile.close();

		mDiagramWidget->setClean();
		mDiagramWidget->viewport()->update();

		mFilePath = filePath;
	}

	return (!fileError);
}

void MainWindow::clearDiagram()
{
	mDiagramWidget->setDefaultMode();
	mDiagramWidget->scene()->clearItems();
}

//==================================================================================================

void MainWindow::createPropertiesDock()
{
	mPropertiesWidget = new DynamicPropertiesWidget();
	mPropertiesDock = new QDockWidget("Properties");
	mPropertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mPropertiesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	mPropertiesDock->setWidget(mPropertiesWidget);
	mPropertiesDock->setObjectName("PropertiesDock");
	addDockWidget(Qt::RightDockWidgetArea, mPropertiesDock);

	connect(mDiagramWidget, SIGNAL(selectionChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setSelectedItems(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(newItemsChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setNewItems(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(itemsPositionChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setItemGeometry(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(itemsTransformChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setItemGeometry(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(itemsGeometryChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setItemGeometry(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(itemsVisibilityChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setItemGeometry(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(itemsStyleChanged(const QList<DrawingItem*>&)), mPropertiesWidget, SLOT(setItemsStyleProperties(const QList<DrawingItem*>&)));
	connect(mDiagramWidget, SIGNAL(itemCornerRadiusChanged(DrawingItem*)), mPropertiesWidget, SLOT(setItemCornerRadius(DrawingItem*)));
	connect(mDiagramWidget, SIGNAL(itemCaptionChanged(DrawingItem*)), mPropertiesWidget, SLOT(setItemCaption(DrawingItem*)));
	connect(mDiagramWidget, SIGNAL(diagramPropertiesChanged(const QHash<DiagramWidget::Property,QVariant>&)), mPropertiesWidget, SLOT(setDiagramProperties(const QHash<DiagramWidget::Property,QVariant>&)));

	connect(mPropertiesWidget, SIGNAL(selectedItemMoved(const QPointF&)), mDiagramWidget, SLOT(moveSelection(const QPointF&)));
	connect(mPropertiesWidget, SIGNAL(selectedItemResized(DrawingItemPoint*, const QPointF&)), mDiagramWidget, SLOT(resizeSelection(DrawingItemPoint*, const QPointF&)));
	connect(mPropertiesWidget, SIGNAL(selectedItemsStyleChanged(const QHash<DrawingItemStyle::Property,QVariant>&)),
		mDiagramWidget, SLOT(setSelectionStyleProperties(const QHash<DrawingItemStyle::Property,QVariant>&)));
	connect(mPropertiesWidget, SIGNAL(selectedItemCornerRadiusChanged(qreal,qreal)), mDiagramWidget, SLOT(setSelectionCornerRadius(qreal,qreal)));
	connect(mPropertiesWidget, SIGNAL(selectedItemCaptionChanged(const QString&)), mDiagramWidget, SLOT(setSelectionCaption(const QString&)));
	connect(mPropertiesWidget, SIGNAL(diagramPropertiesChanged(const QHash<DiagramWidget::Property,QVariant>&)),
		mDiagramWidget, SLOT(setDiagramProperties(const QHash<DiagramWidget::Property,QVariant>&)));

	connect(mDiagramWidget, SIGNAL(propertiesTriggered()), mPropertiesDock, SLOT(show()));
}

void MainWindow::createStatusBar()
{
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

	connect(mDiagramWidget, SIGNAL(modeChanged(DrawingView::Mode)), this, SLOT(setModeText(DrawingView::Mode)));
	connect(mDiagramWidget, SIGNAL(cleanChanged(bool)), this, SLOT(setModifiedText(bool)));
	connect(mDiagramWidget, SIGNAL(numberOfItemsChanged(int)), this, SLOT(setNumberOfItemsText(int)));
	connect(mDiagramWidget, SIGNAL(mouseInfoChanged(const QString&)), mMouseInfoLabel, SLOT(setText(const QString&)));
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
	connect(mDiagramWidget, SIGNAL(modeChanged(DrawingView::Mode)), this, SLOT(setModeFromDiagram(DrawingView::Mode)));

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

	mElectricItemsAction = addPathItems("Electric Items", ElectricItems::items(), ElectricItems::icons());
	mLogicItemsAction = addPathItems("Logic Items", LogicItems::items(), LogicItems::icons());

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
	menu->addSeparator();
	menu->addAction(mElectricItemsAction);
	menu->addAction(mLogicItemsAction);

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
	mZoomCombo = new QComboBox();
	mZoomCombo->setMinimumWidth(QFontMetrics(mZoomCombo->font()).width("XXXXXX.XX%") + 48);
	mZoomCombo->addItems(QStringList() << "Fit to Page" << "25%" << "50%" << "100%" << "150%" << "200%" << "300%" << "400%");
	mZoomCombo->setEditable(true);
	mZoomCombo->setCurrentIndex(3);

	connect(mDiagramWidget, SIGNAL(scaleChanged(qreal)), this, SLOT(setZoomComboText(qreal)));
	connect(mZoomCombo, SIGNAL(activated(const QString&)), this, SLOT(setZoomLevel(const QString&)));

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

QAction* MainWindow::addPathItems(const QString& name, const QList<DrawingPathItem*>& items, const QStringList& icons)
{
	QAction* action = new QAction(QIcon(icons.first()), name, this);
	QMenu* menu = new QMenu(name);

	mPathItems.append(items);
	for(int i = 0; i < items.size() && i < icons.size(); i++)
		menu->addAction(addModeAction("Place " + items[i]->name(), icons[i], ""));

	action->setMenu(menu);
	return action;
}
