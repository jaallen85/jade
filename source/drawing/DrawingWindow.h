// DrawingWindow.h
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

#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <QDir>
#include <QMainWindow>

class DrawingPropertiesBrowser;
class DrawingWidget;
class QStackedWidget;

class DrawingWindow : public QMainWindow
{
	Q_OBJECT

public:
	enum ActionIndex { NewAction, OpenAction, SaveAction, SaveAsAction, CloseAction,
		ExitAction, NumberOfActions  };

private:
	QStackedWidget* mStackedWidget;
	DrawingWidget* mDrawingWidget;
	QHash<QString,QVariant> mDrawingDefaultProperties;

	QDockWidget* mPropertiesDock;
	DrawingPropertiesBrowser* mPropertiesBrowser;

	QString mApplicationName;
	QString mFileFilter;
	QString mFileSuffix;
	bool mPromptOverwrite;
	bool mPromptCloseUnsaved;

	QString mFilePath;
	QDir mWorkingDir;
	int mNewCount;
	QByteArray mWindowState;
	bool mPropertiesDockVisibleOnClose;

public:
	DrawingWindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	virtual ~DrawingWindow();

	void setDrawing(DrawingWidget* drawing);
	void setPropertiesBrowser(DrawingPropertiesBrowser* propertiesBrowser);
	DrawingWidget* drawing() const;
	DrawingPropertiesBrowser* propertiesBrowser() const;
	bool isDrawingWidgetVisible() const;

	void setDefaultDrawingProperties(const QHash<QString,QVariant>& properties);
	QHash<QString,QVariant> defaultDrawingProperties() const;

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

	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());

signals:
	void drawingVisibilityChanged(bool visible);
	void filePathChanged(const QString& filePath);

public slots:
	void newDrawing();
	void openDrawing(const QString& filePath = QString());
	void saveDrawing(const QString& filePath = QString());
	void saveDrawingAs();
	void closeDrawing();

protected:
	virtual bool saveDrawingToFile(const QString& filePath);
	virtual bool loadDrawingFromFile(const QString& filePath);
	virtual void clearDrawing();

	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;
	virtual void closeEvent(QCloseEvent* event) override;

private slots:
	void updateWindow(bool drawingVisible);
	void updateWindowTitle(const QString& filePath);

private:
	void connectDrawingAndPropertiesBrowser();

	void addActions();
};

#endif
