// DrawingWindow.cpp
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

#include "DrawingWindow.h"
#include "DrawingItem.h"
#include "DrawingPropertiesBrowser.h"
#include "DrawingWidget.h"
#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QShowEvent>
#include <QStackedWidget>

DrawingWindow::DrawingWindow(QWidget* parent, Qt::WindowFlags f) : QMainWindow(parent, f)
{
	mDrawingWidget = nullptr;
	mPropertiesBrowser = nullptr;

	mApplicationName = "libdrawing";
	mFileFilter = "XML Files (*.xml);;All Files (*)";
	mFileSuffix = "xml";
	mPromptOverwrite = true;
	mPromptCloseUnsaved = true;

	mNewCount = 0;
	mPropertiesDockVisibleOnClose = true;
#ifndef WIN32
	mWorkingDir = QDir::home();
#endif

	mPropertiesDock = new QDockWidget("Properties");
	mPropertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mPropertiesDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
	mPropertiesDock->setObjectName("PropertiesDock");
	addDockWidget(Qt::RightDockWidgetArea, mPropertiesDock);
	setPropertiesBrowser(new DrawingPropertiesBrowser());

	mStackedWidget = new QStackedWidget();
	mStackedWidget->addWidget(new QWidget());
	mStackedWidget->setCurrentIndex(0);
	setCentralWidget(mStackedWidget);
	setDrawing(new DrawingWidget());

	addActions();
	updateWindowTitle(mFilePath);

	connect(this, SIGNAL(drawingVisibilityChanged(bool)), this, SLOT(updateWindow(bool)));
	connect(this, SIGNAL(filePathChanged(const QString&)), this, SLOT(updateWindowTitle(const QString&)));
}

DrawingWindow::~DrawingWindow() { }

//==================================================================================================

void DrawingWindow::setDrawing(DrawingWidget* drawing)
{
	if (drawing)
	{
		if (mDrawingWidget)
		{
			mStackedWidget->removeWidget(mDrawingWidget);
			mDrawingWidget->disconnect();
			delete mDrawingWidget;
		}

		mDrawingWidget = drawing;
		mDrawingDefaultProperties = mDrawingWidget->properties();
		mStackedWidget->addWidget(mDrawingWidget);
		mStackedWidget->setCurrentIndex(0);

		if (mPropertiesBrowser) mPropertiesBrowser->setDrawingProperties(mDrawingWidget->properties());

		connect(mDrawingWidget, SIGNAL(propertiesTriggered()), mPropertiesDock, SLOT(show()));
		connectDrawingAndPropertiesBrowser();
	}
}

void DrawingWindow::setPropertiesBrowser(DrawingPropertiesBrowser* propertiesBrowser)
{
	if (propertiesBrowser)
	{
		mPropertiesDock->setWidget(propertiesBrowser);

		if (mPropertiesBrowser)
		{
			mPropertiesBrowser->disconnect();
			delete mPropertiesBrowser;
		}

		mPropertiesBrowser = propertiesBrowser;
		if (mDrawingWidget) mPropertiesBrowser->setDrawingProperties(mDrawingWidget->properties());
		mPropertiesBrowser->setDefaultItemProperties(DrawingItem::factory.defaultItemProperties());
		mPropertiesBrowser->setItems(QList<DrawingItem*>());

		connect(mPropertiesBrowser, SIGNAL(defaultItemPropertiesChanged(const QHash<QString,QVariant>&)),
			&DrawingItem::factory, SLOT(setDefaultItemProperties(const QHash<QString,QVariant>&)));
		connectDrawingAndPropertiesBrowser();
	}
}

DrawingWidget* DrawingWindow::drawing() const
{
	return mDrawingWidget;
}

DrawingPropertiesBrowser* DrawingWindow::propertiesBrowser() const
{
	return mPropertiesBrowser;
}

bool DrawingWindow::isDrawingWidgetVisible() const
{
	return (mDrawingWidget && mStackedWidget->currentIndex() == 1);
}

//==================================================================================================

void DrawingWindow::setDefaultDrawingProperties(const QHash<QString,QVariant>& properties)
{
	mDrawingDefaultProperties = properties;
}

QHash<QString,QVariant> DrawingWindow::defaultDrawingProperties() const
{
	return mDrawingDefaultProperties;
}

//==================================================================================================

void DrawingWindow::setApplicationName(const QString& name)
{
	mApplicationName = name;
	setWindowTitle(mFilePath);
}

void DrawingWindow::setFileDialogOptions(const QString& fileFilter, const QString& fileSuffix)
{
	mFileFilter = fileFilter;
	mFileSuffix = fileSuffix;
}

void DrawingWindow::setPromptOnOverwrite(bool prompt)
{
	mPromptOverwrite = prompt;
}

void DrawingWindow::setPromptOnClosingUnsaved(bool prompt)
{
	mPromptCloseUnsaved = prompt;
}

QString DrawingWindow::applicationName() const
{
	return mApplicationName;
}

QString DrawingWindow::fileFilter() const
{
	return mFileFilter;
}

QString DrawingWindow::fileSuffix() const
{
	return mFileSuffix;
}

bool DrawingWindow::shouldPromptOnOverwrite() const
{
	return mPromptOverwrite;
}

bool DrawingWindow::shouldPromptOnClosingUnsaved() const
{
	return mPromptCloseUnsaved;
}

QString DrawingWindow::filePath() const
{
	return mFilePath;
}

QDir DrawingWindow::workingDir() const
{
	return mWorkingDir;
}

//==================================================================================================

QAction* DrawingWindow::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QMainWindow::addAction(action);

	return action;
}

//==================================================================================================

void DrawingWindow::newDrawing()
{
	// Close any open drawing first
	closeDrawing();

	// Create a new drawing only if there is no open drawing (i.e. close was successful or unneeded)
	if (mDrawingWidget && !isDrawingWidgetVisible())
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

void DrawingWindow::openDrawing(const QString& filePath)
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
		if (mDrawingWidget && !isDrawingWidgetVisible())
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

void DrawingWindow::saveDrawing(const QString& filePath)
{
	//emit filePathChanged(mFilePath);

	if (isDrawingWidgetVisible())
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

void DrawingWindow::saveDrawingAs()
{
	if (isDrawingWidgetVisible())
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

void DrawingWindow::closeDrawing()
{
	if (isDrawingWidgetVisible())
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

bool DrawingWindow::saveDrawingToFile(const QString& filePath)
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

bool DrawingWindow::loadDrawingFromFile(const QString& filePath)
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

void DrawingWindow::clearDrawing()
{
	if (mDrawingWidget)
	{
		mDrawingWidget->clearItems();
		mDrawingWidget->setProperties(mDrawingDefaultProperties);
		mDrawingWidget->setClean();
	}
}

//==================================================================================================

void DrawingWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);

	if (!event->spontaneous())
	{
		if (mDrawingWidget) mDrawingWidget->zoomFit();
	}
	else if (!mWindowState.isEmpty()) restoreState(mWindowState);
}

void DrawingWindow::hideEvent(QHideEvent* event)
{
	QMainWindow::hideEvent(event);

	if (event->spontaneous()) mWindowState = saveState();
}

void DrawingWindow::closeEvent(QCloseEvent* event)
{
	closeDrawing();
	if (!isDrawingWidgetVisible()) event->accept();
	else event->ignore();
}

//==================================================================================================

void DrawingWindow::updateWindow(bool drawingVisible)
{
	if (mDrawingWidget)
	{
		QList<QAction*> actionList = actions();
		if (actionList.size() >= NumberOfActions)
		{
			actionList[SaveAction]->setEnabled(drawingVisible);
			actionList[SaveAsAction]->setEnabled(drawingVisible);
			actionList[CloseAction]->setEnabled(drawingVisible);
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

void DrawingWindow::updateWindowTitle(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	QString fileName = fileInfo.fileName();

	QMainWindow::setWindowTitle(fileName.isEmpty() ? mApplicationName : fileName + " - " + mApplicationName);
}

//==================================================================================================

void DrawingWindow::connectDrawingAndPropertiesBrowser()
{
	if (mDrawingWidget && mPropertiesBrowser)
	{
		connect(mDrawingWidget, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
			mPropertiesBrowser, SLOT(setDrawingProperties(const QHash<QString,QVariant>&)));
		connect(mDrawingWidget, SIGNAL(itemsPropertiesChanged(const QList<DrawingItem*>&)),
			mPropertiesBrowser, SLOT(setItemsProperties(const QList<DrawingItem*>&)));
		connect(mDrawingWidget, SIGNAL(currentItemsChanged(const QList<DrawingItem*>&)),
			mPropertiesBrowser, SLOT(setItems(const QList<DrawingItem*>&)));

		connect(mPropertiesBrowser, SIGNAL(drawingPropertiesChanged(const QHash<QString,QVariant>&)),
			mDrawingWidget, SLOT(updateProperties(const QHash<QString,QVariant>&)));
		connect(mPropertiesBrowser, SIGNAL(itemsPropertiesChanged(const QHash< DrawingItem*, QHash<QString,QVariant> >&)),
			mDrawingWidget, SLOT(updateItemsProperties(const QHash< DrawingItem*, QHash<QString,QVariant> >&)));
	}
}

//==================================================================================================

void DrawingWindow::addActions()
{
	addAction("New...", this, SLOT(newDrawing()), "", "Ctrl+N");
	addAction("Open...", this, SLOT(openDrawing()), "", "Ctrl+O");
	addAction("Save", this, SLOT(saveDrawing()), "", "Ctrl+S");
	addAction("Save As...", this, SLOT(saveDrawingAs()), "", "Ctrl+Shift+S");
	addAction("Close", this, SLOT(closeDrawing()), "", "Ctrl+W");
	addAction("Exit", this, SLOT(close()));
}
