// File: JadeWindow.h
// Copyright (C) 2023  Jason Allen
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

#ifndef JADEWINDOW_H
#define JADEWINDOW_H

#include <QMainWindow>

class QComboBox;
class QLabel;
class DrawingWidget;
class PagesWidget;
class PropertiesWidget;
class StylesWidget;

class JadeWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ActionIndex { NewAction, OpenAction, SaveAction, SaveAsAction, CloseAction, ExportPngAction, ExportSvgAction,
                       PreferencesAction, ExitAction, ViewPropertiesAction, ViewPagesAction,
                       AboutAction, AboutQtAction };

private:
    DrawingWidget* mDrawingWidget;

    PagesWidget* mPagesWidget;
    QDockWidget* mPagesDock;

    PropertiesWidget* mPropertiesWidget;
    QDockWidget* mPropertiesDock;

    QLabel* mModeLabel;
    QLabel* mModifiedLabel;
    QLabel* mMouseInfoLabel;

    QComboBox* mZoomCombo;

    QString mFilePath;
    int mNewDrawingCount;
    QString mWorkingDir;
    bool mPromptOverwrite;
    bool mPromptCloseUnsaved;
    bool mPagesDockVisibleOnClose;
    bool mPropertiesDockVisibleOnClose;

public:
    JadeWindow();

private:
    QDockWidget* addDockWidget(const QString& title, QWidget* widget, Qt::DockWidgetArea area);
    QLabel* addStatusBarLabel(const QString& text, const QString& minimumWidthText,
                              const QObject* signalObject, const char* signalFunction);

    void createActions();
    void createMenus();
    void createToolBars();
    QAction* addAction(const QString& text, const QObject* slotObject, const char* slotFunction,
                       const QString& iconPath = QString(), const QString& keySequence = QString());

public slots:
    void newDrawing();
    void openDrawing(const QString& path = QString());
    void saveDrawing(const QString& path = QString());
    void saveDrawingAs();
    void closeDrawing();

    void exportPng();
    void exportSvg();

    void preferences();
    void about();

private:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void setDrawingVisible(bool visible);
    void setFilePath(const QString& path);

    void setZoomLevel(const QString& zoomLevelText);
    void setZoomComboText(double scale);

    void saveSettings();
    void loadSettings();
};

#endif
