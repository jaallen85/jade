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

#include <DrawingTypes.h>
#include <QByteArray>
#include <QDir>
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

	QString mApplicationName;
	QString mFileFilter;
	QString mFileSuffix;
	bool mPromptOverwrite;
	bool mPromptCloseUnsaved;
	QString mFilePath;
	QDir mWorkingDir;

	QHash<QString,QVariant> mDefaultDrawingProperties;

	int mNewCount;
	QByteArray mWindowState;
	bool mPropertiesDockVisibleOnClose;

	QSize mExportSize;
	bool mExportMaintainAspectRatio;

	QList<QAction*> mPlaceActions;
	QList<DrawingPathItem*> mPathItems;

public:
	MainWindow(const QString& filePath = QString());
	~MainWindow();

	DrawingWidget* drawing() const;
	bool isDrawingVisible() const;

	void setApplicationName(const QString& name);
	void setFileDialogOptions(const QString& fileFilter, const QString& fileSuffix);
	void setPromptOnOverwrite(bool prompt);
	void setPromptOnClosingUnsaved(bool prompt);
	QString applicationName() const;
	QString fileFilter() const;
	QString fileSuffix() const;
	bool shouldPromptOnOverwrite() const;
	bool shouldPromptOnClosingUnsaved() const;
	QString filePath() const;
	QDir workingDir() const;

	void setDefaultDrawingProperties(const QHash<QString,QVariant>& properties);
	QHash<QString,QVariant> defaultDrawingProperties() const;

	void saveSettings();
	void loadSettings();

public slots:
	void newDrawing();
	void openDrawing(const QString& filePath = QString());
	void saveDrawing(const QString& filePath = QString());
	void saveDrawingAs();
	void closeDrawing();

	void exportPng();
	void exportSvg();

	void preferences();
	void about();

signals:
	void drawingVisibilityChanged(bool visible);
	void filePathChanged(const QString& filePath);

private slots:
	void updateWindow(bool drawingVisible);
	void updateWindowTitle(const QString& filePath);

	void setZoomComboText(qreal scale);
	void setZoomLevel(const QString& text);

	void setModeText(Drawing::Mode mode);
	void setModifiedText(bool clean);
	void setMouseInfoText(const QPointF& position);
	void setMouseInfoText(const QPointF& position1, const QPointF& position2);

	void setModeFromPlaceAction(QAction* action);

	void resetExportSize(bool drawingVisible);
	void resetExportSize(const QHash<QString,QVariant>& properties);

private:
	bool saveDrawingToFile(const QString& filePath);
	bool loadDrawingFromFile(const QString& filePath);
	void clearDrawing();

	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

	QString positionToString(const QPointF& position) const;

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
