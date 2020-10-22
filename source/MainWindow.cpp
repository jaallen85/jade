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
#include <DrawingWidget.h>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QDebug>

MainWindow::MainWindow(const QString& filePath) : DrawingWindow()
{
	addActions();
	createMenus();
	createToolBars();

	setWindowTitle("Jade");
	setWindowIcon(QIcon(":/icons/jade/jade.png"));
	resize(983, 800);
}

MainWindow::~MainWindow()
{

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

void MainWindow::addActions()
{
	QList<QAction*> drawingActionList = drawing()->actions();

	drawingActionList[DrawingWidget::ZoomInAction]->setIcon(QIcon(":/icons/oxygen/zoom-in.png"));
	drawingActionList[DrawingWidget::ZoomOutAction]->setIcon(QIcon(":/icons/oxygen/zoom-out.png"));
	drawingActionList[DrawingWidget::ZoomFitAction]->setIcon(QIcon(":/icons/oxygen/zoom-fit-best.png"));
}

void MainWindow::createMenus()
{
	QList<QAction*> drawingActionList = drawing()->actions();
	QMenu* menu;

	menu = menuBar()->addMenu("View");
	menu->addAction(drawingActionList[DrawingWidget::ZoomInAction]);
	menu->addAction(drawingActionList[DrawingWidget::ZoomOutAction]);
	menu->addAction(drawingActionList[DrawingWidget::ZoomFitAction]);
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

	QList<QAction*> drawingActionList = drawing()->actions();
	QToolBar* toolBar;
	int size = mZoomCombo->sizeHint().height();

	toolBar = new QToolBar("View");
	toolBar->setObjectName("ViewToolBar");
	toolBar->setIconSize(QSize(size, size));
	toolBar->addAction(drawingActionList[DrawingWidget::ZoomInAction]);
	toolBar->addWidget(zoomWidget);
	toolBar->addAction(drawingActionList[DrawingWidget::ZoomOutAction]);
	addToolBar(toolBar);
}
