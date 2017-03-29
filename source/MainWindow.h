/* MainWindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DiagramWidget.h>
#include <QtPrintSupport>
#include <QtSvg>

class DynamicPropertiesWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	enum ActionIndex { NewAction, OpenAction, SaveAction, SaveAsAction, CloseAction,
		ExportPngAction, ExportSvgAction, ExportOdgAction,
		PrintPreviewAction, PrintSetupAction, PrintAction, PrintPdfAction,
		PreferencesAction, ExitAction,
		AboutAction, AboutQtAction, NumberOfActions };
	enum ModeActionIndex { DefaultModeAction, ScrollModeAction, ZoomModeAction,
		PlaceArcAction, PlaceCurveAction, PlaceEllipseAction, PlaceLineAction,
		PlacePolygonAction, PlacePolylineAction, PlaceRectAction, PlaceTextAction,
		PlaceTextRectAction, PlaceTextEllipseAction, PlaceTextPolygonAction,
		NumberOfModeActions };

private:
	QStackedWidget* mStackedWidget;
	DiagramWidget* mDiagramWidget;
	QHash<DiagramWidget::Property,QVariant> mDiagramDefaultProperties;

	QComboBox* mZoomCombo;

	DynamicPropertiesWidget* mPropertiesWidget;
	QDockWidget* mPropertiesDock;

	QLabel* mModeLabel;
	QLabel* mModifiedLabel;
	QLabel* mNumberOfItemsLabel;
	QLabel* mMouseInfoLabel;

	QActionGroup* mModeActionGroup;
	QList<DrawingPathItem*> mPathItems;
	QAction* mElectricItemsAction;
	QAction* mLogicItemsAction;

	bool mPromptCloseUnsaved;
	bool mPromptOverwrite;

	QString mFilePath;
	QString mFileFilter;
	QString mFileSuffix;
	int mNewDiagramCount;
	QDir mWorkingDir;
	QByteArray mWindowState;

	QSize mPrevExportSize;
	bool mPrevMaintainAspectRatio;

	QPrinter mPrinter;

public:
	MainWindow(const QString& filePath = QString());
	~MainWindow();

	void loadSettings();
	void saveSettings();

public slots:
	bool newDiagram();
	bool openDiagram();
	bool saveDiagram();
	bool saveDiagramAs();
	bool closeDiagram();

	void exportPng();
	void exportSvg();
	void exportOdg();

	void printPreview();
	void printSetup();
	void printDiagram();
	void printPdf();

	void preferences();
	void about();

	void showDiagram();
	void hideDiagram();

private slots:
	void setDiagramVisible(bool visible);
	void setWindowTitle(const QString& filePath);

	void setModeFromAction(QAction* action);
	void setModeFromDiagram(DrawingView::Mode mode);

	void setModeText(DrawingView::Mode mode);
	void setModifiedText(bool clean);
	void setNumberOfItemsText(int itemCount);

	void setZoomComboText(qreal scale);
	void setZoomLevel(const QString& text);

	void printPages(QPrinter* printer);

private:
	void showEvent(QShowEvent* event);
	void hideEvent(QHideEvent* event);
	void closeEvent(QCloseEvent* event);

private:
	bool isDiagramVisible() const;
	bool saveDiagramToFile(const QString& filePath);
	bool loadDiagramFromFile(const QString& filePath);
	void clearDiagram();

	void createPropertiesDock();
	void createStatusBar();

	void createActions();
	void createMenus();
	void createToolBars();
	void addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	QAction* addModeAction(const QString& text,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	QAction* addPathItems(const QString& name, const QList<DrawingPathItem*>& items,
		const QStringList& icons);
};

#endif
