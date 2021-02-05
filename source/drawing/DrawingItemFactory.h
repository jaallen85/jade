// DrawingItemFactory.h
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

#ifndef DRAWINGITEMFACTORY_H
#define DRAWINGITEMFACTORY_H

#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>

class DrawingItem;
class QXmlStreamReader;
class QXmlStreamWriter;

class DrawingItemFactory : public QObject
{
	Q_OBJECT

private:
	QList<DrawingItem*> mItems;
	QHash<QString,QVariant> mDefaultItemProperties;

public:
	DrawingItemFactory();
	~DrawingItemFactory();

	bool registerItem(DrawingItem* item);
	void clearItems();
	bool containsItem(const QString& uniqueKey) const;
	DrawingItem* createItem(const QString& uniqueKey) const;

	QString writeItemsToString(const QList<DrawingItem*>& items) const;
	QList<DrawingItem*> readItemsFromString(const QString xmlString) const;

	void writeItemsToXml(QXmlStreamWriter* xml, const QList<DrawingItem*>& items) const;
	QList<DrawingItem*> readItemsFromXml(QXmlStreamReader* xml) const;

public slots:
	void setDefaultItemProperties(const QHash<QString,QVariant>& properties);
public:
	QHash<QString,QVariant> defaultItemProperties() const;
};

#endif
