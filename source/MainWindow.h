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

#include <DrawingWindow.h>
#include <DrawingTypes.h>

class QComboBox;
class QLabel;

class MainWindow : public DrawingWindow
{
	Q_OBJECT

public:
	enum ActionIndex { ExportPngAction = NumberOfActions, ExportSvgAction,
		PreferencesAction, AboutAction, AboutQtAction  };

private:
	QComboBox* mZoomCombo;

	QLabel* mModeLabel;
	QLabel* mModifiedLabel;
	QLabel* mMouseInfoLabel;

	QSize mExportSize;
	bool mExportMaintainAspectRatio;

public:
	MainWindow(const QString& filePath = QString());
	~MainWindow();

	void saveSettings();
	void loadSettings();

public slots:
	void exportPng();
	void exportSvg();

	void preferences();
	void about();

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private slots:
	void setZoomComboText(qreal scale);
	void setZoomLevel(const QString& text);

	void setModeText(Drawing::Mode mode);
	void setModifiedText(bool clean);
	void setMouseInfoText(const QPointF& position);
	void setMouseInfoText(const QPointF& position1, const QPointF& position2);

	void updateWindow(bool drawingVisible);

	void resetExportSize(bool drawingVisible);
	void resetExportSize(const QHash<QString,QVariant>& properties);

private:
	QString positionToString(const QPointF& position) const;

	void addActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
};

#endif
