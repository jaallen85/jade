/* MainToolBox.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef MAINTOOLBOX_H
#define MAINTOOLBOX_H

#include <DrawingWidget.h>

class MainToolBox : public QFrame
{
	Q_OBJECT

private:
	QMap<QString,QVariant> mDefaultItemProperties;

	QToolButton* mDefaultModeButton;
	QToolButton* mScrollModeButton;
	QToolButton* mZoomModeButton;
	QToolButton* mPropertiesButton;

	QTreeWidget* mPlaceItemsWidget;
	QMap<QTreeWidgetItem*,QAction*> mPlaceActionsMap;
	QMap<QAction*,DrawingItem*> mPlaceItemsMap;

	QActionGroup* mModeActionGroup;

public:
	MainToolBox();
	~MainToolBox();

	QSize sizeHint() const;

	QMap<QString,QVariant> defaultItemProperties() const;

public slots:
	void updateMode(DrawingWidget::Mode mode);
	void updateDefaultItemProperties(const QMap<QString,QVariant>& properties);

signals:
	void defaultModeTriggered();
	void zoomModeTriggered();
	void scrollModeTriggered();
	void placeModeTriggered(DrawingItem* item);
	void propertiesTriggered();

private slots:
	void setModeFromAction(QAction* action);
	void triggerAction(QTreeWidgetItem* item, int column);

private:
	QDialogButtonBox* createButtonBox();
	QTreeWidget* createTreeWidget();

	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	QAction* addModeAction(const QString& text,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	void addItem(DrawingItem* item, const QString& section, const QString& text,
		const QString& iconPath = "");
};

#endif
