// PropertiesBrowser.h
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

#ifndef PROPERTIESBROWSER_H
#define PROPERTIESBROWSER_H

#include <QHash>
#include <QList>
#include <QScrollArea>
#include <QVariant>

class DrawingItem;
class PropertiesWidget;
class QGroupBox;
class QStackedWidget;
class QTabWidget;
class QVBoxLayout;

class PropertiesBrowser : public QScrollArea
{
	Q_OBJECT

private:
	QWidget* mDrawingPropertiesWidget;
	QVBoxLayout* mDrawingPropertiesLayout;
	QList<QGroupBox*> mDrawingPropertiesGroups;
	QList<PropertiesWidget*> mDrawingPropertiesSubWidgets;

	QWidget* mDefaultItemPropertiesWidget;
	QVBoxLayout* mDefaultItemPropertiesLayout;
	QList<QGroupBox*> mDefaultItemPropertiesGroups;
	QList<PropertiesWidget*> mDefaultItemPropertiesSubWidgets;

	QWidget* mItemsPropertiesWidget;
	QVBoxLayout* mItemsPropertiesLayout;
	QList<QGroupBox*> mItemsPropertiesGroups;
	QList<PropertiesWidget*> mItemsPropertiesSubWidgets;

	QTabWidget* mTabWidget;
	QStackedWidget* mStackedWidget;

	int mPreferredWidth;

public:
	PropertiesBrowser(QWidget* parent = nullptr);
	virtual ~PropertiesBrowser();

	void registerDrawingWidget(const QString& groupTitle, PropertiesWidget* widget);
	void registerDefaultItemWidget(const QString& groupTitle, PropertiesWidget* widget);
	void registerItemsWidget(const QString& groupTitle, PropertiesWidget* widget);

	void setPreferredWidth(int width);
	void setLabelWidth(int width);
	QSize sizeHint() const;

public slots:
	void setItems(const QList<DrawingItem*>& items);

	void setDrawingProperties(const QHash<QString,QVariant>& properties);
	void setDefaultItemProperties(const QHash<QString,QVariant>& properties);
	void setItemsProperties(const QList<DrawingItem*>& items);

signals:
	void drawingPropertiesChanged(const QHash<QString,QVariant>& properties);
	void defaultItemPropertiesChanged(const QHash<QString,QVariant>& properties);
	void itemsPropertiesChanged(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties);

private:
	void createPropertiesWidget(QWidget*& widget, QVBoxLayout*& layout);
	QGroupBox* findOrCreateGroup(const QString& groupTitle, QVBoxLayout* layout, QList<QGroupBox*>& groups);
};

#endif
