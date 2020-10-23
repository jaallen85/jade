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
#include "DrawingPropertiesBrowser.h"
#include "DrawingWidget.h"
#include <QDockWidget>
#include <QShowEvent>
#include <QStackedWidget>

DrawingWindow::DrawingWindow(QWidget* parent, Qt::WindowFlags f) : QMainWindow(parent, f)
{
	mDrawingWidget = nullptr;
	mPropertiesBrowser = nullptr;

	mPropertiesBrowser = nullptr;
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

	mStackedWidget->setCurrentIndex(1);
}

DrawingWindow::~DrawingWindow()
{

}

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
		mStackedWidget->addWidget(mDrawingWidget);
		mStackedWidget->setCurrentIndex(0);

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
			delete mDrawingWidget;
		}

		mPropertiesBrowser = propertiesBrowser;
		if (mDrawingWidget) mPropertiesBrowser->setDrawingProperties(mDrawingWidget->properties());

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

//==================================================================================================

void DrawingWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);
	if (!event->spontaneous() && mDrawingWidget) mDrawingWidget->zoomFit();
}

//==================================================================================================

void DrawingWindow::connectDrawingAndPropertiesBrowser()
{
	if (mDrawingWidget && mPropertiesBrowser)
	{
		connect(mDrawingWidget, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
			mPropertiesBrowser, SLOT(setDrawingProperties(const QHash<QString,QVariant>&)));

		connect(mPropertiesBrowser, SIGNAL(drawingPropertiesChanged(const QHash<QString,QVariant>&)),
			mDrawingWidget, SLOT(updateProperties(const QHash<QString,QVariant>&)));
	}
}
