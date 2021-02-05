// MainWindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QList>
#include <QMainWindow>
#include <QString>
#include <QVariant>

class DrawingBrowser;
class DrawingItem;
class DrawingPathItem;
class DrawingWidget;
class PropertiesBrowser;
class QComboBox;
class QLabel;
class QStackedWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	enum ActionIndex { NewAction, OpenAction, SaveAction, SaveAsAction, CloseAction,
		ExportPngAction, ExportSvgAction, PreferencesAction, ExitAction,
		AboutAction, AboutQtAction,	NumberOfActions };
	enum PlaceActionIndex { PlaceLineAction, PlaceCurveAction, PlacePolylineAction,
		PlaceRectAction, PlaceEllipseAction, PlacePolygonAction,
		PlaceTextAction, PlaceTextRectAction, PlaceTextEllipseAction };

private:
	DrawingWidget* mDrawingWidget;
	QStackedWidget* mStackedWidget;

	DrawingBrowser* mDrawingBrowser;
	QDockWidget* mDrawingDock;

	PropertiesBrowser* mPropertiesBrowser;
	QDockWidget* mPropertiesDock;

	QComboBox* mZoomCombo;

	QLabel* mModeLabel;
	QLabel* mModifiedLabel;
	QLabel* mMouseInfoLabel;

	QHash<QString,QVariant> mDrawingDefaultProperties;

	QList<QAction*> mPlaceActions;
	QList<DrawingPathItem*> mPathItems;

public:
	MainWindow(const QString& filePath = QString());
	~MainWindow();

private slots:
	void setModeFromPlaceAction(QAction* action);

private:
	void addActions();
	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	QAction* addPlaceAction(const QString& text, const QString& iconPath = QString(),
		DrawingItem* item = nullptr);

	void createMenus();
	void createToolBars();
	void createStatusBar();
};

#endif
